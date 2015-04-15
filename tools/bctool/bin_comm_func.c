/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 * License: General Public License (GPL v3)
 *
 */

/*****************************************************************************/

#include "bin_comm_func.h"

/*****************************************************************************/

/**!\brief
 *
 */
void bc_init()
{
    time_old = time(0);

    cmd_bytesToRead[BC_CMD_GET_AMBI_STATUS]     = BC_CMD_GET_AMBI_STATUS_RECV_BYTE;
    cmd_bytesToRead[BC_CMD_TVP_GET_STATUS]      = BC_CMD_TVP_GET_STATUS_RECV_BYTE;
    cmd_bytesToRead[BC_CMD_ADV_GET_STATUS]      = BC_CMD_ADV_GET_STATUS_RECV_BYTE;

    cmd_bytesToRead[BC_CMD_SET_AMBI_STATUS]     = BC_CMD_SET_AMBI_STATUS_RECV_BYTE;
    cmd_bytesToRead[BC_CMD_SET_LOGLEVEL]        = BC_CMD_SET_LOGLEVEL_RECV_BYTE;
    cmd_bytesToRead[BC_CMD_SET_SIG_SEARCH]      = BC_CMD_SET_SIG_SEARCH_RECV_BYTE;
    cmd_bytesToRead[BC_CMD_SET_SLEEP_TOUT]      = BC_CMD_SET_SLEEP_TOUT_RECV_BYTE;

    cmd_bytesToRead[BC_CMD_SET_NC_RGB]          = BC_CMD_SET_NC_RGB_RECV_BYTE;
    cmd_bytesToRead[BC_CMD_SET_OFFSET_RGB]      = BC_CMD_SET_OFFSET_RGB_RECV_BYTE;
    cmd_bytesToRead[BC_CMD_SET_MAX_RGB]         = BC_CMD_SET_MAX_RGB_RECV_BYTE;
    cmd_bytesToRead[BC_CMD_SET_BRIGHTNESS]      = BC_CMD_SET_BRIGHTNESS_RECV_BYTE;

    cmd_bytesToRead[BC_CMD_SET_LPF]             = BC_CMD_SET_LPF_RECV_BYTE;

    cmd_bytesToRead[BC_CMD_TVP_NEXT_INPUT]      = BC_CMD_TVP_NEXT_INPUT_RECV_BYTE;
    cmd_bytesToRead[BC_CMD_DSP_SEL]             = BC_CMD_DSP_SEL_RECV_BYTE;

    cmd_bytesToRead[BC_CMD_RESET_AVR]           = BC_CMD_RESET_AVR_RECV_BYTE;
    cmd_bytesToRead[BC_CMD_RESET_TO_BTLDR_AVR]  = BC_CMD_RESET_TO_BTLDR_AVR_RECV_BYTE;

    cmd_bytesToRead[BC_CMD_GET_VERSION_INFO]    = BC_CMD_GET_VERSION_INFO_RECV_BYTE;

    cmd_bytesToRead[BC_CMD_SET_SIM_YCBCR_DATA]  = BC_CMD_SET_SIM_YCBCR_DATA_RECV_BYTE;

    cmd_bytesToRead[BC_CMD_GET_DSP_SEL]         = BC_CMD_GET_DSP_SEL_RECV_BYTE;

    cmd_bytesToRead[BC_CMD_SET_CHANNEL_LED_NUM] = BC_CMD_SET_CHANNEL_LED_NUM_RECV_BYTE;
    cmd_bytesToRead[BC_CMD_SET_CHANNEL_DATA]    = BC_CMD_SET_CHANNEL_DATA_RECV_BYTE;

}

/*****************************************************************************/
/*!\brief
 *
 */
void pollCommFSM()
{

#ifdef DEBUG
    if (bytesRead > 0)
        printf("%d Bytes were read:%s\n",bytesRead,readbuffer);
#endif

/*#ifdef DEBUG
        if (bytesRead > 0)
            printf("t%d ct%d ambi%d tvp%d adv%d ans%d\n", time_old, comm_timeout, waitForAmbiStatus, waitForTVPStatus, waitForADVStatus, waitForAnswer);
#endif*/

    if (bytesRead > 0)
    {

        unsigned char c = readbuffer[0];
        static unsigned int data[25]; // incoming data buffer
        static unsigned int read_pos = 0;
        static unsigned int bytesToReceive = 0;
        unsigned int px, py = 0;
        unsigned int r, g, b = 0;

#ifdef DEBUG
        char c2 = c;
        if (c2 <= 32)
            c2 = 32;
        printf("s: 0x%02x c: 0x%02x = %c\r\n", msgState, c, c2);           
#endif

        switch (msgState)
        {
            case BC_STATE_READ_FIRST_CHAR:;
                // reset the variables
                bytesToReceive = 0;
                read_pos =  0;
                type     = -1;
                status   = -1;
                cmd      = -1;
                nextStateToProcessData = BC_STATE_READ_FIRST_CHAR;
                if (c == BC_READER_FIRST_START_CHAR)
                    msgState = BC_STATE_READ_SECOND_CHAR;
                else
                    msgState = BC_STATE_READ_FIRST_CHAR;
                break;

            case BC_STATE_READ_SECOND_CHAR:;
                if (c == BC_READER_SECOND_START_CHAR)
                    msgState = BC_STATE_READ_TYPE;
                else
                    msgState = BC_STATE_READ_FIRST_CHAR;
                break;

            case BC_STATE_READ_TYPE:;
                type = c;
                if (type == BC_OUT_MSG_TYPE_STATUS) 
                    msgState = BC_STATE_READ_STATUS;
                else if (type == BC_OUT_MSG_TYPE_ANSWER) 
                    msgState = BC_STATE_READ_ANSWER;
                else if (type == BC_OUT_MSG_TYPE_YCBCR_FRAME_DATA)
                    msgState = BC_STATE_READ_YCBCR_FRAME_DATA;
                else if (type == BC_OUT_MSG_TYPE_RGB_LED_DATA)
                    msgState = BC_STATE_READ_RGB_LED_DATA;
                else
                    msgState = BC_STATE_READ_FIRST_CHAR;
                break;

            /**************************************************************/
            case BC_STATE_READ_ANSWER:;
                cmd = c;

                if (cmd >= BC_CMD_CNT)
                    msgState = BC_STATE_READ_FIRST_CHAR; // command is invalid, abort

                msgState = BC_STATE_READ_ANSWER_DATA;

                bytesToReceive = cmd_bytesToRead[cmd];

                break;

            case BC_STATE_READ_ANSWER_DATA:;
#ifdef DEBUG
                printf("btr: %d\r\n", bytesToReceive);
#endif
                if (bytesToReceive > 0)
                {
                    data[read_pos] = c;
                    read_pos++;
                    bytesToReceive--;
#ifdef DEBUG
                    printf("rp: %d 0x%02x\r\n", read_pos, data[read_pos-1]);
#endif
                    if (bytesToReceive == 0)
                    {
                        msgState = BC_STATE_READ_FIRST_END_CHAR;
                        nextStateToProcessData = BC_STATE_PROCESS_ANSWER_DATA;
                    }
                } else {
                    msgState = BC_STATE_READ_FIRST_END_CHAR;
                    nextStateToProcessData = BC_STATE_PROCESS_ANSWER_DATA;
                }
                
                break;

            /**************************************************************/
            case BC_STATE_READ_STATUS:;
                status = c;
                msgState = BC_STATE_READ_FIRST_END_CHAR;
                nextStateToProcessData = BC_STATE_PROCESS_STATUS;
                break;
            
            /**************************************************************/

            case BC_STATE_READ_YCBCR_FRAME_DATA:;
                data[0] = (unsigned int)(c << 8);
                msgState = BC_STATE_READ_YCBCR_FRAME_DATA1;
                break;

            case BC_STATE_READ_YCBCR_FRAME_DATA1:;
                data[0] += (unsigned int)c;
                msgState = BC_STATE_READ_YCBCR_FRAME_DATA2;
                break;

            case BC_STATE_READ_YCBCR_FRAME_DATA2:;
                data[1] = c;
                msgState = BC_STATE_READ_YCBCR_FRAME_DATA3;
                break;

            case BC_STATE_READ_YCBCR_FRAME_DATA3:;
                data[2] = c;
                msgState = BC_STATE_READ_YCBCR_FRAME_DATA4;
                break;

            case BC_STATE_READ_YCBCR_FRAME_DATA4:;
                data[3] = c;
                msgState = BC_STATE_READ_FIRST_END_CHAR;
                nextStateToProcessData = BC_STATE_PROCESS_YCBCR_FRAME_DATA;
                break;

            /**************************************************************/
            case BC_STATE_READ_FIRST_END_CHAR:;
#ifdef DEBUG
                printf("ns: 0x%02x\r\n", nextStateToProcessData);
#endif
                if (c == BC_READER_FIRST_END_CHAR)
                    // skip next state because the newline is removed by the read function
                    msgState = BC_STATE_READ_SECOND_END_CHAR;
                else
                    msgState = BC_STATE_READ_FIRST_CHAR;
                break;

            case BC_STATE_READ_SECOND_END_CHAR:;
                if (c == BC_READER_SECOND_END_CHAR)
                {
                    msgState = nextStateToProcessData;

#ifdef DEBUG
                    printf("Processing: 0x%02x 0x%02x\r\n", msgState, nextStateToProcessData);
#endif
                }
                else
                    msgState = BC_STATE_READ_FIRST_CHAR;
                break;

            /**************************************************************/
            case BC_STATE_READ_RGB_LED_DATA:;
                data[0] = (unsigned int)(c << 8);
                msgState = BC_STATE_READ_RGB_LED_DATA1;
                break;

            case BC_STATE_READ_RGB_LED_DATA1:;
                data[0] += (unsigned int)c;
                msgState = BC_STATE_READ_RGB_LED_DATA2;
                break;

            case BC_STATE_READ_RGB_LED_DATA2:;
                data[1] = c;
                msgState = BC_STATE_READ_RGB_LED_DATA3;
                break;

            case BC_STATE_READ_RGB_LED_DATA3:;
                data[2] = c;
                msgState = BC_STATE_READ_RGB_LED_DATA4;
                break;

            case BC_STATE_READ_RGB_LED_DATA4:;
                data[3] = c;
                msgState = BC_STATE_READ_FIRST_END_CHAR;
                nextStateToProcessData = BC_STATE_PROCESS_RGB_LED_DATA;
                break;

            default:;
                msgState = BC_STATE_READ_FIRST_CHAR;

        } // end of first msgState switch

#ifdef DEBUG
        if (nextStateToProcessData != BC_STATE_READ_FIRST_CHAR)
            printf("stp: 0x%02x s: 0x%02x\r\n", nextStateToProcessData, msgState);
        
        printf("m: 0x%02x\r\n", msgState);
#endif

        // state machine to process received data
        switch (msgState)
        {

            case BC_STATE_PROCESS_ANSWER_DATA:;

#ifdef DEBUG
                printf("cmd = 0x%02x; read bytes %d; data [", cmd, read_pos);
                int i = 0;
                for (i = 0; i < read_pos; i++)
                {
                    printf("0x%02x", data[i]);
                    if (i < read_pos-1)
                        printf(", ");
                }
                printf("]\n");
#endif

                // hier die Daten entsprechend von der variable "cmd" verarbeiten
                if      (cmd == BC_CMD_ADV_GET_STATUS)
                {
                    printf("\nADV7611 status:\n===============\n");
                    printf("Hotplug detected 0x%02x\n", data[0]);
                    printf("ROM version      0x%02x%02x\n", data[2], data[1]);
                    waitForADVStatus = 0;
                }
                else if (cmd == BC_CMD_TVP_GET_STATUS)
                {
                    printf("\nTVP5148 status:\n===============\n");
                    printf("Status Register 1     0x%02x\n", data[0]);
                    printf("Status Register 2     0x%02x\n", data[1]);
                    printf("Video Status Register 0x%02x\n", data[2]);
                    printf("ROM version           0x%02x\n", data[3]);
                    waitForTVPStatus = 0;
                }
                else if (cmd == BC_CMD_GET_AMBI_STATUS)
                {
                    printf("\nAmbiController status:\n======================\n");
                    printf("Color processing is active %d\n", data[0]);
                    printf("Input signal detected      %d\n", data[1]);
                    printf("Brightness                 0x%02x\n", data[2]);
                    printf("RGB max. values            0x%02x 0x%02x 0x%02x\n",
                           data[3], data[4], data[5]);
                    printf("RGB offset values          0x%02x 0x%02x 0x%02x\n",
                           data[6], data[7], data[8]);
                    printf("No signal RGB values       0x%02x 0x%02x 0x%02x\n",
                           data[9], data[10], data[11]);
                    printf("Low pass filter C          0x%04x\n",
                           (data[12] + (data[13] << 8)));
                    printf("LogLevel                   %d\n", data[14]);
                    printf("Search signal flag         %d\n", data[15]);
                    printf("Sleep timeout              0x%04x\n",
                           (data[17] + (data[16] << 8)));
                    printf("FPGA YCbCr data simulation %d\n", data[18]);
                    waitForAmbiStatus = 0;
                }
                else if (cmd == BC_CMD_GET_VERSION_INFO)
                {
                    printf("\nVersion information:\n====================\n");
                    printf("Hardware revision:       %c%c%c%c%c%c%c%c\n",
                           data[0], data[1], data[2], data[3],
                           data[4], data[5], data[6], data[7]);
                    printf("Firmware version:           %c%c%c%c%c\n",
                           data[8], data[9], data[10],
                           data[11], data[12]);
                    printf("Binary protocol version:    %c%c%c%c%c\n",
                           data[13], data[14], data[15],
                           data[16], data[17]);
                    printf("Bootloader protocol:       %c%c%c%c%c%c\n",
                           data[18], data[19], data[20],
                           data[21], data[22], data[23]);
                    waitForAnswer = 0;
                }
                else if (cmd == BC_CMD_GET_DSP_SEL)
                {
                    printf("\nDSP input selection:\n====================\n");
                    printf("Used DSP: ");
                    if (data[0] == 1)
                    {
                        printf("ADV7611\n");
                    } else {
                        printf("TVP5146\n");
                    }
                    waitForAnswer = 0;
                }
                else if (data[0] == BC_ANSWER_OK)
                { // ok

                    waitForAnswer = 0; // TODO sicherstellen das auf das richtige kommando gehört wird
                } // ELSE not ok

                msgState = BC_STATE_READ_FIRST_CHAR;
                break;

            /**************************************************************/
            case BC_STATE_PROCESS_STATUS:;

                printf("status = 0x%02x\n", status);
                msgState = BC_STATE_READ_FIRST_CHAR;
                break;

            /**************************************************************/
#ifdef ENABLE_SDL
            case BC_STATE_PROCESS_YCBCR_FRAME_DATA:;

                py = floor(data[0] / FRAME_WIDTH);
                px = data[0] - (py * FRAME_WIDTH);
                int y  = data[1];
                int cb = data[2];
                int cr = data[3];

                unsigned int tmpYCbCr = convertRGB(y,cb,cr);
                r =  (tmpYCbCr & 0x0000ff);
                g = ((tmpYCbCr & 0x00ff00) >>  8);
                b = ((tmpYCbCr & 0xff0000) >> 16);

                if (gui_enabled && py < FRAME_WIDTH-1)
                {
                    gui_show_YCbCr_pixel(px, py, r, g, b);
                }

                msgState = BC_STATE_READ_FIRST_CHAR;
                break;

            /**************************************************************/
            case BC_STATE_PROCESS_RGB_LED_DATA:;

                if (gui_enabled)
                {
                    gui_show_RGB_pixel(data[0], data[1], data[2], data[3]);
                }

                msgState = BC_STATE_READ_FIRST_CHAR;

                break;
#endif

        } // end of second msgState switch
    }
}

/*****************************************************************************/

void bc_setFPGAYCbCrSimulation(int s)
{
    if (s < 0 || s > 1)
        return;

    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c%c",
                    BC_READER_FIRST_START_CHAR,
                    BC_READER_SECOND_START_CHAR,
                    BC_CMD_SET_SIM_YCBCR_DATA,
                    (char)(s & 0x01),
                    BC_READER_FIRST_END_CHAR,
                    BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 6);

    waitForAnswer = 1;
}

void bc_setSearchSignal(int s)
{
    if (s < 0 || s > 1)
        return;

    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c%c",
                    BC_READER_FIRST_START_CHAR,
                    BC_READER_SECOND_START_CHAR,
                    BC_CMD_SET_SIG_SEARCH,
                    (char)(s & 0x01),
                    BC_READER_FIRST_END_CHAR,
                    BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 6);

    waitForAnswer = 1;

}

void bc_setSleepTimeout(int t)
{
    if (t < 0 || t > 65535)
        return;

    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c%c%c",
                    BC_READER_FIRST_START_CHAR,
                    BC_READER_SECOND_START_CHAR,
                    BC_CMD_SET_SLEEP_TOUT,
                    (char) (t & 0xFF),
                    (char)((t & 0xFF00) >> 8),
                    BC_READER_FIRST_END_CHAR,
                    BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 7);

    waitForAnswer = 1;

}

void bc_nextTVPInput()
{
    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c",
                    BC_READER_FIRST_START_CHAR,
                    BC_READER_SECOND_START_CHAR,
                    BC_CMD_TVP_NEXT_INPUT,
                    BC_READER_FIRST_END_CHAR,
                    BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 5);
    
    waitForAnswer = 1;
}

void bc_resetAVR_to_Bootloader()
{
    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c",
                    BC_READER_FIRST_START_CHAR,
                    BC_READER_SECOND_START_CHAR,
                    BC_CMD_RESET_TO_BTLDR_AVR,
                    BC_READER_FIRST_END_CHAR,
                    BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 5);
    
    waitForAnswer = 1;
}

void bc_resetAVR()
{
    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c",
                    BC_READER_FIRST_START_CHAR,
                    BC_READER_SECOND_START_CHAR,
                    BC_CMD_RESET_AVR,
                    BC_READER_FIRST_END_CHAR,
                    BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 5);
    
    waitForAnswer = 1;
}

void bc_setNCRGB(int r, int g, int b)
{
    if (r < 0 || r > 255)
        return;
    if (g < 0 || g > 255)
        return;
    if (b < 0 || b > 255)
        return;

    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c%c%c%c",
                  BC_READER_FIRST_START_CHAR,
                  BC_READER_SECOND_START_CHAR,
                  BC_CMD_SET_NC_RGB,
                  (char)(r & 0xFF),
                  (char)(g & 0xFF),
                  (char)(b & 0xFF),
                  BC_READER_FIRST_END_CHAR,
                  BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 8);
    
    waitForAnswer = 1;
}

void bc_setMaxRGB(int r, int g, int b)
{
    if (r < 0 || r > 255)
        return;
    if (g < 0 || g > 255)
        return;
    if (b < 0 || b > 255)
        return;

    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c%c%c%c",
                  BC_READER_FIRST_START_CHAR,
                  BC_READER_SECOND_START_CHAR,
                  BC_CMD_SET_MAX_RGB,
                  (char)(r & 0xFF),
                  (char)(g & 0xFF),
                  (char)(b & 0xFF),
                  BC_READER_FIRST_END_CHAR,
                  BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 8);
    
    waitForAnswer = 1;
}

void bc_setOffsetRGB(int r, int g, int b)
{
    if (r < 0 || r > 255)
        return;
    if (g < 0 || g > 255)
        return;
    if (b < 0 || b > 255)
        return;

    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c%c%c%c",
                  BC_READER_FIRST_START_CHAR,
                  BC_READER_SECOND_START_CHAR,
                  BC_CMD_SET_OFFSET_RGB,
                  (char)(r & 0xFF),
                  (char)(g & 0xFF),
                  (char)(b & 0xFF),
                  BC_READER_FIRST_END_CHAR,
                  BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 8);
    
    waitForAnswer = 1;
}

void bc_setLPFC(int c)
{
    if (c < 0 || c > 0xFFFF)
        return;

    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c%c%c",
                  BC_READER_FIRST_START_CHAR,
                  BC_READER_SECOND_START_CHAR,
                  BC_CMD_SET_LPF,
                  (char)((c & 0xFF00)>>8),
                  (char) (c &   0xFF),
                  BC_READER_FIRST_END_CHAR,
                  BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 7);
    
    waitForAnswer = 1;
}

void bc_setBrightness(int b)
{
    if (b < 0 || b > 255)
        return;

    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c%c",
                  BC_READER_FIRST_START_CHAR,
                  BC_READER_SECOND_START_CHAR,
                  BC_CMD_SET_BRIGHTNESS,
                  (char)(b & 0xFF),
                  BC_READER_FIRST_END_CHAR,
                  BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 6);
    
    waitForAnswer = 1;
}

void bc_setAmbiStatus(int s)
{
    if (s < 0 || s > 1)
        return;

    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c%c",
                    BC_READER_FIRST_START_CHAR,
                    BC_READER_SECOND_START_CHAR,
                    BC_CMD_SET_AMBI_STATUS,
                    (char)(s & 0xFF),
                    BC_READER_FIRST_END_CHAR,
                    BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 6);
    
    waitForAnswer = 1;
}

void bc_setLogLevel(int l)
{
    if (l < 0 || l > 255)
        return;

    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c%c",
                  BC_READER_FIRST_START_CHAR,
                  BC_READER_SECOND_START_CHAR,
                  BC_CMD_SET_LOGLEVEL,
                  (char)(l & 0xFF),
                  BC_READER_FIRST_END_CHAR,
                  BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 6);
    
    waitForAnswer = 1;
}

void bc_getAmbiStatus(void)
{
    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c",
                  BC_READER_FIRST_START_CHAR,
                  BC_READER_SECOND_START_CHAR,
                  BC_CMD_GET_AMBI_STATUS,
                  BC_READER_FIRST_END_CHAR,
                  BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 5);
    
    waitForAmbiStatus = 1;
}

void bc_getADVStatus(void)
{
    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c",
                  BC_READER_FIRST_START_CHAR,
                  BC_READER_SECOND_START_CHAR,
                  BC_CMD_ADV_GET_STATUS,
                  BC_READER_FIRST_END_CHAR,
                  BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 5);
    
    waitForADVStatus = 1;
}

void bc_getTVPStatus(void)
{
    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c",
                  BC_READER_FIRST_START_CHAR,
                  BC_READER_SECOND_START_CHAR,
                  BC_CMD_TVP_GET_STATUS,
                  BC_READER_FIRST_END_CHAR,
                  BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 5);

    waitForTVPStatus = 1;
}


void bc_getVersionInformation(void)
{
    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c",
                  BC_READER_FIRST_START_CHAR,
                  BC_READER_SECOND_START_CHAR,
                  BC_CMD_GET_VERSION_INFO,
                  BC_READER_FIRST_END_CHAR,
                  BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 5);

    waitForAnswer = 1;

}

void bc_setDSPInSW(int s)
{
    if (s < 0 || s > 1)
        return;

    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c%c",
                    BC_READER_FIRST_START_CHAR,
                    BC_READER_SECOND_START_CHAR,
                    BC_CMD_DSP_SEL,
                    (char)(s & 0xFF),
                    BC_READER_FIRST_END_CHAR,
                    BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 6);
    
    waitForAnswer = 1;
}

void bc_getDSPSel(void)
{
    char tmsg[10] = "";
    sprintf(tmsg, "%c%c%c%c%c",
                  BC_READER_FIRST_START_CHAR,
                  BC_READER_SECOND_START_CHAR,
                  BC_CMD_GET_DSP_SEL,
                  BC_READER_FIRST_END_CHAR,
                  BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 5);

    waitForAnswer = 1;
}

void bc_setChannelLEDNum(int a, int b, int c, int d, int e)
{
    char tmsg[20] = "";
    sprintf(tmsg, "%c%c%c%c%c%c%c%c%c%c",
                  BC_READER_FIRST_START_CHAR,
                  BC_READER_SECOND_START_CHAR,
                  BC_CMD_SET_CHANNEL_LED_NUM,
                  (char)(a & 0xFF),
                  (char)(b & 0xFF),
                  (char)(c & 0xFF),
                  (char)(d & 0xFF),
                  (char)(e & 0xFF),
                  BC_READER_FIRST_END_CHAR,
                  BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 10);

    waitForAnswer = 1;
}

void bc_sendChannelConfigData(int page, unsigned int *data, int len)
{
    if (len > 32/2)
        return; // 16 cells allowed max

    char tmsg[20] = "";
    sprintf(tmsg, "%c%c%c%c",
                  BC_READER_FIRST_START_CHAR,
                  BC_READER_SECOND_START_CHAR,
                  BC_CMD_SET_CHANNEL_DATA,
                  (char)(page & 0xFF));
    SERIAL_WRITE(tmsg, 4);

#ifdef DEBUG
    printf("page: %d\n", (char)(page & 0xFF));
#endif

    // send all data
    for (int pos = 0; pos < len; pos++)
    {
        // byte order: low, than high
#ifdef DEBUG
        printf("0x%02x 0x%02x ", (data[pos] & 0xff), ((data[pos] >> 8) && 0xff));
#endif
        sprintf(tmsg, "%c%c", (data[pos] & 0xff), ((data[pos] >> 8) && 0xff));
        SERIAL_WRITE(tmsg, 2);
    }

    if (len < 32/2)
    {
        // fill 32 byte buffer with dummy data

#ifdef DEBUG
        printf("- ");
#endif
        for (int pos = 0; pos < (32/2) - len; pos++)
        {
#ifdef DEBUG
            printf("0xff 0xff ");
#endif
            sprintf(tmsg, "%c%c", 0xff, 0xff);
            SERIAL_WRITE(tmsg, 2);
        }
    }

#ifdef DEBUG
        printf("\n");
#endif

    sprintf(tmsg, "%c%c",
                  BC_READER_FIRST_END_CHAR,
                  BC_READER_SECOND_END_CHAR);
    SERIAL_WRITE(tmsg, 2);

    waitForAnswer = 1;
}

/*****************************************************************************/


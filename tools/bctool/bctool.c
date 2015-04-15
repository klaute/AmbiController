/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 * License: General Public License (GPL v3)
 *
 * The SDL and serial port access code is mostly copied from several websites:
 * http://en.wikibooks.org/wiki/Serial_Programming/termios
 * http://123a321.wordpress.com/2010/02/01/serial-port-with-mingw/
 * http://www.amigalien.de/AMIGAProjekt/help/sdl/index.htm
 *
 */

/*****************************************************************************/

#include "bctool.h"

/*****************************************************************************/

#include "serial_helper.c"
#include "gui_help.c"
#include "bin_comm_func.c"
#include "gui_helper.c"

/*****************************************************************************/

#ifdef USE_THREADS
void* dataReaderThreadFunction(void *arg)
{
    pthread_t id = pthread_self();
    retDataReader = 0;

    while (!dataReaderThreadExit)
    {
        pthread_mutex_lock(&mutex1);

        sh_readDataFromUSART();

        pollCommFSM();

        pthread_mutex_unlock(&mutex1);

        // wait some time to prevent deadlock
        usleep(DELAY_READ_POLL_FSM_US);

    }

    retDataReader = 1;
    pthread_exit(&retDataReader);

    return NULL;
}
#endif

/*****************************************************************************/

void init()
{

    // Disable buffering for stdout and stderr
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    // set the number of bytes to read as data bytes from AmbiController
    bc_init();
    
}

/*****************************************************************************/

/*
 * Main function.
 */
int main(int argc, char **argv)
{

    init();

    /*********************/

    struct arg_str *param_sport       = arg_str0("p", "port",     "[Device name]", "Serial port");
#ifdef ENABLE_SDL
    struct arg_lit *param_withgui     = arg_lit0("g", "gui",      "Start tool with GUI");

    struct arg_lit *param_helpgui     = arg_lit0("H", "gui-help", "Print GUI help");
#endif
    struct arg_lit *param_help        = arg_lit0("h", "help",     "Print commandline help");

    struct arg_lit *param_gambis      = arg_lit0("S", "ambis",    "Get AmbiConroller status");
    struct arg_int *param_sambi       = arg_int0("s", "sambi",    "0=off|1=on", "Set color processing activity");

    struct arg_int *param_simFPGAData = arg_int0("T", "simdata",  "0=off|1=on", "Enable/disable simulation of YCbCr data");

    struct arg_lit *param_gtvps       = arg_lit0("t", "tvps",     "Get TVP status");
    struct arg_lit *param_gadvs       = arg_lit0("a", "advs",     "Get ADV status");

    struct arg_lit *param_ravr        = arg_lit0("r", "rambi",    "Reset AmbiController");
    struct arg_lit *param_ravrb       = arg_lit0("R", "rambib",   "Reset AmbiController for firmware update");

    struct arg_lit *param_tvpni       = arg_lit0("i", "tvpni",    "Skip to next input");
    struct arg_int *param_ssig        = arg_int0("I", "ssig",     "0=off|1=on", "Set the search signal flag");

    struct arg_str *param_ncrgb       = arg_str0("n", "ncrgb",    "0xRR,0xGG,0xBB", "Set no color RGB values");
    struct arg_str *param_maxrgb      = arg_str0("m", "maxrgb",   "0xRR,0xGG,0xBB", "Set maximum RGB values");
    struct arg_str *param_offrgb      = arg_str0("o", "offrgb",   "0xRR,0xGG,0xBB", "Set offset RGB values");
    struct arg_int *param_bri         = arg_int0("b", "bri",      "[0x00-0xff]", "Set brightness");
    struct arg_int *param_lpfc        = arg_int0("l", "lpfc",     "[0x0000-0xffff]", "Set low pass filter constant");

    struct arg_int *param_sleep       = arg_int0("P", "sleep",    "0-65535", "Set sleep timer if no input signal is detected or the color processing is disabled for NN seconds");

    struct arg_lit *param_getver      = arg_lit0("V", "vinfo",    "Get AmbiController version information");

    struct arg_lit *param_gdsps       = arg_lit0("d", "dspget",   "Get the selected input DSP information");
    struct arg_int *param_sdsps       = arg_int0("D", "dspset",   "0=TVP|1=ADV", "Set the selected input DSP information");

    struct arg_str *param_sendLEDCfg  = arg_str0("L", "ledcfg",   "<File name>", "Send a LED configuration file");

    struct arg_str *param_usbcon      = arg_lit0("U", "usb",      "Connected with USB");

    struct arg_int *param_loglvl      = arg_int0("O", "loglvl",   "0-255", "Set the log level");

    struct arg_end *param_end         = arg_end(25);

    // default parameter
    //param_sport->sval = "";

    // argtable array (with used commandline parameter objects)
    void* argtable[] = {
        param_sport,
#ifdef ENABLE_SDL
        param_withgui,
        param_helpgui,
#endif
        param_help,
        param_gambis,
        param_gadvs,
        param_gtvps,
        param_ravr,
        param_ravrb,
        param_tvpni,
        param_ncrgb,
        param_maxrgb,
        param_offrgb,
        param_bri,
        param_sambi,
        param_simFPGAData,
        param_lpfc,
        param_ssig,
        param_sleep,
        param_getver,
        param_gdsps,
        param_sdsps,
        param_sendLEDCfg,
        param_usbcon,
        param_loglvl,
        param_end
    };

    // process parameter check
    if (arg_nullcheck(argtable) != 0)
    {
        fprintf(stderr, "error: insufficient memory\n");
        arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
        return 0;
    }

    // parse the commandline arguments
    int nerrors = arg_parse(argc,argv,argtable);

    if (nerrors == 0 && param_help->count > 0)
    {
        printf("\nusage: ./bctool <OPTIONS>\n");
        arg_print_glossary(stdout, argtable, "\t%-25s %s\n");
        arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

#ifndef WIN32
        printf("\n");
#endif

        return 0;
    }

#ifdef ENABLE_SDL
    /*********************/
    if (param_helpgui->count > 0)
    {
        showGUIHelp();
        return 0;
    }
#endif

    // parsing failed or to less parameter given
    if (nerrors > 0
        || ( param_sport->count       >  0 && 
#ifdef ENABLE_SDL
             param_withgui->count     == 0 &&
             param_helpgui->count     == 0 &&
#endif
             param_gambis->count      == 0 &&
             param_gadvs->count       == 0 &&
             param_gtvps->count       == 0 &&
             param_ravr->count        == 0 &&
             param_ravrb->count       == 0 &&
             param_tvpni->count       == 0 &&
             param_ncrgb->count       == 0 &&
             param_maxrgb->count      == 0 &&
             param_offrgb->count      == 0 &&
             param_bri->count         == 0 &&
             param_sambi->count       == 0 &&
             param_simFPGAData->count == 0 &&
             param_lpfc->count        == 0 &&
             param_ssig->count        == 0 &&
             param_sleep->count       == 0 &&
             param_getver->count      == 0 &&
             param_gdsps->count       == 0 &&
             param_sendLEDCfg->count  == 0 &&
             param_usbcon->count      == 0 &&
             param_loglvl->count      == 0 &&
             param_sdsps->count       == 0)
        || param_sport->count == 0)
    {
        arg_print_errors(stdout,param_end,"bctool");
        printf("\nusage: ./bctool <OPTIONS>\n");

        arg_print_glossary(stdout, argtable, "\t%-25s %s\n");
        arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

#ifndef WIN32
        printf("\n");
#endif

        return -20;
    }

#ifdef ENABLE_SDL
    /*********************/
    if (param_helpgui->count > 0)
    {
        showGUIHelp();
    }
#endif

#ifdef DEBUG
    printf("Open serial port %s\n", param_sport->sval[0]);
#endif

    /*********************/
    // serial port parameter
    if (param_sport->count > 0)
    {
        if (sh_openSerialPort(param_sport->sval[0], param_usbcon->count) != 0)
            return -10; // cant open serial port
    }

#ifdef USE_THREADS
    /*********************/
    // start USART reader/fsm thread
    int threadErr = pthread_create(&tdataReceiver, NULL, &dataReaderThreadFunction, NULL);
    if (threadErr != 0)
    {
        fprintf(stderr, "Error, can't create thread :[%s]", strerror(threadErr));
        exit_all();
        return -1;
#ifdef DEBUG
    } else {
        printf("Thread created successfully\n");
#endif
    }
#endif // USE THREADS


#ifdef ENABLE_SDL
    /*********************/
    // start GUI parameter
    if (param_withgui->count > 0)
    {
        // GUI is enabled now, all other parameter are ignored
        printf("Starting the GUI, all other aprameter will be ignored.\n");
        gui_enabled = 1;
    } else {
#endif
        sleep(1);

        // parameter welche bei geöffneter gui nicht interessieren
        if (param_tvpni->count > 0)
        {
            printf("Skip to next TVP input source\n");
            bc_nextTVPInput();
        }
        if (param_ravr->count > 0)
        {
            printf("Reset AmbiController\n");
            bc_resetAVR();
        }
        if (param_ravrb->count > 0)
        {
            printf("Reset AmbiController for firmware update\n");
            bc_resetAVR_to_Bootloader();
        }
        if (param_gtvps->count > 0)
        {
            printf("Get TVP5148 status\n");
            bc_getTVPStatus();
        }
        if (param_gadvs->count > 0)
        {
            printf("Get ADV7611 status\n");
            bc_getADVStatus();
        }
        if (param_gambis->count > 0)
        {
            printf("Get ambilight status\n");
            bc_getAmbiStatus();
        }
        if (param_ncrgb->count > 0)
        {
            int r, g, b;
            r = 0;
            g = 0;
            b = 0;
            if (sscanf(param_ncrgb->sval[0], "0x%02x,0x%02x,0x%02x", &r, &g, &b))
            {
                printf("Set no color RGB values (0x%02x 0x%02x 0x%02x)\n", r, g, b);
                bc_setNCRGB(r, g, b);
            } else {
                fprintf(stderr, "Error: Color value \'$%s\' is invalid.\nShould be \"0x12,0x34,0x56\"\n", param_ncrgb->sval[0]);
    	        return -2;
            }
        }
        if (param_maxrgb->count > 0)
        {
            int r, g, b;
            r = 0;
            g = 0;
            b = 0;
            if (sscanf(param_maxrgb->sval[0], "0x%02x,0x%02x,0x%02x", &r, &g, &b))
            {
                printf("Set max. RGB values (0x%02x 0x%02x 0x%02x)\n", r, g, b);
                bc_setMaxRGB(r, g, b);
            } else {
                fprintf(stderr, "Error: Color value \'$%s\' is invalid.\nShould be \"0x12,0x34,0x56\"\n", param_maxrgb->sval[0]);
    	        return -3;
            }
        }
        if (param_offrgb->count > 0)
        {
            int r, g, b;
            r = 0;
            g = 0;
            b = 0;
            if (sscanf(param_offrgb->sval[0], "0x%02x,0x%02x,0x%02x", &r, &g, &b))
            {
                printf("Set max. RGB values (0x%02x 0x%02x 0x%02x)\n", r, g, b);
                bc_setOffsetRGB(r, g, b);
            } else {
                fprintf(stderr, "Error: Color value \'$%s\' is invalid.\nShould be \"0x12,0x34,0x56\".\n", param_offrgb->sval[0]);
    	        return -4;
            }
        }
        if (param_loglvl->count > 0)
        {
            int l = param_loglvl->ival[0];

            if (l >= 0 && l < 255)
            {
                printf("Set the log level\n");
                bc_setLogLevel(l);
            } else {
                fprintf(stderr, "Error: Invalid parameter value.\nLogLevel range is 0x00 to 0xff.\n");
                return -5;
            }
        }
        if (param_bri->count > 0)
        {
            int b = param_bri->ival[0];

            if (b >= 0 && b < 255)
            {
                printf("Set the brightness\n");
                bc_setBrightness(b);
            } else {
                fprintf(stderr, "Error: Invalid parameter value.\nBrightness range is 0x00 to 0xff.\n");
                return -5;
            }
        }
        if (param_sambi->count > 0)
        {
            int s = param_sambi->ival[0];

            if (s == 0)
            {
                printf("Disable color processing\n");
                bc_setAmbiStatus(s);
            } else if (s == 1) {
                printf("Enable color processing\n");
                bc_setAmbiStatus(s);
            } else {
                fprintf(stderr, "Error: Invalid parameter value.\nAllowed values are 0=off and 1=on.\n");
                return -6;
            }
        }
        if (param_simFPGAData->count > 0)
        {
            int s = param_simFPGAData->ival[0];

            if (s == 0)
            {
                printf("Disable FPGA YCbCr simulation\n");
                bc_setFPGAYCbCrSimulation(s);
            } else if (s == 1) {
                printf("Enable FPGA YCbCr simulation\n");
                bc_setFPGAYCbCrSimulation(s);
            } else {
                fprintf(stderr, "Error: Invalid parameter value.\nAllowed values are 0=off and 1=on.\n");
                return -7;
            }
        }
        if (param_lpfc->count > 0)
        {
            int l = param_lpfc->ival[0];

            if (l >= 0 && l <= 0xffff)
            {
                printf("Set the low pass filter C\n");
                bc_setLPFC(l);
            } else {
                fprintf(stderr, "Error: Invalid parameter value.\nLow pass filter C range is 0x0000 to 0xffff.\n");
                return -8;
            }
        }
        if (param_ssig->count > 0)
        {
            int l = param_ssig->ival[0];
            
            if (l == 1 || l == 0)
            {
                printf("Set the search signal flag\n");

                bc_setSearchSignal(l);
            }
        }
        if (param_getver->count > 0)
        {
            printf("Get AmbiController version information\n");
            bc_getVersionInformation();
        }
        if (param_sleep->count > 0)
        {
            int l = param_sleep->ival[0];
            
            if (l >=0 && l <= 65535)
            {
                printf("Set the sleep timeout\n");
            
                bc_setSleepTimeout(l);
            }
        }
        if (param_sdsps->count > 0)
        {
            int s = param_sdsps->ival[0];

            if (s == 0)
            {
                printf("Enable TVP5146 usage\n");
                bc_setDSPInSW(0);
            } else if (s == 1) {
                printf("Enable ADV7611 usage\n");
                bc_setDSPInSW(1);
            } else {
                fprintf(stderr, "Error: Invalid parameter value.\nAllowed values are 0 and 1.\n");
                return -9;
            }
        }
        if (param_gdsps->count > 0)
        {
            printf("Get DSP selection\n");
            bc_getDSPSel();
        }
        if (param_sendLEDCfg->count > 0)
        {
            printf("Send LED channel config file: %s\n", param_sendLEDCfg->sval[0]);

            readAndSendLEDChannelCfg(param_sendLEDCfg->sval[0]);
        }
#ifdef ENABLE_SDL
    }
#endif

    /*********************/
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

#ifdef ENABLE_SDL
    /*********************/
    if (gui_enabled)
    {
        gui_init_window();

    }
#endif // ENABLE_SDL

    /*********************/
    while(running)
    {

#ifdef ENABLE_SDL
        gui_pollSDLWindowFunctionality();
#endif // ENABLE_SDL

#ifndef USE_THREADS
        sh_readDataFromUSART();
        pollCommFSM(); // process communication
#endif

        // TODO hier prüfen ob die alle informationen empfangen wurden, die abgefragt werden sollten.

        if (!gui_enabled && waitForAmbiStatus == 0 && waitForTVPStatus == 0 && waitForADVStatus == 0 && waitForAnswer == 0)
            running = 0;

        if (!gui_enabled && (waitForAmbiStatus == 1 || waitForTVPStatus == 1 || waitForADVStatus == 1 || waitForAnswer == 1))
        {
            time_t t = time(0);
            if (t > time_old)
            {
                time_old = time(0);
                comm_timeout++; // one second is gone
            }
            if (comm_timeout >= COMMUNICATION_TIMEOUT_SEC)
            {
                fprintf(stderr, "Error: Communication timeout!\n");
                running = 0;
            }
        }

    } // end of main loop

    exit_all();

    return 0;

}

/*****************************************************************************/

void readAndSendLEDChannelCfg(char *filename)
{

    unsigned int channel_A[CHANNEL_A_LED_MAXIMUM];
    unsigned int channel_B[CHANNEL_B_LED_MAXIMUM];
    unsigned int channel_C[CHANNEL_C_LED_MAXIMUM];
    unsigned int channel_D[CHANNEL_D_LED_MAXIMUM];
    unsigned int channel_E[CHANNEL_E_LED_MAXIMUM];
    unsigned int channel_A_num;
    unsigned int channel_B_num;
    unsigned int channel_C_num;
    unsigned int channel_D_num;
    unsigned int channel_E_num;

    // 1. open file
    FILE *fh = fopen(filename, "r");
    if (fh == NULL)
    {
        fprintf(stderr, "ERROR: Can't open input file!\n");
        exit (-1000);
    }

    // 2. read file into data structure
    int c = 0; // input character
    int pos_inline = 0;
    int line_cnt = 0;
    char line[400];

    while((c = fgetc(fh)) != EOF)
    {
        if (c != '\n')
        {
            line[pos_inline] = c;
            pos_inline++;
        } else {
            // copy the read line into the data structure
            line[pos_inline] = '\0'; // close string

#ifdef DEBUG
            printf("Line %d: \"%s\"\n", line_cnt, line);
#endif

            int pos_inconfig = 0;
            char *tstr;
            char *tmpstr;
            // read channel name from the line (first character in line)
            char *channel_name = strtok_r(line, ":", &tmpstr);

#ifdef DEBUG
            printf("Channel %s: ", channel_name);
#endif

            // read line configuration data
            for (tstr = tmpstr; ; tstr = NULL)
            {
                // read the next value from the config data
                char *value = strtok_r(tstr, ",", &tmpstr);
                if (value != NULL)
                {
                    unsigned int ival = 0;
                    sscanf(value, "%d", &ival);

                    switch(channel_name[0])
                    {
                        case 'A':;
                            channel_A[pos_inconfig] = ival;
                            channel_A_num = pos_inconfig + 1;
#ifdef DEBUG
                            //printf("%d 0x%02x%02x, ", ((channel_A[pos_inconfig] >> 8) & 0xff), (channel_A[pos_inconfig] & 0xff));
                            printf("%d ", channel_A[pos_inconfig]);
#endif
                            break;
                        case 'B':;
                            channel_B[pos_inconfig] = ival;
                            channel_B_num = pos_inconfig + 1;
#ifdef DEBUG
                            printf("%d ", channel_B[pos_inconfig]);
#endif
                            break;
                        case 'C':;
                            channel_C[pos_inconfig] = ival;
                            channel_C_num = pos_inconfig + 1;
#ifdef DEBUG
                            printf("%d ", channel_C[pos_inconfig]);
#endif
                            break;
                        case 'D':;
                            channel_D[pos_inconfig] = ival;
                            channel_D_num = pos_inconfig + 1;
#ifdef DEBUG
                            printf("%d ", channel_D[pos_inconfig]);
#endif
                            break;
                        case 'E':;
                            channel_E[pos_inconfig] = ival;
                            channel_E_num = pos_inconfig + 1;
#ifdef DEBUG
                            printf("%d ", channel_E[pos_inconfig]);
#endif
                            break;
                        default:;
                            break;
                    }

                    //printf("%s = %d ", value, channel_A[pos_inconfig]);
                    //printf("%s = %d ", value, ival);

                    pos_inconfig++;
                } else {
#ifdef DEBUG
                    printf("\n");
#endif
                    break;
                }
            }

            pos_inconfig = 0;
            pos_inline = 0;
            line_cnt++;
        }
    }

    // 3. send channel led numbers
    printf("Channel LED numbers: A = %d; B = %d; C = %d; D = %d; E = %d\n",
            channel_A_num, channel_B_num, channel_C_num,
            channel_D_num, channel_E_num);
    bc_setChannelLEDNum(channel_A_num, channel_B_num, channel_C_num,
                        channel_D_num, channel_E_num);

    // 4. send channel data pagewise (32 byte per page)

    //----------- Send channel A data -----------//
    printf("Send LED channel data: A");
    int page = 0;
    unsigned int pages = ceil((float)channel_A_num / 16);
#ifdef DEBUG
    printf("pages = %d\n", pages);
#endif

    for (int s = 0; s < channel_A_num; s += 16)
    {
        unsigned int len = 16;

#ifdef DEBUG
        printf("s = %d %d\n", s, pages);
#endif

        if (channel_A_num - ((page) * 16) < 16)
        {
            len = channel_A_num - ((page) * 16);
        }
#ifdef DEBUG
        printf("len = %d\n", len);
#endif
        bc_sendChannelConfigData(EEPROM_CHANNEL_A_DATA_PAGE + page,
                                 &(channel_A[s]),
                                 len);
        page++;
    }
#ifdef DEBUG
    printf("\n");
#endif

    //----------- Send channel B data -----------//
    printf(", B");
    page = 0;
    pages = ceil((float)channel_B_num / 16);
#ifdef DEBUG
    printf("pages = %d\n", pages);
#endif

    for (int s = 0; s < channel_B_num; s += 16)
    {
        unsigned int len = 16;

#ifdef DEBUG
        printf("s = %d %d\n", s, pages);
#endif

        if (channel_B_num - ((page) * 16) < 16)
        {
            len = channel_B_num - ((page) * 16);
        }
#ifdef DEBUG
        printf("len = %d\n", len);
#endif
        bc_sendChannelConfigData(EEPROM_CHANNEL_B_DATA_PAGE + page,
                                 &channel_B[s],
                                 len);
        page++;
    }
#ifdef DEBUG
    printf("\n");
#endif

    //----------- Send channel C data -----------//
    printf(", C");
    page = 0;
    pages = ceil((float)channel_C_num / 16);
#ifdef DEBUG
    printf("pages = %d\n", pages);
#endif

    for (int s = 0; s < channel_C_num; s += 16)
    {
        unsigned int len = 16;

#ifdef DEBUG
        printf("s = %d %d\n", s, pages);
#endif

        if (channel_C_num - ((page) * 16) < 16)
        {
            len = channel_C_num - ((page) * 16);
        }
#ifdef DEBUG
        printf("len = %d\n", len);
#endif
        bc_sendChannelConfigData(EEPROM_CHANNEL_C_DATA_PAGE + page,
                                 &channel_C[s],
                                 len);
        page++;
    }
#ifdef DEBUG
    printf("\n");
#endif

    //----------- Send channel D data -----------//
    printf(", D");
    page = 0;
    pages = ceil((float)channel_D_num / 16);
#ifdef DEBUG
    printf("pages = %d\n", pages);
#endif

    for (int s = 0; s < channel_D_num; s += 16)
    {
        unsigned int len = 16;

#ifdef DEBUG
        printf("s = %d %d\n", s, pages);
#endif

        if (channel_D_num - ((page) * 16) < 16)
        {
            len = channel_D_num - ((page) * 16);
        }
#ifdef DEBUG
        printf("len = %d\n", len);
#endif
        bc_sendChannelConfigData(EEPROM_CHANNEL_D_DATA_PAGE + page,
                                 &channel_D[s],
                                 len);
        page++;
    }
#ifdef DEBUG
    printf("\n");
#endif

    //----------- Send channel E data -----------//
    printf(", E\n");
    page = 0;
    pages = ceil((float)channel_E_num / 16);
#ifdef DEBUG
    printf("pages = %d\n", pages);
#endif

    for (int s = 0; s < channel_E_num; s += 16)
    {
        unsigned int len = 16;

#ifdef DEBUG
        printf("s = %d %d\n", s, pages);
#endif

        if (channel_E_num - ((page) * 16) < 16)
        {
            len = channel_E_num - ((page) * 16);
        }
#ifdef DEBUG
        printf("len = %d\n", len);
#endif
        bc_sendChannelConfigData(EEPROM_CHANNEL_E_DATA_PAGE + page,
                                 &channel_E[s],
                                 len);
        page++;
    }

    // 5. close file
    fclose(fh);

    // reset the AVR after channe data hes been sent
    bc_resetAVR();

}

/*****************************************************************************/

/*!\brief Color convertion function.
 * \param y the y-component
 * \param cb the cb-component
 * \param cr the cr-component
 * \param y the y-component
 * \return The 3 color components as a 32 bit unsigned integer
 */
int convertRGB(int y, int cb, int cr)
{

    // standard conversion between YCbCr and RGB (both 8bit)
    float yratio = (float)298.082     *(y - 16);
    float      r = (float)  0.00390625*(yratio +    408.583 *(cr - 128));
    float      g = (float)  0.00390625*(yratio + (- 100.291)*(cb - 128) + (- 208.120)*(cr - 128));
    float      b = (float)  0.00390625*(yratio +    516.411 *(cb - 128));

    // test for allowed range of RGB values and fix them if required
    if (r > 255.0) r = 255.0;
    if (g > 255.0) g = 255.0;
    if (b > 255.0) b = 255.0;
    if (r < 0.0)   r =   0.0;
    if (g < 0.0)   g =   0.0;
    if (b < 0.0)   b =   0.0;

    uint32_t rgb  =   (uint8_t) r;
             rgb += (((uint16_t)g) << 8);
             rgb += (((uint32_t)b) << 16);

    return rgb & 0x00ffffff;

}

/*****************************************************************************/

/*!\brief Exit function to call during exit of the application.
 *
 */
void exit_all(void)
{

#ifdef USE_THREADS
    int ptr;
    dataReaderThreadExit = 1;

    if (!pthread_join(tdataReceiver, (void**)(&ptr)))
        pthread_exit(tdataReceiver); // stop thread in case that the join was not successfull
#endif

    sh_closeSerialPort();

#ifdef ENABLE_SDL
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
#endif

}

/*****************************************************************************/


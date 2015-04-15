/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 */

/******************************************************************************/

#include "fpga_flash.h"

/******************************************************************************/

void init()
{

    time_now = time(NULL);

    // Disable buffering for stdout and stderr
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

}

/******************************************************************************/

int main(int argc, char **argv)
{

    init();
    
    struct arg_str *param_sport     = arg_str0("p", "port",   "[Device name]", "Serial port");
    struct arg_lit *param_full_read = arg_lit0("R", "fread",  "Read FPGA design from flash (full flash dump)");
    struct arg_lit *param_read      = arg_lit0("r", "read",   "Read FPGA design from flash (0x000000 to specified address with --addr parameter)");
    struct arg_lit *param_write     = arg_lit0("w", "write",  "Write FPGA design to flash");
    struct arg_lit *param_help      = arg_lit0("h", "help",   "Print out the commandline help");
    struct arg_str *param_laddr     = arg_str0("l", "addr",   "[0x1FFFFF max.]", "Last page address");
                    param_file      = arg_str0("f", "file",   "[File name]", "Input/Output file");
                    param_invbit    = arg_lit0("i", "invbit", "Invert the bit order byte wise to SPI flash or output file");
                    param_shsp      = arg_lit0("s", "sspeed", "Show speed information");
    struct arg_end *param_end       = arg_end(25);

    // argtable array (with used commandline parameter objects)
    void* argtable[] = {
        param_sport,
        param_help,
        param_full_read,
        param_read,
        param_write,
        param_file,
        param_invbit,
        param_shsp,
        param_laddr,
        param_end
    };

    // process parameter check
    if (arg_nullcheck(argtable) != 0)
    {
        fprintf(stderr, "ERROR: Nicht ausreichend Arbeitsspeicher vorhanden.\n");
        arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
        return 0;
    }

    // parse the commandline arguments
    int nerrors = arg_parse(argc,argv,argtable);

    if (nerrors == 0 && param_help->count > 0)
    {
        printf("\nusage: fpga_flash <OPTIONS>\n");
        arg_print_glossary(stdout, argtable, "\t%-25s %s\n");
        arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

#ifndef WIN32
        printf("\n");
#endif

        return 0;
    }

    //========================================================================//
    // parsing failed or to less parameter given
    if (nerrors > 0 // some error happened
        || ( param_sport->count > 0 && param_full_read->count == 0 && param_write->count == 0 && param_read->count == 0) // serial device name given, but no action defined
        ||   param_sport->count == 0 // no serial device name given
        || ( param_sport->count > 0 && (param_read->count > 0 || param_full_read->count > 0 || param_write->count > 0) && param_file->count == 0 ) // rw action enabled but no file name given
        || ( param_sport->count > 0 &&  param_read->count > 0 && param_laddr->count == 0 ) // read til specified address enabled but no last address given
       )
    {
        arg_print_errors(stdout,param_end,"fpga_flash");
        printf("\nusage: fpga_flash <OPTIONS>\n");

        arg_print_glossary(stdout, argtable, "\t%-25s %s\n");
        arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

#ifndef WIN32
        printf("\n");
#endif

        exit(-10);
    }

    //========================================================================//
    // serial port parameter
    if (param_sport->count > 0)
    {
        if (sh_openSerialPort(param_sport->sval[0], 0) != 0)
            exit(-20); // cant open serial port

        // hier die transceiveJicFile Funktion auf dem AVR-TTY aktivieren
        // 1. TTY kommando senden
        SERIAL_WRITE("sftfc\r\n", 6);
        printf("Entering TFC mode... ");

        // 2. warten auf OK (0x01)
        wfo = waitForOK();
        if        (wfo == WAIT_FOR_OK_FOUND_FLAG )
        {
            printf("OK\n");
        } else if (wfo == WAIT_FOR_OK_TIMEOUT_FLAG)
        { // timeout
            fprintf(stderr, "\nERROR: Can't set AmbiController into TFC mode, timeout happened.\n");
            sh_closeSerialPort();
            exit(-100);
        } else if (wfo == WAIT_FOR_OK_NOT_FOUND_FLAG )
        {
            fprintf(stderr, "NOT OK\n");
            sh_closeSerialPort();
            exit(-150);
        } else {
            fprintf(stderr, "\nERROR: Can't set AmbiController into TFC mode, unknown error happened (0x%04x).\n", wfo);
            sh_closeSerialPort();
            exit(-200);
        }

    }

    //========================================================================//
    // the "last address" parameter will be ignored in modes which were different than write and read
    if ((param_write->count > 0 || param_read->count > 0) && param_laddr->count > 0)
    {
        unsigned char addr[3];
        if (sscanf(param_laddr->sval[0], "0x%02x%02x%02x", &addr[0], &addr[1], &addr[2]))
        {
            unsigned int tmp = (addr[0] << 16) + (addr[1] << 8) + addr[2];
            if (tmp <= FLASH_LAST_ADDRESS)
            {
                flash_last_address = (addr[0] << 16) + (addr[1] << 8) + addr[2];
                printf("New last address is 0x%06x\n", flash_last_address);
            } else {
                fprintf(stderr, "ERROR: The last address value is greater than 0x%06x!\n", FLASH_LAST_ADDRESS);
                exit(-250);
            }
        }
    } else if (param_full_read->count > 0 && param_laddr->count > 0)
    { // last page parameter is ignored in full read mode
        printf("New last address parameter will be ignored in full read mode.\n");
    }

    //========================================================================//
    // write flash operation
    if (param_write->count > 0 && param_file->count > 0)
    {
        writeFlashContent();

    //========================================================================//
    // specified read operation
    } else if (param_read->count > 0 && param_file->count > 0 && param_laddr->count > 0)
    {
        readFlashContent();

    //========================================================================//
    // full read operation
    } else if (param_full_read->count > 0 && param_file->count > 0)
    {
        readFullFlashContent();
    }

    //========================================================================//0
    // TFC mode beenden
    // 1. sende END (0xff)
    char tmp[1];
    tmp[0] = SPI_FLASH_CONTENT_CMD_END;
    SERIAL_WRITE(tmp, 1);
    printf("Exiting TFC mode... ");

    // 2. warten auf OK
    wfo = waitForOK();
    if (wfo == WAIT_FOR_OK_TIMEOUT_FLAG)
    {
        fprintf(stderr, "\nERROR: TFC mode can't be disabled: timeout happened\n");
        sh_closeSerialPort();
        exit(-300);
    } else if (wfo == WAIT_FOR_OK_FOUND_FLAG) {
        printf("OK\n");
    } else {
        fprintf(stderr, "NOT OK: 0x%04x\n", wfo);
    }

    printf("done\n");
    sh_closeSerialPort();

    return 0;

}

/******************************************************************************/

unsigned int waitForOK()
{

    int found   = 0; // not found
    int timeout = 0;

    while (timeout < WAIT_FOR_OK_TIMEOUT_COUNTS && found == 0)
    {
        sh_readDataFromUSART();

        if (bytesRead > 0)
        {
            if (readbuffer[0] == SPI_FLASH_CONTENT_ANSWER_OK)
            {
                found = WAIT_FOR_OK_FOUND_FLAG;
            } else if (readbuffer[0] == SPI_FLASH_CONTENT_ANSWER_NOK)
            {
                found = WAIT_FOR_OK_NOT_FOUND_FLAG;
            } else {
                found = WAIT_FOR_OK_UNKNOWN_FLAG + (readbuffer[0] & 0xff);
            }
        }
        timeout++;
    }

    if (timeout >= WAIT_FOR_OK_TIMEOUT_COUNTS)
        found = WAIT_FOR_OK_TIMEOUT_FLAG;

    return found;

}

/******************************************************************************/

uint8_t invert_bit_order(uint8_t b)
{
    uint8_t inv = 0;
    
    inv = ((b & 0x01) << 7)
        | ((b & 0x02) << 5)
        | ((b & 0x04) << 3)
        | ((b & 0x08) << 1)
        | ((b & 0x10) >> 1)
        | ((b & 0x20) >> 3)
        | ((b & 0x40) >> 5)
        | ((b & 0x80) >> 7);
    
    return inv;
}

/******************************************************************************/

void update_byte_per_second()
{
    if (time(NULL) <= time_now)
    {
        byte_per_sec++;
    } else {
        byte_per_sec_avg += byte_per_sec;
        byte_per_sec_avg  = byte_per_sec_avg / 2;
        byte_per_sec      = 0;
        time_now          = time(NULL);
    }
}

/******************************************************************************/

void readFullFlashContent()
{
    char tmp[1];

    // 1. read modus starten
    tmp[0] = SPI_FLASH_CONTENT_CMD_RD_FLASH;
    SERIAL_WRITE(tmp, 1);
    printf("Start to read flash content...\n");

    if (waitForOK() == WAIT_FOR_OK_FOUND_FLAG)
    { // read modus aktiviert

        tmp[0] = SPI_FLASH_CONTENT_CMD_RD_FLASH;
        SERIAL_WRITE(tmp, 1); // send random byte to start the data transfer

        // 2. datei öffnen für ausgabe (falls vorhanden abbrechen)
        printf("Opening output file: %s\n", param_file->sval[0]);
        FILE *fh = fopen(param_file->sval[0], "w");
        if (fh == NULL)
        {
            fprintf(stderr, "ERROR: Can't open output file!");
            sh_closeSerialPort();
            exit(-3000);
        }
        setvbuf(fh, NULL, _IONBF, 0); // disable output buffering

        unsigned char in_buffer[FLASH_PAGE_SIZE+ADDRESS_SIZE_BYTE]; // 256 byte page data and 3 byte flash address

        unsigned int addr     = 0x000000;
        unsigned int done     = 0;
        unsigned int state    = SPI_FLASH_CONTENT_STATE_WAIT_FOR_SOD;
        unsigned int byte_cnt = 0; // byte counter
        unsigned int page_cnt = 0;
        unsigned int timeout  = READ_DATA_TIMEOUT_COUNTS;

        // 3. loop bis die komplette Page empfangen wurde (0x000000 .. 0x1fffff)
        while (!done && timeout > 0)
        {
            timeout--;
            sh_readDataFromUSART();

            if (bytesRead > 0)
            {

                timeout = READ_DATA_TIMEOUT_COUNTS; // reset timeout
                unsigned char inchar = readbuffer[0] & 0xff;

                // 4. hier die empfangenen Daten auswerten, falls vorhanden (bytesRead > 0)
                switch(state)
                {
                    case SPI_FLASH_CONTENT_STATE_WAIT_FOR_SOD:; // warten auf SOD
                        if (inchar == SPI_FLASH_CONTENT_MSG_SOD)
                        {
                            state = SPI_FLASH_CONTENT_STATE_READ_USART_DATA;
                        }
                        break;

                    case SPI_FLASH_CONTENT_STATE_READ_USART_DATA:; 

                        in_buffer[byte_cnt] = inchar; // speichern in puffer

                        // hier prüfen ob ein Fehler aufgetreten ist SPI_FLASH_CONTENT_MSG_ERR gefolgt von SPI_FLASH_CONTENT_ANSWER_NOK
                        /*if (byte_cnt > 3 && // 0, 1, 2 sind die adress-bytes und +1 da zwei aufeinander folgende bytes beachtet werden müssen
                            in_buffer[byte_cnt-1] == SPI_FLASH_CONTENT_MSG_ERR &&
                            in_buffer[byte_cnt]   == SPI_FLASH_CONTENT_ANSWER_NOK)
                        {
                            // TODO was ist wenn im flash die daten in genau dieser folge stehen? also 0xEE 0xFF,
                            // dann wird hier ein Fehler erkannt obwohl keiner vorliegt. An dieser Stelle sollte eine Prüfung durchgeführt werden welche
                            // erkennen kann das die daten korrekt sind.
                            // abort read operation, timeout happened
                            fprintf(stderr, "\nERROR: Timeout happend during page read operation!\n");
                            sh_closeSerialPort();
                            fclose(fh);
                            exit(-3050);
                        } else {*/
                        
                            printf("Page: %04d - Addr: ", page_cnt+1);

                            if (byte_cnt >= 2)
                            { // print out address

                                printf("0x%02x%02x%02x ", in_buffer[0], in_buffer[1], in_buffer[2]);
                                addr = (in_buffer[0] << 16) + (in_buffer[1] << 8) + in_buffer[2];

                            } else {
                                printf("0x000000 ");
                            }

                            if (param_shsp->count > 0)
                            {
                                update_byte_per_second();
								PRINTF_BYTE_PER_SC_AVG;
                            } else {
                                printf("       \r");
                            }

                            if (byte_cnt+1 == FLASH_PAGE_SIZE + ADDRESS_SIZE_BYTE)
                            { // all data bytes received
                                state = SPI_FLASH_CONTENT_STATE_WAIT_FOR_EOD;
                            }
                        //} // prüfung ob der AVR beim lesen der Page einen timeout gemeldet hat
                            
                        byte_cnt++;
                        break;

                    case SPI_FLASH_CONTENT_STATE_WAIT_FOR_EOD:; // warten auf EOD

                        if (inchar == SPI_FLASH_CONTENT_MSG_EOD)
                        {

                            if (page_cnt == FLASH_PAGE_CNT-1)
                            {
                                // last page received
                                done = 1;
                                
                                // print out last read information
                                printf("\rPage: %04d - Addr: ", page_cnt+1);
                                printf("0x%02x%02x%02x \r", in_buffer[0], in_buffer[1], in_buffer[2]);
                                //printf("                   \n"); // clear speed information
                            } else {
                                // more pages to receive
                                byte_cnt = 0;
                                page_cnt++;
                            }

                            // write page to file
                            int ti = 0;
                            for (ti = 0; ti < FLASH_PAGE_SIZE; ti++)
                            {
                                // 256 byte daten, nacheinander (und hexadezimal), in die datei schreiben, adresse beachten (ersten 3 byte)
                                if (param_invbit->count > 0)
                                {
                                    // invertierte und binäre ausgabe in datei
                                    fprintf(fh, "%c", invert_bit_order(in_buffer[ti+ADDRESS_SIZE_BYTE]));
                                } else {
                                    // binäre ausgabe in datei
                                    fprintf(fh, "%c",                  in_buffer[ti+ADDRESS_SIZE_BYTE] );
                                }
                            }

                            state = SPI_FLASH_CONTENT_STATE_WAIT_FOR_SOD;

                        /*} else if (inchar == SPI_FLASH_CONTENT_ANSWER_NOK &&
                                   in_buffer[ADDRESS_SIZE_BYTE+FLASH_PAGE_SIZE-1] == SPI_FLASH_CONTENT_MSG_ERR)
                        { // timeout happened
                            fprintf(stderr, "\nERROR: Timeout happened during pare read!\n");
                            sh_closeSerialPort();
                            fclose(fh);
                            exit(-3100);*/
                        } else {
                            fprintf(stderr, "\nERROR: End of data message not received (0x%02x)!\n", inchar);
                            sh_closeSerialPort();
                            fclose(fh);
                            exit(-3150);
                        }

                        break;

                    default:;
                        state = SPI_FLASH_CONTENT_STATE_WAIT_FOR_SOD;
                        done = 1;
                } // end of switch
            } // readBytes

        } // while

        // 5. datei schließen
        fclose(fh);

        // 6. warten auf ein OK vom AmbiController
        wfo = waitForOK();
        if        (wfo == WAIT_FOR_OK_TIMEOUT_FLAG)
        {
            fprintf(stderr, "\nERROR: Timeout happend after read page was finished and all Data were received.\nWARNING: The read data may be inconsistent...\n");
            sh_closeSerialPort();
            exit(-3200);
        } else if (wfo == WAIT_FOR_OK_FOUND_FLAG)
        {
            printf("\nRead of flash content done\n");
        } else {
            fprintf(stderr, "\nERROR: No OK received (0x%04x)\n", wfo);
            sh_closeSerialPort();
            exit(-3250);
        }
    } else { // waitForOK erzeugte einen Fehler
        fprintf(stderr, "\nERROR: AmbiController is not in TFC mode (0x%04x).\n", wfo);
        sh_closeSerialPort();
        exit(-3300);
    }
}

/******************************************************************************/

void readFlashContent()
{
    char tmp[1];

    // 1. datei öffnen für ausgabe (falls vorhanden abbrechen)
    printf("Opening output file: %s\n", param_file->sval[0]);
    FILE *fh = fopen(param_file->sval[0], "w");
    if (fh == NULL)
    {
        fprintf(stderr, "ERROR: Can't open output file!");
        sh_closeSerialPort();
        exit(-2000);
    }
    setvbuf(fh, NULL, _IONBF, 0); // disable output buffering

    printf("Start to read flash content...\n");

    unsigned int page_cnt = 0;

    uint32_t addr = 0;
    // the lowest address byte should be 0xff to prevent a unwanted ent of the reading process
    for (addr = 0; addr < (flash_last_address | 0xff); addr += FLASH_PAGE_SIZE)
    {

        // 2. read modus starten
        tmp[0] = SPI_FLASH_CONTENT_CMD_RD_PAGE;
        SERIAL_WRITE(tmp, 1);

        // 3. wait for ok
        wfo = waitForOK();
        if (wfo == WAIT_FOR_OK_FOUND_FLAG)
        {

            unsigned char in_buffer[FLASH_PAGE_SIZE+ADDRESS_SIZE_BYTE]; // 256 byte page data and 3 byte flash address
            unsigned int done     = 0;
            unsigned int byte_cnt = 0;
            unsigned int state    = SPI_FLASH_CONTENT_STATE_WAIT_FOR_SOD;
            unsigned int timeout  = READ_DATA_TIMEOUT_COUNTS;

            // write the flash address to read from
            tmp[0] = ((addr >> 16) & 0xff);
            SERIAL_WRITE(tmp, 1);
            tmp[0] = ((addr >>  8) & 0xff);
            SERIAL_WRITE(tmp, 1);
            tmp[0] = (addr & 0xff);
            SERIAL_WRITE(tmp, 1);
                
            // 4. read page data
            while (!done && timeout > 0)
            {
                timeout--;

                sh_readDataFromUSART();

                if (bytesRead > 0)
                {
                    timeout = READ_DATA_TIMEOUT_COUNTS; // reset timeout

                    unsigned char inchar = readbuffer[0];
                    switch (state)
                    {
                        case SPI_FLASH_CONTENT_STATE_WAIT_FOR_SOD:; // 4. read SOD
                            if (inchar == SPI_FLASH_CONTENT_MSG_SOD)
                            {
                                state = SPI_FLASH_CONTENT_STATE_READ_USART_DATA;
                            }

                            break;

                        case SPI_FLASH_CONTENT_STATE_READ_USART_DATA:; // 5. 256 + 3 byte data

                            in_buffer[byte_cnt] = inchar; // speichern in puffer

                            printf("Page: %04d - Addr: ", page_cnt+1);

                            if (byte_cnt >= 2)
                            { // print out address
                                printf("0x%02x%02x%02x ", in_buffer[0], in_buffer[1], in_buffer[2]);
                                //addr = (in_buffer[0] << 16) + (in_buffer[1] << 8) + in_buffer[2];

                            } else {
                                printf("0x000000 ");
                            }

                            if (param_shsp->count > 0)
                            {
                                update_byte_per_second();
								PRINTF_BYTE_PER_SC_AVG;
                            } else {
                                printf("       \r");
                            }

                            if (byte_cnt+1 == FLASH_PAGE_SIZE + ADDRESS_SIZE_BYTE)
                            { // all data bytes received

                                state = SPI_FLASH_CONTENT_STATE_WAIT_FOR_EOD;
                            }

                            byte_cnt++;
                            break;

                        case SPI_FLASH_CONTENT_STATE_WAIT_FOR_EOD:; // 6. read eod

                            if (inchar == SPI_FLASH_CONTENT_MSG_EOD)
                            {

                                // last page received
                                done = 1;
                                    
                                // print out last read information
                                printf("\rPage: %04d - Addr: ", page_cnt+1);
                                printf("0x%02x%02x%02x \r", in_buffer[0], in_buffer[1], in_buffer[2]);
                                //printf("                   \r"); // clear byte information

                                page_cnt++;

                                // 5. write data to file
                                int ti = 0;
                                for (ti = 0; ti < FLASH_PAGE_SIZE; ti++)
                                {


                                    if ( addr  < (flash_last_address & 0xffff00) || // wenn die aktuell empfangene adresse noch nicht der endadresse entspricht
                                        (addr == (flash_last_address & 0xffff00) && ti < (flash_last_address & 0xff))) // wenn der zaehler ti noch unterhalb der maximalen anzahl der zu schreibenden daten in der letzten page ist
                                    {
                                        // 256 byte daten, nacheinander (und binär), in die datei schreiben, adresse beachten (ersten 3 byte)
                                        if (param_invbit->count > 0)
                                        {
                                            // invertierte und binäre ausgabe in datei
                                            fprintf(fh, "%c", invert_bit_order(in_buffer[ti+ADDRESS_SIZE_BYTE]));
                                        } else {
                                            // binäre ausgabe in datei
                                            fprintf(fh, "%c",                  in_buffer[ti+ADDRESS_SIZE_BYTE] );
                                        }
                                    } else {
                                        // die Daten werden verworfen
                                    }
                                }

                                state = SPI_FLASH_CONTENT_STATE_WAIT_FOR_SOD;
                            }
                            
                            break;

                        default:;
                            state = SPI_FLASH_CONTENT_STATE_WAIT_FOR_SOD;
                            done = 1;

                    } // switch
                } // if bytesRead > 0
            } // while done == 0
        } else {
            // TODO hier wfo auswerten wenn nicht OK
        } // wfo
    
    } // for

    printf("\n");
    fclose(fh);
}

/******************************************************************************/

void writeFlashContent()
{

    // 1. hier das flash content file öffnen fopen
    printf("Opening input file: %s\n", param_file->sval[0]);
    FILE *fh = fopen(param_file->sval[0], "r");
    if (fh == NULL)
    {
        fprintf(stderr, "ERROR: Can't open input file!\n");
        sh_closeSerialPort();
        exit(-1000);
    }

    // 2. datei größe ermitteln
    char tmp[1];

    // load file size
    struct stat tmp_st;
    stat(param_file->sval[0], &tmp_st);
    unsigned int se_addr = tmp_st.st_size;
    printf("File size is %lu byte\n", se_addr);
    flash_last_address = se_addr;

    // 3. hier dateigröße prüfen und ggfls. abbrechen
    if (se_addr > flash_last_address)
    {
        printf(stderr, "WARNING: File size is exceeds address limitation (0x%06x).\n", flash_last_address);
    }

    // 4. sektoren löschen: alle sektor-adressen einzeln löschen
    unsigned int ta = 0;
    for (ta = 0; ta < se_addr; ta += 0x10000)
    {
        printf("Erase flash sector address: 0x%06x\r", ta);
        tmp[0] = SPI_FLASH_CONTENT_CMD_SECTOR_ERASE;
        SERIAL_WRITE(tmp, 1);
        tmp[0] = ((ta >> 16) & 0xff); // address high byte
        SERIAL_WRITE(tmp, 1);
        tmp[0] = ((ta >> 8) & 0xff); // address middle byte
        SERIAL_WRITE(tmp, 1);
        tmp[0] = (ta & 0xff); // address low byte
        SERIAL_WRITE(tmp, 1);
        wfo = waitForOK(); // warten bis erase to sector das ok gesendet hat
        //printf("wfo = %d\n", wfo);
        wfo = waitForOK(); // warten bis erase to sector das ok gesendet hat
        //printf("wfo = %d\n", wfo);
    }
    printf("\n");

    if (wfo == WAIT_FOR_OK_NOT_FOUND_FLAG)
    {
        fprintf(stderr, "ERROR: Erase to sector failed, timeout happened.\n");
        sh_closeSerialPort();
        fclose(fh);
        exit(-1010);
    } else if (wfo == WAIT_FOR_OK_TIMEOUT_FLAG)
    {
        fprintf(stderr, "ERROR: Timeout happened during erase to sector operation.\n");
        sh_closeSerialPort();
        fclose(fh);
        exit(-1020);
    }

    // 5. Loop über 8192 x 256 byte blöcke der Zieldaten
    unsigned int addr      = 0;
    unsigned int eof_found = 0;

	// 6. senden des schreibe kommandos
	tmp[0] = SPI_FLASH_CONTENT_CMD_WR_FLASH;
	SERIAL_WRITE(tmp, 1);
	wfo = waitForOK();

	if (wfo == WAIT_FOR_OK_FOUND_FLAG)
	{
		while (eof_found == 0)
		{
			unsigned char out_buffer[FLASH_PAGE_SIZE];

            // 7. start of data senden
            tmp[0] = SPI_FLASH_CONTENT_MSG_SOD;
            SERIAL_WRITE(tmp, 1);
            wfo = waitForOK();
			//printf("SOD\n");

            if (wfo == WAIT_FOR_OK_FOUND_FLAG)
            {

                printf("                   \rAddr: 0x%02x%02x%02x", ((addr >> 16) & 0xff), ((addr >> 8) & 0xff), (addr & 0xff));

                // 8. adresse senden
                tmp[0] = ((addr >> 16) & 0xff); // address high byte
                SERIAL_WRITE(tmp, 1);
                tmp[0] = ((addr >> 8) & 0xff); // address middle byte
                SERIAL_WRITE(tmp, 1);
                tmp[0] = (addr & 0xff); // address low byte
                SERIAL_WRITE(tmp, 1);

                printf(".");
                
                int i = 0;
                for (i = 0; i < FLASH_PAGE_SIZE; i++)
                { // read/generate 265 byte of data
                    int in_c = 0;
                    if ((in_c = fgetc(fh)) != EOF)
                    {
                        out_buffer[i] = in_c;
                    } else {
						//printf("\nEOF\n");
						eof_found = 1;
                        out_buffer[i] = 0xff; // mit 0xff auffüllen
                    }
                }

                printf(".");

                // send data
                for (i = 0; i < FLASH_PAGE_SIZE; i++)
                {
                    // send every single byte
                    tmp[0] = out_buffer[i];
                    SERIAL_WRITE(tmp, 1);
                    //printf("%d ", i);
					if (param_shsp->count > 0)
					{
						update_byte_per_second();
					}
                }
                
                printf(".");
				
                // end of data senden
                tmp[0] = SPI_FLASH_CONTENT_MSG_EOD;
                SERIAL_WRITE(tmp, 1);

				addr += 0x100;

                printf(".");

				if (param_shsp->count > 0)
				{
					PRINTF_BYTE_PER_SC_AVG;
				} else {
					printf("       \r");
				}
				
                // warten auf OK für EOD
                wfo = waitForOK();
				
                if (wfo == WAIT_FOR_OK_FOUND_FLAG)
                {
					//printf("\nOK\n");
                    // do nothing in case that the OK was correctly received
                } else if (wfo == WAIT_FOR_OK_TIMEOUT_FLAG)
                {
                    fprintf(stderr, "ERROR: Timeout happened after sending EOD message footer.\n");
                    sh_closeSerialPort();
                    fclose(fh);
                    exit(-1050);
                } else if (wfo == WAIT_FOR_OK_NOT_FOUND_FLAG)
                {
                    fprintf(stderr, "ERROR: Data written but AmbiController returned NOT OK.\n");
                    sh_closeSerialPort();
                    fclose(fh);
                    exit(-1100);
                } else {
                    fprintf(stderr, "ERROR: Data written but AmbiController returned an unknown error (0x%04x).\n", wfo);
                    sh_closeSerialPort();
                    fclose(fh);
                    exit(-1150);
                } // wfo nach EOD

            } else if (wfo == WAIT_FOR_OK_TIMEOUT_FLAG)
            {
                fprintf(stderr, "ERROR: Timeout after sending SOD. (%d)\n", wfo);
                sh_closeSerialPort();
                fclose(fh);
                exit(-1151);
            } else if (wfo == WAIT_FOR_OK_NOT_FOUND_FLAG)
			{
                fprintf(stderr, "ERROR: NOT OK received after sending SOD. (%d)\n", wfo);
                sh_closeSerialPort();
                fclose(fh);
                exit(-1152);
            } else {
                fprintf(stderr, "ERROR: Unknown error received after SOD (0x%04x).\n", wfo);
                sh_closeSerialPort();
                fclose(fh);
                exit(-1153);
            } // wfo nach SOD
		} // while not eof
	} else if (wfo == WAIT_FOR_OK_TIMEOUT_FLAG)
	{
		fprintf(stderr, "ERROR: Timeout after sending the write command. (%d)\n", wfo);
		sh_closeSerialPort();
		fclose(fh);
		exit(-1154);
	} else if (wfo == WAIT_FOR_OK_NOT_FOUND_FLAG)
	{
		fprintf(stderr, "ERROR: NOT OK received after sending the write command. (%d)\n", wfo);
		sh_closeSerialPort();
		fclose(fh);
		exit(-1155);
	} else {
		fprintf(stderr, "ERROR: Unknown error received after sending the write command (0x%04x).\n", wfo);
		sh_closeSerialPort();
		fclose(fh);
		exit(-1156);
	} // wfo nach send WR

    // schreiben beenden
    printf("\nSend write done command...   \n");   
    tmp[0] = SPI_FLASH_CONTENT_CMD_WR_DONE;
    SERIAL_WRITE(tmp, 1);
    
    // warten auf OK für
    wfo = waitForOK();
    if (wfo == WAIT_FOR_OK_TIMEOUT_FLAG)
    {
        fprintf(stderr, "ERROR: Timeout happened after sending WRITE_DONE message.\n");
        sh_closeSerialPort();
        fclose(fh);
        exit(-1450);
    } else if (wfo == WAIT_FOR_OK_NOT_FOUND_FLAG)
    {
        fprintf(stderr, "ERROR: Data written but AmbiController returned NOT OK.\n");
        sh_closeSerialPort();
        fclose(fh);
        exit(-1500);
    }

    // 9. datei schließen
    fclose(fh);

}

/******************************************************************************/


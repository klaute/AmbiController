/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 */

/******************************************************************************/

#include "rtbt.h"

/******************************************************************************/

void init()
{

    // Disable buffering for stdout and stderr
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

}

/******************************************************************************/

int main(int argc, char **argv)
{

    init();
    
    struct arg_str *param_sport = arg_str0("p", "port",   "[Device name]", "Serial port");
    struct arg_end *param_end   = arg_end(25);

    // argtable array (with used commandline parameter objects)
    void* argtable[] = {
        param_sport,
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

    //========================================================================//
    // parsing failed or to less parameter given
    if (nerrors > 0 // some error happened
        || param_sport->count == 0
       )
    {
        arg_print_errors(stdout,param_end,"rtbt");
        printf("\nusage: rtbt <OPTIONS>\n");

        arg_print_glossary(stdout, argtable, "\t%-25s %s\n");
        arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

#ifndef WIN32
        printf("\n");
#endif

        exit(-10);
    }

    if (param_sport->count > 0)
    {
        if (sh_openSerialPort(param_sport->sval[0], 1) != 0)
            exit(-20); // cant open serial port

        printf("Resetting AmbiController to bootloader... ");

        // 1. TTY kommando senden
        SERIAL_WRITE("ravrb\r\n", 6);
        char *tstr = "          ";
        tstr[0] = '<';
        tstr[1] = '>';
        tstr[2] = 18; // reset to bootloader command
        tstr[3] = '\r';
        tstr[4] = '\n';
        tstr[5] = '\0';
        SERIAL_WRITE(tstr, 5);

        printf("done\n\n");

        sh_closeSerialPort();

    }

    return 0;

}

/******************************************************************************/


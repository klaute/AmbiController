/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 */

/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/stat.h>

#include <argtable2.h>

#if WIN32// || __CYGWIN__

#warning "Windows system..."

#include "serialport.h"

#else

#warning "Linux system..."
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#endif

#include "globals.h"

#include "flash_content_transceiver.h"

#include "serial_helper.c"

/******************************************************************************/

#define PRINTF_BYTE_PER_SC_AVG printf(" - %03d Byte/s             \r", byte_per_sec_avg);

/******************************************************************************/

unsigned int time_now;
unsigned int byte_per_sec       = 0;
unsigned int byte_per_sec_avg   = 0;
int          wfo                = 0;
unsigned int flash_last_address = FLASH_LAST_ADDRESS;

struct arg_lit *param_shsp;
struct arg_lit *param_invbit;
struct arg_str *param_file;

/******************************************************************************/

uint8_t      invert_bit_order(uint8_t);
unsigned int waitForOK(void);
void         init(void);
void         update_byte_per_second(void);
void    writeFlashContent(void);
void     readFlashContent(void);
void readFullFlashContent(void);

/******************************************************************************/



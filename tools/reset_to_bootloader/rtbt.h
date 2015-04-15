/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 */

/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
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

#include "serial_helper.c"

/******************************************************************************/

void         init(void);

/******************************************************************************/



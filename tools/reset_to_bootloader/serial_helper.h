/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 * License: General Public License (GPL v3)
 *
 */

/*****************************************************************************/

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

/*****************************************************************************/

#if WIN32
HANDLE sh_file_handle;
#else
int sh_file_handle;
#endif

/*****************************************************************************/

#if WIN32
  #ifdef USE_THREADS
#define SERIAL_WRITE_(M, S) writeToSerialPort(sh_file_handle, M, S);
  #else
#define SERIAL_WRITE(M, S) writeToSerialPort(sh_file_handle, M, S);
  #endif
#else
  #ifdef USE_THREADS
#define SERIAL_WRITE_(M, S) write(sh_file_handle, M, S);
  #else
#define SERIAL_WRITE(M, S) write(sh_file_handle, M, S);
  #endif
#endif

#ifdef USE_THREADS
// pthread implementation of SERIAL_WRITE (uses mutex lock/unlock functionality)
#define SERIAL_WRITE(M, S) \
    pthread_mutex_lock(&mutex1); \
    SERIAL_WRITE_(M, S); \
    pthread_mutex_unlock(&mutex1);
#endif

/*****************************************************************************/

#define INPUT_BUFFER_SIZE 2 //< 20 byte input buffer

/*****************************************************************************/

char readbuffer[INPUT_BUFFER_SIZE];
int bytesRead = 0;

/*****************************************************************************/

int  sh_openSerialPort(char*, int);
void sh_closeSerialPort(void);
void sh_readDataFromUSART(void);

#ifndef WIN32
int  sh_set_interface_attribs(int, int, int);
void sh_set_blocking(int, int);
#endif

/*****************************************************************************/


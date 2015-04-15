/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 * License: General Public License (GPL v3)
 *
 */

/*****************************************************************************/

#include "serial_helper.h"

/*****************************************************************************/

int sh_openSerialPort(char *sport, int usb_baud)
{

#if WIN32
    if (usb_baud <= 0)
    {
        sh_file_handle = openSerialPort(sport, SERIAL_HELPER_BAUDRATE, one, off);
    } else {
        sh_file_handle = openSerialPort(sport, SERIAL_HELPER_BAUDRATE_USB, one, off);
    }
#else
    sh_file_handle = open(sport, O_RDWR | O_NOCTTY | O_SYNC);
    if (sh_file_handle < 0)
    {
        fprintf(stderr, "Error %d opening %s: %s\n", errno, sport, strerror (errno));
        return 1;
    }
    if (usb_baud <= 0)
    {
        sh_set_interface_attribs(sh_file_handle, SERIAL_HELPER_BAUDRATE, 0); // set speed to SERIAL_HELPER_BAUDRATE bps, 8n1 (no parity)
    } else {
        sh_set_interface_attribs(sh_file_handle, SERIAL_HELPER_BAUDRATE_USB, 0); // set speed to SERIAL_HELPER_BAUDRATE_USB bps, 8n1 (no parity)
    }
    sh_set_blocking(sh_file_handle, 0);                   // enable non blocking read functionality
#endif
    return 0;

}

void sh_closeSerialPort()
{

#if WIN32
    // TODO disable video and led data sending
    closeSerialPort(sh_file_handle);
#else
    // TODO disable video and led data sending
    close(sh_file_handle);
#endif

}

/*****************************************************************************/

/*!\brief
 *
 */
void sh_readDataFromUSART()
{

#if WIN32
        //read something
        bytesRead = readFromSerialPort(sh_file_handle, readbuffer, sizeof(readbuffer)-1);
#else
        bytesRead = read(sh_file_handle, readbuffer, sizeof(readbuffer)-1);
#endif

        readbuffer[bytesRead] = 0;

}

/*****************************************************************************/


/*!\brief Windows specific function to set (non) blocking COM-port reading.
 *
 */
#ifndef WIN32
void sh_set_blocking(int sh_file_handle, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (sh_file_handle, &tty) != 0)
        {
                fprintf(stderr, "Error %d from tggetattr\n", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (sh_file_handle, TCSANOW, &tty) != 0)
                fprintf(stderr, "Error %d setting term attributes", errno);
}

/*!\brief Windows specific function to set COM-port attributes.
 *
 */
int sh_set_interface_attribs(int sh_file_handle, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (sh_file_handle, &tty) != 0)
        {
                fprintf(stderr, "Error %d from tcgetattr\n", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // ignore break signal
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (sh_file_handle, TCSANOW, &tty) != 0)
        {
                fprintf(stderr, "Error %d from tcsetattr\n", errno);
                return -1;
        }
        return 0;
}
#endif

/*****************************************************************************/


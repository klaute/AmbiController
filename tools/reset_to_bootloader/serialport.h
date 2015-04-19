#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <windows.h>

enum Baudrate
{
	B50	= 50,
	B110	= 110,
	B150	= 150,
	B300	= 300,
	B1200	= 1200,
	B2400	= 2400,
	B4800	= 4800,
	B9600   = 9600,
	B19200	= 19200,
	B38400	= 38400,
	B57600	= 57600,
	B115200 = 115200,
	B230400	= 230400,
	B460800	= 460800,
	B500000 = 500000,
	B921600 = 921600,
	B1000000= 1000000
};

enum Stopbits
{
	one = ONESTOPBIT,
	onePointFive = ONE5STOPBITS,
	two = TWOSTOPBITS
};

enum Paritycheck
{
	even = EVENPARITY,
	odd  = ODDPARITY,
	off  = NOPARITY,
	mark = MARKPARITY	
};


void ErrorExit(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    sprintf((LPTSTR)lpDisplayBuf, 
        TEXT("%s failed with error %d:\n%s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}



/**
	\brief Opens a new connection to a serial port
	\param portname		name of the serial port(COM1 - COM9 or \\\\.\\COM1-COM256)
	\param baudrate		the baudrate of this port (for example 9600)
	\param stopbits		th nuber of stoppbits (one, onePointFive or two)
	\param parity		the parity (even, odd, off or mark)
	\return			HANDLE to the serial port
	*/
HANDLE openSerialPort(LPCSTR portname,enum Baudrate baudrate, enum Stopbits stopbits, enum Paritycheck parity)
{
	DWORD  accessdirection =GENERIC_READ | GENERIC_WRITE;
	HANDLE hSerial = CreateFile(portname,
		accessdirection,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);
	if (hSerial == INVALID_HANDLE_VALUE) {
		ErrorExit("CreateFile");
	}
	DCB dcbSerialParams = {0};
	dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
		 ErrorExit("GetCommState");
	}
	dcbSerialParams.BaudRate=baudrate;
	dcbSerialParams.ByteSize=8;
	dcbSerialParams.StopBits=stopbits;
	dcbSerialParams.Parity=parity;
	if(!SetCommState(hSerial, &dcbSerialParams)){
		 ErrorExit("SetCommState");
	}
	COMMTIMEOUTS timeouts={0};
	timeouts.ReadIntervalTimeout=50;
	timeouts.ReadTotalTimeoutConstant=50;
	timeouts.ReadTotalTimeoutMultiplier=10;
	timeouts.WriteTotalTimeoutConstant=50;
	timeouts.WriteTotalTimeoutMultiplier=10;
	if(!SetCommTimeouts(hSerial, &timeouts)){
		ErrorExit("SetCommTimeouts");
	}
	return hSerial;
}

/**
	\brief Read data from the serial port
	\param hSerial		File HANDLE to the serial port
	\param buffer		pointer to the area where the read data will be written
	\param buffersize	maximal size of the buffer area
	\return				amount of data that was read
	*/
DWORD readFromSerialPort(HANDLE hSerial, char * buffer, int buffersize)
{
    DWORD dwBytesRead = 0;
    if(!ReadFile(hSerial, buffer, buffersize, &dwBytesRead, NULL)){
        ErrorExit("ReadFile");
    }
    return dwBytesRead;
}

/**
	\brief write data to the serial port
	\param hSerial	File HANDLE to the serial port
	\param buffer	pointer to the area where the read data will be read
	\param length	amount of data to be read
	\return			amount of data that was written
	*/
DWORD writeToSerialPort(HANDLE hSerial, char * data, int length)
{
	DWORD dwBytesRead = 0;
	if(!WriteFile(hSerial, data, length, &dwBytesRead, NULL)){
		ErrorExit("WriteFile");
	}
	return dwBytesRead;
}

void closeSerialPort(HANDLE hSerial)
{
	CloseHandle(hSerial);
}
#endif

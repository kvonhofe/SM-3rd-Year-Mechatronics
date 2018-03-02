#ifndef SERIALCOMMS_H
#define SERIALCOMMS_H

#include <windows.h>
#include <tchar.h>


//Code framework by A/Prof. J.Katupitiya(c)2013


HANDLE OpenSerialPort(TCHAR* commPort, int baudRate, int bits, int parity, int stopbits); 
#endif

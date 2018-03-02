#include <windows.h>
#include <tchar.h>


//Code framework by A/Prof. J.Katupitiya(c)2013


HANDLE OpenSerialPort(TCHAR* commPort, int baudRate, int bits, int parity, int stopbits) 
{
	DCB dcb;
	int OK;
	//  Open a handle for the specified com port.
	HANDLE hCom = CreateFile( commPort,
		GENERIC_READ | GENERIC_WRITE,
		0,				//  a must 
		NULL,			//  default security attributes
		OPEN_EXISTING,  //  a must
		0,				//  non-overlapped I/O
		NULL);			//  a must

	if (hCom == INVALID_HANDLE_VALUE) 
	{
		return hCom;
	}


	//  Device Control Block init.
	SecureZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	//  Fill in DCB values 

	dcb.BaudRate = baudRate;//CBR_115200;
	dcb.ByteSize = bits; //8;
	dcb.fBinary  = 1;// versus ASCII (no EOT detection)
	dcb.Parity   = parity;//NOPARITY;
	dcb.StopBits = stopbits;//ONESTOPBIT;

	OK = SetCommState(hCom, &dcb);

	if (!OK) 
	{
		CloseHandle(hCom);
		return NULL;
	}
	return hCom;
}

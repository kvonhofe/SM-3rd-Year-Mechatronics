#define _USE_MATH_DEFINES
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <SMObject.h>
#include <SMStructs.h>
#include <GPSClient.h>
#include <EthernetClient.h>
#include <SMFcn.h>


#define GPS_PORT "24000"
#define IP_ADDRESS "192.168.1.200"
#define DATA_BLOCK_SIZE 112 
#define GPS_HEADER 0xAA44121C

#pragma pack(1)

using namespace std;

int main() {
	unsigned char *BytePtr;
	unsigned char *StructPtr;
	unsigned char Byte;	
	unsigned long CRC;
	unsigned int Header;

	double Northing = -1;
	double Easting = -1;
	double Height = -1;

	//SM references
	SMObject PMObj(TEXT("Process Management"), sizeof(ProcessManagement)); 
	SMObject GPSObj(TEXT("GPS"), sizeof(GPS));

	PMObj.SMAccess();
	GPSObj.SMAccess();

	ProcessManagement * PM = (ProcessManagement*)PMObj.pData;
	GPS * pGPS = (GPS*)GPSObj.pData;


	GPSClient GPSConnect(IP_ADDRESS, GPS_PORT);
	
	if (GPSConnect.ConnectToServer() == 0) {
		cout << "Connected to GPS" << endl;
		char zID[10] = "5061866\n";
		GPSConnect.SendUser("5061866\n", strlen(zID));
	}
	else {
		cout << "Connection to GPS Failed" << endl;
	}

	//RxData now contains the entire memory block
	while(!PM->Shutdown.Flags.GPS) {

		//set heartbeat
		PM->HeartBeat.Flags.GPS = 1;
		Sleep(50);

		GPSConnect.ReceiveData();
		
		//GPSConnect.Data.MsgHeader = 0x00;
		Header = 0x00;
		
		//detect the header
		int i = 0;
		while (Header != GPS_HEADER) {
			Byte = GPSConnect.RxData[i];
			Header <<= 8;
			Header |= Byte;
			i++;
		}
		

		GPSConnect.Data.MsgHeader = Header;
		BytePtr = (unsigned char*)&GPSConnect.RxData[i];		    //points to discard set 1 in RxData
		StructPtr = (unsigned char*)&GPSConnect.Data.DiscardSet1;   //points to discard set 1 in my struct
		
		//copy all of the data over to my struct (destination, source, size)
		memcpy(StructPtr, BytePtr, DATA_BLOCK_SIZE-4);

		pGPS->Northing = GPSConnect.Data.Northing;
		pGPS->Easting = GPSConnect.Data.Easting;
		pGPS->Height = GPSConnect.Data.Height;
		cout << "Northing:    " << pGPS->Northing << "    Easting:    " << pGPS->Easting << "    Height    " << pGPS->Height << endl;

		//check data integrity & calculate the CRC locally
		CRC = GPSConnect.CalculateBlockCRC32(DATA_BLOCK_SIZE-4, (unsigned char*)&GPSConnect.RxData);

		//tell other modules if the GPS data is correct
		if (CRC == GPSConnect.Data.CRC) {
			cout << "Data Valid" << endl;
			pGPS->CorrectCRC = true;
		}
		else {
			cout << "Data Invalid" << endl;
			pGPS->CorrectCRC = false;
		}
		cout << "\n" << endl;
		
		if (_kbhit()) break;
	}
	return 0;
}




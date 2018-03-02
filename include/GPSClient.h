#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <EthernetClient.h>

#pragma pack(1)

struct GPSData {
	unsigned int MsgHeader;  //4 bytes 
	unsigned char DiscardSet1[40]; //40 bytes
	double Northing;  //8 bytes 
	double Easting;   //8 bytes 
	double Height;    //8 bytes 
	unsigned char DiscardSet2[40]; //40 bytes 
	unsigned long CRC;  //4 bytes 
};
/*
struct GPSData {
	unsigned int MsgHeader;  //4 bytes 
	unsigned char rejects1[9]; //9 bytes 
	unsigned char GPSQuality;  //1 byte
	unsigned short week;  //4 bytes 
	unsigned long timeStamp; //4 bytes
	unsigned char rejects2[24];  //24 bytes 
	double Northing;  //8 bytes 
	double Easting;  //8 bytes
	double Height;   //8 bytes 
	unsigned char rejects3[40];  //40 bytes 
	unsigned long CRC;  //4 bytes 
};*/



class GPSClient : public EthernetClient {
public:
	GPSData Data;
public:
	GPSClient();
	GPSClient(char * portaddress, char * port);
	~GPSClient() {}
    unsigned long CRC32Value(int i);
    unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char *ucBuffer);
};
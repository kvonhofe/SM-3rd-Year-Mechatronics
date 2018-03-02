#define _USE_MATH_DEFINES
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>
#include <GPSClient.h>

#define CRC32_POLYNOMIAL        0xEDB88320L
#define SIZE                    100000
#define GPS_OFFSET              0.230
#define TWIN_GPS_SEPARATION     1.45

using namespace std;

#pragma pack(1)

GPSClient::GPSClient() : EthernetClient() 
{
};

GPSClient::GPSClient(char * portaddress, char * port) : EthernetClient(portaddress, port) 
{
};


unsigned long GPSClient::CRC32Value(int i){
    int j;
    unsigned long ulCRC;
    ulCRC = i;
    for (j = 8; j > 0; j--)
    {
        if (ulCRC & 1)
            ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
        else
            ulCRC >>= 1;
    }
    return ulCRC; 
}

//calculates CRC-32 of an entire block of data - pass in number of bytes in block & the data block itself
//Note: Data Block ucBuffer should contain all data bytes of the full data record except the checksum bytes. (last 4)
unsigned long GPSClient::CalculateBlockCRC32(unsigned long ulCount, unsigned char *ucBuffer) 
{
    unsigned long ulTemp1;
    unsigned long ulTemp2;
    unsigned long ulCRC = 0;
    while (ulCount-- != 0)
    {
        ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
        ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xff); //^ is bitwise exclusive or
        ulCRC = ulTemp1 ^ ulTemp2;
    }
    return(ulCRC);
}

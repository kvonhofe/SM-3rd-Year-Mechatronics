#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef ETHERNETCLIENT_H
#define ETHERNETCLIENT_H

#define WSASTARTUP_FAILED	1
#define ADDRESSINFO_FAILED	2
#define CONNECTION_CLOSED   3
#define RECEIVE_FAILED      4

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>


class EthernetClient
{
public: 
	char RxData[4096];

protected:
	char* IpAddress;
	char* Port;
	int Error;
private:
	WSADATA wsaData;
	int iResult;
	struct addrinfo *result,*ptr, hints;
	//struct addrinfo *result = NULL,*ptr = NULL,hints;
	SOCKET ConnectSocket;
	//SOCKET ConnectSocket = INVALID_SOCKET;
public:
	EthernetClient();
	EthernetClient(char* ipaddress, char* port);
	virtual ~EthernetClient();
	int GetError();
	int SendData(char* data, int dataLength);
	int SendUser(char* data, int dataLength);
	void ReceiveData();
	int ConnectToServer();
	void CloseConnection();
	void Cleanup();
	int getiResult();
};

#endif
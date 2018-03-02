#define _USE_MATH_DEFINES
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "EthernetClient.h"

EthernetClient::EthernetClient()
{
	Error = 0;
	ConnectSocket = INVALID_SOCKET;
	result = NULL;
	ptr = NULL;
}

EthernetClient::EthernetClient(char* ipaddress, char* port)
{
	IpAddress = ipaddress;
	Port = port;
	Error = 0;
}

EthernetClient::~EthernetClient()
{
	if(ConnectSocket != INVALID_SOCKET)
			closesocket(ConnectSocket);
	WSACleanup();
}

int EthernetClient::GetError()
{
	return Error;
}

int EthernetClient::SendData(char* data, int dataLength)
{
	char STX[1] = {0x02};
	char ETX[1] = {0x03};

	Error = 0;

	iResult = send(ConnectSocket, STX, 1, 0);
	if (iResult == SOCKET_ERROR) 
	{
		Error = SOCKET_ERROR;
		closesocket(ConnectSocket);
		WSACleanup();
		return SOCKET_ERROR;
	}
	iResult = send(ConnectSocket, data, (int)strlen(data), 0);
	if (iResult == SOCKET_ERROR) 
	{
		Error = SOCKET_ERROR;
		closesocket(ConnectSocket);
		WSACleanup();
		return SOCKET_ERROR;
	}
	//else
	//{
	//	printf("<< ");
	//	for(int ii = 0; ii < iResult; ii++)
	//		printf("%c",cmd[ii]);
	//	printf("\n");
	//}

	iResult = send(ConnectSocket, ETX, 1, 0);
	if (iResult == SOCKET_ERROR) 
	{
		Error = SOCKET_ERROR;
		closesocket(ConnectSocket);
		WSACleanup();
		return SOCKET_ERROR;
	}
	return 0;
}


int EthernetClient::SendUser(char* data, int dataLength)
{
	Error = 0;

	iResult = send(ConnectSocket, data, (int)strlen(data), 0);
	if (iResult == SOCKET_ERROR)
	{
		Error = SOCKET_ERROR;
		closesocket(ConnectSocket);
		WSACleanup();
		return SOCKET_ERROR;
	}
	
	ReceiveData();

	return 0;
}


void EthernetClient::ReceiveData()
{
	Error = 0;
	iResult = recv(ConnectSocket, RxData, (int)sizeof(RxData), 0);
	if (iResult > 0)
	{
		//printf(">> ");
		//for(int ii = 0; ii < iResult; ii++)
		//	printf("%c",Data[ii]);
		//printf("\n");
	}
	else if (iResult == 0)
		Error = CONNECTION_CLOSED;
	else
		Error = RECEIVE_FAILED;	
}

int EthernetClient::ConnectToServer()
{
	Error = 0;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) 
	{
		Error = WSASTARTUP_FAILED;
		return WSASTARTUP_FAILED;
	}

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(IpAddress, Port, &hints, &result);
	if (iResult != 0) 
	{
		Error = ADDRESSINFO_FAILED;
		WSACleanup();
		return ADDRESSINFO_FAILED;
	}

	ptr=result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) 
	{
		Error = INVALID_SOCKET;
		freeaddrinfo(result);
		WSACleanup();
		return INVALID_SOCKET;
	}
	// Connect to server.
	iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) 
	{
		Error = INVALID_SOCKET;
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		return INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) 
	{
		Error = INVALID_SOCKET;
		WSACleanup();
		return INVALID_SOCKET;
	}
	return 0;
}

void EthernetClient::CloseConnection()
{
	closesocket(ConnectSocket);
}

void EthernetClient::Cleanup()
{
	WSACleanup();
}

int EthernetClient::getiResult(){

	return iResult;
}
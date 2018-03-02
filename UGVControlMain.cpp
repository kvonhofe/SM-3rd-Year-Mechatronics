#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <SMObject.h>
#include <SMStructs.h>
#include <iostream>
#include <conio.h>
#include <UGVControl.h>

#define IP_ADDRESS "192.168.1.200"
#define UGV_PORT "25000"


using namespace std;
 
int main() {
	
	
	UGVControl UGV(IP_ADDRESS, UGV_PORT);
	if (UGV.ConnectToServer() == 0) cout << "Connected to UGV" << endl; //this line sometimes causes the module to start 30 seconds late
	else {
		cout << "Connection to UGV failed" << endl;
	}

	char zID[10] = "5061866\n";
	UGV.SendUser("5061866\n",strlen(zID));
	//UGV.ReceiveData();


	SMObject PMObj(TEXT("Process Management"), sizeof(ProcessManagement)); //size of the struct 
	SMObject XBoxObj(TEXT("XBox"), sizeof(Remote));

	PMObj.SMAccess();
	XBoxObj.SMAccess();

	ProcessManagement * PM = (ProcessManagement*)PMObj.pData;
	Remote * pXBox = (Remote*)XBoxObj.pData;

	PM->Shutdown.Flags.UGV = 0;
	
	while (!PM->Shutdown.Flags.UGV) {
	
		//set heartbeat
		PM->HeartBeat.Flags.UGV = 1;
		//Sleep(50);
		Sleep(20);

		UGV.Drive(pXBox->SetSpeed, pXBox->SetSteering);

		if (_kbhit()) break;
	}
	
	return 0;
}

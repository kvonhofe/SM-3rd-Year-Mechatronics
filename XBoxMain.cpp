#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <SMObject.h>
#include <SMStructs.h>
#include <iostream>
#include <conio.h>
#include <xinput.h>
#include <XBox.h>

#define STEERING_FACTOR 40
#define PM_HEARTBEAT_CHECK 7

using namespace std;

int main() {

	Controller GameController;
	//int PMCount = 0;

	cout << "XBox module running" << endl;

	SMObject PMObj(TEXT("Process Management"), sizeof(ProcessManagement)); //size of the struct 
	SMObject XBoxObj(TEXT("XBox"), sizeof(Remote));

	PMObj.SMAccess();
	XBoxObj.SMAccess();

	ProcessManagement * PM = (ProcessManagement*)PMObj.pData;
	Remote * pXBox = (Remote*)XBoxObj.pData;

	PM->Shutdown.Flags.Remote = 0;
	pXBox->Terminate = 0;

	bool connected = false;

	while (!PM->Shutdown.Flags.Remote) {

		// set xbox heartbeat
		PM->HeartBeat.Flags.Remote = 1;
		//Sleep(50);

		//an alternative method of monitering PM --> this would be implemented in every module 
		/*
		if (PM->HeartBeat.Flags.Remote == 0) {
			PM->HeartBeat.Flags.Remote = 1;
			PMCount = 0;
		}
		else { //PM has not set the heartbeats to zero
			PMCount++;
		}

		if (PMCount > PM_HEARTBEAT_CHECK) PM->Shutdown.Status = 0xFFFF;
		*/

		//Set speed and steering for the UGV Control
		if (!GameController.Refresh()) {
			pXBox->Connected = false;
			pXBox->SetSpeed = 0;
			pXBox->SetSteering = 0;
			cout << " Controller not connected" << endl;
		}
		else {
			if (!connected) {
				connected = true;
				pXBox->Connected = true;
				cout << "Controller connected" << endl;
			}
			pXBox->Connected = true;
			pXBox->SetSpeed = (double)GameController.leftJoyY;
			pXBox->SetSteering = (double)GameController.rightJoyX*STEERING_FACTOR;
			if (GameController.IsPressed(XINPUT_GAMEPAD_X)) pXBox->Terminate = 1;
		}

		if (_kbhit()) break;
	}
	return 0;
}
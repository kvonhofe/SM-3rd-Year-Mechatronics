#define _USE_MATH_DEFINES
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <stdlib.h>

#include <SMObject.h>
#include <SMStructs.h>
#include <EthernetClient.h>
#include <LMS151.h>
#include <SMFcn.h>

#define IP_ADDRESS "192.168.1.200"
#define LASER_PORT "23000"
#define PM_HEARTBEAT_CHECK 7
 
using namespace std;

int main() {

	LMS151 LMS151Laser(IP_ADDRESS, LASER_PORT); 

	SMObject PMObj(TEXT("Process Management"), sizeof(ProcessManagement)); //size of the struct 
	SMObject LaserObj(TEXT("Laser"), sizeof(Laser));

	PMObj.SMAccess();
	LaserObj.SMAccess();
	
	ProcessManagement * PM = (ProcessManagement*)PMObj.pData;
	Laser * pLaser = (Laser*)LaserObj.pData;

	
	//connect to laser
	if (LMS151Laser.ConnectToServer() == 0) {
		cout << "Connected to Laser" << endl;
		char zID[10] = "5061866\n";
		LMS151Laser.SendUser("5061866\n", strlen(zID));
	}
	else {
		cout << "Connection to Laser Failed" << endl;
	}

	//char zID[10] = "5061866\n";
	//LMS151Laser.SendUser("5061866\n", strlen(zID));
	
	PM->Shutdown.Flags.Laser = 0;

	cout << "Laser Module Running" << endl;

	//for checking the heartbeat of PM
	int PMCount = 0;

	//start measurement
	while (!PM->Shutdown.Flags.Laser) {

		//an alternative method of monitering PM --> this is implemented in every module 
		/*
		if (PM->HeartBeat.Flags.Laser == 0) {
			PM->HeartBeat.Flags.Laser = 1;
			PMCount = 0;
		}
		else {
			PMCount++;
		}

		if (PMCount > PM_HEARTBEAT_CHECK) PM->Shutdown.Status = 0xFFFF;
		*/

		PM->HeartBeat.Flags.Laser = 1;
		Sleep(100);

		//read laser 
		LMS151Laser.GetRangeCoords();

		//put data in shared memory
		for (int i = 0; i < LMS151Laser.NumPoints; i++)
		{
			pLaser->x[i] = LMS151Laser.RangeData[i][0];
			pLaser->y[i] = LMS151Laser.RangeData[i][1];
			pLaser->NumPoints = LMS151Laser.NumPoints;
			//cout << pLaser->x[i] << " " << pLaser->y[i] << endl;
		}

		
		// make sure that if you close PM then everything closes 
		if (!PM->HeartBeat.Flags.PM) {
			PMCount++;
			//cout << "a" << endl;
			if (PMCount > PM_HEARTBEAT_CHECK) {
				//shutdown
				PM->Shutdown.Status = 0xFFFF;
				PMCount = 0;
			}
		}
		PM->HeartBeat.Flags.PM = 0;
		Sleep(100);

		if (_kbhit()) break;
	}

	cout << "Termination laser module normal" << endl;
	return 0;
}
#define _USE_MATH_DEFINES
#include <Windows.h>
#include <SMObject.h>
#include <SMStructs.h>
#include <SMFcn.h>
#include <iostream>
#include <conio.h>
#include <math.h>

using namespace std;

int main() {

	//time stamp variables 
	_int64 Frequency, HPCount;
	double SimTimeStamp, LastSimTimeStamp;

	//simulator variables 
	double DeltaT;
	double Length = 0.7;  //needs checking 

	SMObject PMObj(TEXT("Process Management"), sizeof(ProcessManagement));
	SMObject SimObj(TEXT("Simulator"), sizeof(Simulator));
	SMObject XBoxObj(TEXT("XBox"), sizeof(Remote));

	PMObj.SMAccess();
	SimObj.SMAccess();
	XBoxObj.SMAccess();

	ProcessManagement * PM = (ProcessManagement*)PMObj.pData;
	Simulator * pSim = (Simulator*)SimObj.pData;
	Remote * pXBox = (Remote*)XBoxObj.pData;

	//set initial conditions
	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
	QueryPerformanceCounter((LARGE_INTEGER*)&HPCount);
	pSim->SimTimeStamp = (double)HPCount / (double)Frequency;
	pSim->X = 1.0;
	pSim->Y = 2.0;
	pSim->Heading = M_PI / 4.0;

	PM->Shutdown.Flags.Simulator = 0;
	cout << "Simulator Module Running" << endl;

	while (!PM->Shutdown.Flags.Simulator) {

		//set heartbeat
		PM->HeartBeat.Flags.Simulator = 1;
		Sleep(100);

		//check to see if the simulator gets the xbox data (this should update as you press buttons on the controller)
		//cout << pXBox->SetSpeed << "	" << pXBox->SetSteering << "	" << endl;

		cout << pSim->X << "	" << pSim->Y << "	" << pSim->Heading << "		" << endl;

		LastSimTimeStamp = pSim->SimTimeStamp;

		QueryPerformanceCounter((LARGE_INTEGER*)&HPCount);
		SimTimeStamp = (double)HPCount / (double)Frequency;
		pSim->SimTimeStamp = SimTimeStamp;

		DeltaT = SimTimeStamp - LastSimTimeStamp;
		pSim->X += pXBox->SetSpeed*cos(pSim->Heading)*DeltaT;
		pSim->Y += pXBox->SetSpeed*sin(pSim->Heading)*DeltaT;
		pSim->Heading += pXBox->SetSpeed*tan(pXBox->SetSteering*M_PI/180.0)*DeltaT / Length;

		if (_kbhit()) break;
	}

	cout << "Terminating nomally" << endl;
	return 0;
}
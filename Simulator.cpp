#define _USE_MATH_DEFINES
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <tchar.h>
#include <Xbox.h>
#include <math.h>
#include <SMObject.h>
#include <SMStruct.h>
#include <Vehicle.h>
#include <carlike.h>
#include <conio.h>

using namespace std;

int main()
{
	Vehicle* VPtr = NULL;
	VPtr = new CarLike(Pose(0.0, 0.0, M_PI / 4.0), 0.750);
	if (VPtr == NULL)
		return -2;
	
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	SMObject SimObj(TEXT("SimulatorData"), sizeof(Simulator));
	SMObject RemoteObj(TEXT("RemoteControl"), sizeof(Remote));

	PMObj.SMAccess();
	SimObj.SMAccess();
	RemoteObj.SMAccess();


	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);// SW_SHOWMINIMIZED);//SW_SHOW, SW_HIDE

	ProcessManagement* PM = (ProcessManagement*)PMObj.pData;
	Remote* RM = (Remote*)RemoteObj.pData;
	Simulator* SimP = (Simulator*)SimObj.pData;
	// Any other initialization


	PM->ShutDown.Flags.Simulator = 0;

	// DO as long as not Simulator.shutdown
	while (!PM->ShutDown.Flags.Simulator)
	{
		VPtr->SetVehicleSpeed(RM->SetSpeed);
		VPtr->SetVehicleSteering(RM->SetSteering);
		VPtr->UpdatePose();
		SimP->X = VPtr->GetPose().x;
		SimP->Y = VPtr->GetPose().y;
		SimP->Heading = VPtr->GetPose().Heading;
		PM->HeartBeat.Flags.Simulator = 1;
		if (_kbhit())
			break;
		Sleep(25);
	}
	// Any termination
	return 0;
	// End
}
/* Using MATLAB inside a C/C++ programs */
/*
*	MATLABInC.cpp
*
*	J.Katupitiya@unsw.edu.au
*	(c)2013 UNSW.
*
*
*/
#define _USE_MATH_DEFINES
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <SMObject.h>
#include <SMStruct.h>


#include "engine.h"
/**** Specify include files from Matlab/extern and also lib files
libmx.lib, libmex.lib and libeng.lib from Matlab/extern/lib/microsoft
*******/
//h = findall(gca, 'type', 'line', 'color', 'b')
//h = findall(gca, 'type', 'line', 'marker', '+') ???
//delete(h)
using namespace std;

int main()
{
	SMObject RemoteObj(TEXT("RemoteControl"), sizeof(Remote));
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	SMObject SimObj(TEXT("SimulatorData"), sizeof(Simulator));
	// Request access to GPS mem block
	RemoteObj.SMAccess();
	PMObj.SMAccess();
	SimObj.SMAccess();

	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);// SW_SHOWMINIMIZED);//SW_SHOW, SW_HIDE

	ProcessManagement* PM = (ProcessManagement*)PMObj.pData;
	Remote* RM = (Remote*)RemoteObj.pData;
	Simulator* SimP = (Simulator*)SimObj.pData;

	char Data[4096] = " ";
	// Declarations for MATLAB
	Engine *ep;
	mxArray *X = NULL, *Y = NULL, *PSI = NULL;
	mxArray *BODY = NULL, *FR = NULL, *FL = NULL, *RR = NULL, *RL = NULL;
	double Body[5][2] = { { -0.1,0.25 },{ 0.65,0.25 },{ 0.65,-0.25 },{ -0.1,-0.25 },{ -0.1,0.25 } };
	double BodyTFR[5][2];
	double Wheel[5][2] = { { -0.2,0.05 },{ 0.2,0.05 },{ 0.2,-0.05 },{ -0.2,-0.05 },{ -0.2,0.05 } };
	double WheelTFR[5][2];
	double FR0[5][2], FL0[5][2];
	double x, y, psi, delta;


	//Start local MATLAB engine
	if (!(ep = engOpen("\0")))
	{
		printf("\nMATLAB engine failure\n");
		return 1;
	}

	// Make engine window visible or not visible (0 not visible)
	engSetVisible(ep, 0);
	engEvalString(ep, "axis([-5 5 -5 5]);hold on;");
	X = mxCreateDoubleMatrix(1, 1, mxREAL);
	Y = mxCreateDoubleMatrix(1, 1, mxREAL);
	BODY = mxCreateDoubleMatrix(2, 5, mxREAL);
	FR = mxCreateDoubleMatrix(2, 5, mxREAL);
	FL = mxCreateDoubleMatrix(2, 5, mxREAL);
	RR = mxCreateDoubleMatrix(2, 5, mxREAL);
	RL = mxCreateDoubleMatrix(2, 5, mxREAL);
	PM->ShutDown.Flags.Plot = 0;

	while (!PM->ShutDown.Flags.Plot)
	{
		// c Program operations
		x = SimP->X;
		y = SimP->Y;
		psi = SimP->Heading;
		delta = RM->SetSteering*M_PI / 180.0;

		//MATLAB operations 
		memcpy((void *)mxGetPr(X), (void *)&x, sizeof(x));
		memcpy((void *)mxGetPr(Y), (void *)&y, sizeof(y));
		// Body
		for (int i = 0; i < 5; i++)
		{
			BodyTFR[i][0] = Body[i][0] * cos(SimP->Heading) - Body[i][1] * sin(SimP->Heading) + SimP->X;
			BodyTFR[i][1] = Body[i][0] * sin(SimP->Heading) + Body[i][1] * cos(SimP->Heading) + SimP->Y;
		}
		memcpy((void *)mxGetPr(BODY), (void *)BodyTFR, sizeof(BodyTFR));
		for (int i = 0; i < 5; i++)
		{
			FR0[i][0] = Wheel[i][0] * cos(delta) - Wheel[i][1] * sin(delta) + 0.55;
			FR0[i][1] = Wheel[i][0] * sin(delta) + Wheel[i][1] * cos(delta) + -0.32;
			FL0[i][0] = Wheel[i][0] * cos(delta) - Wheel[i][1] * sin(delta) + 0.55;
			FL0[i][1] = Wheel[i][0] * sin(delta) + Wheel[i][1] * cos(delta) + 0.32;
		}
		// FR
		for (int i = 0; i < 5; i++)
		{
			WheelTFR[i][0] = FR0[i][0] * cos(SimP->Heading) - FR0[i][1] * sin(SimP->Heading) + SimP->X;
			WheelTFR[i][1] = FR0[i][0] * sin(SimP->Heading) + FR0[i][1] * cos(SimP->Heading) + SimP->Y;

		}
		memcpy((void *)mxGetPr(FR), (void *)WheelTFR, sizeof(WheelTFR));
		//FL
		for (int i = 0; i < 5; i++)
		{
			WheelTFR[i][0] = FL0[i][0] * cos(SimP->Heading) - FL0[i][1] * sin(SimP->Heading) + SimP->X;
			WheelTFR[i][1] = FL0[i][0] * sin(SimP->Heading) + FL0[i][1] * cos(SimP->Heading) + SimP->Y;

		}
		memcpy((void *)mxGetPr(FL), (void *)WheelTFR, sizeof(WheelTFR));
		//RR
		for (int i = 0; i < 5; i++)
		{
			WheelTFR[i][0] = Wheel[i][0] * cos(SimP->Heading) - (Wheel[i][1] - 0.32) * sin(SimP->Heading) + SimP->X;
			WheelTFR[i][1] = Wheel[i][0] * sin(SimP->Heading) + (Wheel[i][1] - 0.32) * cos(SimP->Heading) + SimP->Y;

		}
		memcpy((void *)mxGetPr(RR), (void *)WheelTFR, sizeof(WheelTFR));
		//RL
		for (int i = 0; i < 5; i++)
		{
			WheelTFR[i][0] = Wheel[i][0] * cos(SimP->Heading) - (Wheel[i][1] + 0.32) * sin(SimP->Heading) + SimP->X;
			WheelTFR[i][1] = Wheel[i][0] * sin(SimP->Heading) + (Wheel[i][1] + 0.32) * cos(SimP->Heading) + SimP->Y;

		}
		memcpy((void *)mxGetPr(RL), (void *)WheelTFR, sizeof(WheelTFR));

		engPutVariable(ep, "BODY", BODY);
		engPutVariable(ep, "X", X);
		engPutVariable(ep, "Y", Y);
		engPutVariable(ep, "FL", FL);
		engPutVariable(ep, "FR", FR);
		engPutVariable(ep, "RL", RL);
		engPutVariable(ep, "RR", RR);
		engEvalString(ep, "BODY = BODY';");
		engEvalString(ep, "FL = FL';");
		engEvalString(ep, "FR = FR';");
		engEvalString(ep, "RL = RL';");
		engEvalString(ep, "RR = RR';");
		//_getch();
		engEvalString(ep, "h = findall(gca, 'type', 'line', 'color', 'black')");// ???
		engEvalString(ep, "delete(h)");
		engEvalString(ep, "plot(BODY(:,1),BODY(:,2),'k');");
		engEvalString(ep, "plot(FL(:,1),FL(:,2),'k');");
		engEvalString(ep, "plot(FR(:,1),FR(:,2),'k');");
		engEvalString(ep, "plot(RL(:,1),RL(:,2),'k');");
		engEvalString(ep, "plot(RR(:,1),RR(:,2),'k');");
		engEvalString(ep, "plot(X,Y,'+r');");

		PM->HeartBeat.Flags.Plot = 1;
		if (_kbhit())
			break;
		Sleep(25);
	}

	/*
	* Free memory and close the MATLAB engine.
	*/
	printf("Closing!\n");
	mxDestroyArray(X);
	mxDestroyArray(Y);
	engClose(ep);

	return 0;
}
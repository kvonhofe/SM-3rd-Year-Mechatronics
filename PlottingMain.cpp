#define _USE_MATH_DEFINES
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <math.h>
#include <SMObject.h>
#include <SMStructs.h>
#include <engine.h>

using namespace std;

int main() {

	_int64 Frequency, HPCount;
	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
	double PlotTimeStamp;

	//local variables for x and y
	double x[181], y[181];

	//double UGVX = 1.0;
	//double UGVY = 2.0;
	//double UGVHeading = M_PI / 4.0;
	
	// Declarations for MATLAB
	Engine *ep;
	mxArray *X = NULL, *Y = NULL;
	
	//mxArray *XUGV = NULL;
	//mxArray *YUGV = NULL;
	//mxArray *HeadingUGV = NULL;
	

	SMObject PMObj(TEXT("Process Management"), sizeof(ProcessManagement)); //size of the struct 
	SMObject LaserObj(TEXT("Laser"), sizeof(Laser));
	SMObject SimObj(TEXT("Simulator"), sizeof(Simulator));

	//get access to shared memory
	PMObj.SMAccess();
	LaserObj.SMAccess();
	SimObj.SMAccess();

	// create pointers to the shared memory block
	ProcessManagement * PM = (ProcessManagement*)PMObj.pData;
	Laser *pLaser = (Laser*)LaserObj.pData;
	Simulator * pSim = (Simulator*)SimObj.pData;

	
	//Start local MATLAB engine
	if (!(ep = engOpen("\0")))
	{
		printf("\nMATLAB engine failure\n");
		return 1;
	}
	
	engEvalString(ep, "figure(1);");
	engEvalString(ep, "grid on;");

	engEvalString(ep, "axis([-1.0 5.0 -5.0 5.0]); hold on;");

	cout << "Plotting in matlab" << endl;

	while (!PM->Shutdown.Flags.Plot) {

		//set heartbeat
		PM->HeartBeat.Flags.Plot = 1;

		//get HPC count
		QueryPerformanceCounter((LARGE_INTEGER*)&HPCount);

		//calculate time stamp 
		PlotTimeStamp = (double)HPCount / (double)Frequency;

		//UGVX = pSim->X;
		//UGVY = pSim->Y;
		//UGVHeading = pSim->Heading;

		// the coordinate transformation is carried out from laser frame to the vehicle frame
		for (int i = 0; i < pLaser->NumPoints; i++) {
			x[i] = pLaser->x[i];
			y[i] = pLaser->y[i];

			//UGV simulation stuff
			/*
			double xLocal, yLocal;
			xLocal = pLaser->y[i];
			yLocal = pLaser->x[i];
			x[i] = xLocal*cos(UGVHeading) + yLocal*sin(UGVHeading);
			y[i] = -xLocal*sin(UGVHeading) + yLocal*cos(UGVHeading);
			*/
		}

		
		//matlab tasks 
		X = mxCreateDoubleMatrix(181, 1, mxREAL);
		Y = mxCreateDoubleMatrix(181, 1, mxREAL);

		//XUGV = mxCreateDoubleMatrix(1, 1, mxREAL);
		//YUGV = mxCreateDoubleMatrix(1, 1, mxREAL);

		//fill in matlab space
		memcpy((void *)mxGetPr(X), (void *)&x, sizeof(x));
		memcpy((void *)mxGetPr(Y), (void *)&y, sizeof(y));

		//memcpy((void *)mxGetPr(XUGV), (void *)&UGVX, sizeof(UGVX));
		//memcpy((void *)mxGetPr(YUGV), (void *)&UGVY, sizeof(UGVY));

		//transfer data to matlab
		engPutVariable(ep, "X", X);
		engPutVariable(ep, "Y", Y);

		//engPutVariable(ep, "XUGV", XUGV);
		//engPutVariable(ep, "YUGV", YUGV);

		//plot the data
		engEvalString(ep,"h = findall(gca, 'type', 'line', 'color', 'r');");
		engEvalString(ep,"delete(h);");
		engEvalString(ep, "plot(X,Y,'r');");
		//engEvalString(ep, "plot(XUGV,YUGV,'b+'");
		

		if (_kbhit()) break;
	}

	
	//Free memory and close the MATLAB engine.
	cout << "Closing Matlab" << endl;
	mxDestroyArray(X);
	mxDestroyArray(Y);
	engClose(ep);
	
	return 0;
}
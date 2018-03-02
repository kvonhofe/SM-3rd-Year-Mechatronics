#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <string>
#include <stdio.h>
#include <time.h>
#include <tchar.h>
#include <dos.h>
#include <cstdio>
#include <TlHelp32.h>
#include <process.h>
#include <winbase.h>

#include <SMObject.h>
#include <SMStructs.h>

using namespace std;

// Unused:Simulator    Plot:GPS:Camera:UGV    Remote:OpenGlView:Laser:PM
// xxxx xxxx x111 1111  Operating
// xxxx xxxx x011 1011 = 0x003B Critical
// xxxx xxxx x100 0100 = 0x0044 Non Critical
 
#define CRITICAL_MASK 0x003B   
#define NONCRITICAL_MASK 0x0044 
#define NUM_UNITS 6
#define NUM_MODULES 7

//start up sequence
TCHAR* Units[10] =
{
	TEXT("Laser.exe"),
	TEXT("OpenGL.exe"),
	TEXT("XBox.exe"),
	TEXT("UGVControl.exe"),
	TEXT("Camera.exe"),
	TEXT("GPS.exe"),
	//TEXT("Plotting.exe"),
	//TEXT("Simulator.exe"),
};

//function prototypes 
bool IsProcessRunning(const char *processName);
int RestartUnit(int i, STARTUPINFO *s, PROCESS_INFORMATION *p);
void killProcess(const char *filename);


int main() {
	
	_int64 Frequency, HPCount;
	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
	int CriticalMaskCount = 0;
	int NonCriticalMaskCount = 0;
	int Compare = 0; // comparing masks for the non critical module restart

	//Module execution based variable declerations
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];
	
	//give the objects a tag name --> think of this as a file name
	SMObject PMObj(TEXT("Process Management"), sizeof(ProcessManagement)); //size of the struct 
	SMObject GPSObj(TEXT("GPS"), sizeof(GPS));
	SMObject LaserObj(TEXT("Laser"), sizeof(Laser));
	//SMObject PlotObj(TEXT("Plotting"), sizeof(Plotting));
	//SMObject SimObj(TEXT("Simulator"), sizeof(Simulator));
	SMObject UGVObj(TEXT("UGV"), sizeof(UGV));
	SMObject XBoxObj(TEXT("XBox"), sizeof(Remote));
	SMObject CamObj(TEXT("Camera"), sizeof(Cam));

	//create the shared memory for all modules to attempt to access
	PMObj.SMCreate();
	GPSObj.SMCreate();
	LaserObj.SMCreate();
	//PlotObj.SMCreate();
	//SimObj.SMCreate();
	UGVObj.SMCreate();
	XBoxObj.SMCreate();
	CamObj.SMCreate();

	//get access to shared memory
	PMObj.SMAccess(); 
	XBoxObj.SMAccess();
	
	//start other modules in specified order
	for (int i = 0; i < NUM_UNITS; i++) {
		// Check if each process is running
		if (!IsProcessRunning(Units[i])) {
			ZeroMemory(&s[i], sizeof(s[i])); //ZeroMemory fills a block of memory with zeros
			s[i].cb = sizeof(s[i]);          //above line is to allocate memory for the executables in UNITS
			ZeroMemory(&p[i], sizeof(p[i]));
			// Start the child processes.

			if (!CreateProcess(NULL,   // No module name (use command line)
				Units[i],        // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				CREATE_NEW_CONSOLE,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory
				&s[i],            // Pointer to STARTUPINFO structure
				&p[i])           // Pointer to PROCESS_INFORMATION structure
				)
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				//_getch();
				return -1;
			}
		}
		cout << "Started: " << Units[i] << endl;
		Sleep(300);		
	}

	// create pointers to the shared memory block
	ProcessManagement * PM = (ProcessManagement*)PMObj.pData; 
	Remote * pXBox = (Remote*)XBoxObj.pData;

	PM->Shutdown.Status = 0x00; //run all modules

	while (!PM->Shutdown.Flags.PM) {
		
		//set heartbeat
		PM->HeartBeat.Flags.PM = 1;
		
		//get HPC count
		QueryPerformanceCounter((LARGE_INTEGER*)&HPCount);
		
		//calculate time stamp 
		PM->PMTimeStamp = (double)HPCount / (double)Frequency;
		
		//update screen
		system("CLS");   
		cout << "PM Time stamp:	" << PM->PMTimeStamp << endl;
		cout << "\n" << endl;
		cout << "Heartbeats" << endl;
		cout << "\n" << endl;
		cout << "	PM: " << PM->HeartBeat.Flags.PM << endl;
		cout << "	Laser: " << PM->HeartBeat.Flags.Laser << endl;
		cout << "	Remote: " << PM->HeartBeat.Flags.Remote << endl;
		cout << "	UGV: " << PM->HeartBeat.Flags.UGV << endl;
		cout << "	GPS: " << PM->HeartBeat.Flags.GPS << endl;
		cout << "	OpenGl: " << PM->HeartBeat.Flags.OpenGlView << endl;
		cout << "	Camera: " << PM->HeartBeat.Flags.Camera << endl;
		cout << "\n" << endl;
		cout << "Critical Mask Count:	" << CriticalMaskCount << endl;
		cout << "Noncritical Mask Count:	" << NonCriticalMaskCount << endl;
		

		Sleep(40);
		//check non critical modules
		if ((PM->HeartBeat.Status & NONCRITICAL_MASK) != NONCRITICAL_MASK) {
			NonCriticalMaskCount++;
			if (NonCriticalMaskCount > 100) {
				//initiate restart
				Compare = PM->HeartBeat.Status & NONCRITICAL_MASK;
				for (int i = 0; i < NUM_MODULES; i++) {
					if (((Compare >> i)&1) != ((NONCRITICAL_MASK >> i)&1)) { //&1 is there so that we can compare single bits 
						RestartUnit(i-1, s, p);
					}
				}
				NonCriticalMaskCount = 0;
			} 
		}
		else NonCriticalMaskCount = 0;
		

		//check critical modules 
		if ((PM->HeartBeat.Status & CRITICAL_MASK) != CRITICAL_MASK) {
			CriticalMaskCount++;
			if (CriticalMaskCount > 100) {
				//shutdown
				Compare = PM->HeartBeat.Status & CRITICAL_MASK;
				for (int i = 0; i < NUM_MODULES; i++) {
					if (((Compare >> i) & 1) != ((CRITICAL_MASK >> i) & 1)) {
						if (IsProcessRunning(Units[i-1])) killProcess(Units[i-1]); //kill any hanging processes
					}
				}
				PM->Shutdown.Status = 0xFFFF;
				CriticalMaskCount = 0;
			}
		}
		else CriticalMaskCount = 0;

		if (pXBox->Terminate) break;
		if (_kbhit()) break;

		//set all heartbeats to zero at the end of each loop --> acts as a watchdog timer
		PM->HeartBeat.Status = 0x00; 
	}

	PM->Shutdown.Status = 0xFFFF; //shut down all modules 
	cout << "Terminating normally" << endl; // all memory is released by destructors
}



bool IsProcessRunning(const char *processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL); //a handle is a pointer to an object located on the heap

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp(entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}




int RestartUnit(int i, STARTUPINFO *s, PROCESS_INFORMATION *p) {


	if(IsProcessRunning(Units[i])) killProcess(Units[i]);

	if (!IsProcessRunning(Units[i])) {

		ZeroMemory(&s[i], sizeof(s[i]));
		s[i].cb = sizeof(s[i]);
		ZeroMemory(&p[i], sizeof(p[i]));


		if (!CreateProcess(NULL,   // No module name (use command line)
			Units[i],        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			CREATE_NEW_CONSOLE,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory
			&s[i],            // Pointer to STARTUPINFO structure
			&p[i])           // Pointer to PROCESS_INFORMATION structure
			)
		{
			printf("%s failed (%d).\n", Units[i], GetLastError());
			return -1;
		}

		Sleep(100);
		return 0;
	}
}



void killProcess(const char *filename)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (strcmp(pEntry.szExeFile, filename) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
}




#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <UGVControl.h>
#include <stdio.h>

UGVControl::UGVControl()
{
	SetSpeed = 0.0;
	SetSteering = 0.0;
	Flag = 1;
}

UGVControl::UGVControl(char * portaddress, char * port):EthernetClient(portaddress, port)
{
	SetSpeed = 0.0;
	SetSteering = 0.0;
	Flag = 1;
}

void UGVControl::SetSetSpeed(double setSpeed) 
{
	SetSpeed = setSpeed;
}

void UGVControl::SetSetSteering(double setSteering) 
{
	SetSteering = setSteering;
}

double UGVControl::GetSpeed()
{
	return SetSpeed;
}

double UGVControl::GetSteering() 
{
	return SetSteering;
}

void UGVControl::Drive(double speed, double steering)
{
	char Buffer[128];
	SetSpeed = speed;
	SetSteering = -steering;
	Flag = 1 - Flag;
	//sprintf_s(Buffer, "# %10.3f %10.3f %1d #", SetSteering, SetSpeed, Flag);
	sprintf_s(Buffer, "# %0.3f %0.3f %1d #", SetSteering, SetSpeed, Flag);
	SendData(Buffer, strlen(Buffer));
}

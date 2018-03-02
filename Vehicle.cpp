#include <Vehicle.h>
#include <vector>

using namespace std;

void Vehicle::SetVehicleSpeed(double setSpeed)
{
	SetSpeed = setSpeed;
}

void Vehicle::SetVehicleSteering(double setSteering)
{
	SetSteering = setSteering;
}

void Vehicle::SetDesiredPath(vector<Pose> desiredPath)
{
	DesiredPath = desiredPath;
}

Pose Vehicle::GetPose()
{
	return P;
}

double Vehicle::GetSpeed()
{
	return SetSpeed;
}

double Vehicle::GetVehicleSteering()
{
	return SetSteering;
}

double Vehicle::GetVehicleSteeringEX()
{
	return SetSteeringEx;
}

vector<Pose> Vehicle::GetPath()
{
	return ActualPath;
}


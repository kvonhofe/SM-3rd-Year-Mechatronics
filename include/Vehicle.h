#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>

using namespace std;

struct Pose
{
	double x;
	double y;
	double Heading;
	Pose() {}
	Pose(double x, double y, double heading)
	{
		this->x = x;
		this->y = y;
		Heading = heading;
	}
};

class Vehicle
{
protected:
	Pose P;
	vector<Pose> DesiredPath;
	vector<Pose> ActualPath;
	double SetSpeed;
	double SetSteering;//2WS
	double SetSteeringEx;//4WS
public:
	Vehicle() {}
	virtual void SetVehicleSpeed(double setSpeed);
	virtual void SetVehicleSteering(double setSteering);
	virtual void SetDesiredPath(vector<Pose> desiredPath);
	virtual Pose GetPose();
	virtual double GetSpeed();
	virtual double GetVehicleSteering();
	virtual double GetVehicleSteeringEX();
	virtual vector<Pose> GetPath();
	virtual void UpdatePose() = 0;
	virtual ~Vehicle() {}
};
#endif

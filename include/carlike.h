#ifndef CARLIKE_H
#define CARLIKE_H

#include "Vehicle.h"

class CarLike : public Vehicle
{
	double Time;
	double Length;
	double Width;
	double MaxSteering;
	double MaxSpeed;
public:
	CarLike() {}
	CarLike(Pose p, double length);
	double UpdateTime();
	void UpdatePose();
	~CarLike() {}
};

#endif

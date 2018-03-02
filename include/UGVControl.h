#ifndef UGVCONTROL_H
#define UGVCONTROL_H

#include <EthernetClient.h>

class UGVControl : public EthernetClient
{
private:
	double SetSpeed;
	double SetSteering;
	int Flag;
	int EndAngle;
	double Resolution;
public:
	UGVControl();
	UGVControl(char * portaddress, char * port);
	~UGVControl() {}
	void SetSetSpeed(double setSpeed);
	void SetSetSteering(double setSteering);
	double GetSpeed();
	double GetSteering();
	void Drive(double speed, double steering);
};
#endif
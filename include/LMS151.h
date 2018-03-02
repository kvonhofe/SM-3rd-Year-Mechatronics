#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <EthernetClient.h>

#define LOGIN_FAILED -1
#define LOGOUT_FAILED -2
#define SCAN_CONFIG_ERROR -3

#define HALF_DEGREE 5000
#define QUART_DEGREE 2500

class LMS151 : public EthernetClient
{
public:
	double RangeData[10000][2];
	int NumPoints;
private:
	int StartAngle;
	int EndAngle;
	double Resolution;
public:
	LMS151();
	LMS151(char * portaddress, char * port);
	~LMS151();
	void Login();
	void Logout();
	void Configure(int startAngle, int endAngle, double resolution);
	void GetRangeCoords();
};

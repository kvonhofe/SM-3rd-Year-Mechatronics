#include <iostream>
#include <Windows.h>
#include <Xinput.h>

class Controller {
private:
	XINPUT_STATE state;
	int cId;
	float deadX;
	float deadY;
	bool deadZone;
public:
	Controller() : deadX(0.5f), deadY(0.5f), deadZone(true) {}
	Controller(float dzX, float dzY) : deadX(dzX), deadY(dzY) {}
	float leftJoyX;
	float leftJoyY;
	float rightJoyX;
	float rightJoyY;
	float leftTrigger;
	float rightTrigger;
	int  GetPort();
	bool CheckConnection();
	bool Refresh();
	bool IsPressed(WORD);
	void setcId(int a);
};


#include <XBox.h>

using namespace std;

int Controller::GetPort() {
	return cId + 1;
}


//Sets cId for each Controller connected
bool Controller::CheckConnection() {
	int ControllerId = -1;
	for (DWORD i = 0; i < XUSER_MAX_COUNT && ControllerId == -1; i++) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		if (XInputGetState(i, &state) == ERROR_SUCCESS) {
			ControllerId = i;
		}
	}
	cId = ControllerId;
	return ControllerId != -1;
}

// Returns false if the Controller has been disconnected
bool Controller::Refresh() {
	if (cId == -1) {
		CheckConnection();
	}
	else {
		if (XInputGetState(cId, &state) != ERROR_SUCCESS) {
			cId = -1;
			return false;
		}

		//Left joystick
		//Joystick values are signed 16 bit int's. Dividing by 32767 gives us a percentage or a value between -1 and 1.
		float normLX = fmaxf(-1, (float)state.Gamepad.sThumbLX / 32767);
		float normLY = fmaxf(-1, (float)state.Gamepad.sThumbLY / 32767);

		//Right joystick
		float normRX = fmaxf(-1, (float)state.Gamepad.sThumbRX / 32767);
		float normRY = fmaxf(-1, (float)state.Gamepad.sThumbRY / 32767);

		//if dead is true then we apply deadzone, otherwise we dont.
		if (deadZone) {
			//If the normalized values are less than the deadzone values then we set the
			//joystick values to 0. Otherwise, we scale them to the value proportional to the deadzone. 
			leftJoyX = (abs(normLX) < deadX ? 0 : (abs(normLX) - deadX) * (normLX / abs(normLX)));
			leftJoyY = (abs(normLY) < deadY ? 0 : (abs(normLY) - deadY) * (normLY / abs(normLY)));

			if (deadX > 0) leftJoyX *= 1 / (1 - deadX);
			if (deadY > 0) leftJoyY *= 1 / (1 - deadY);

			rightJoyX = (abs(normRX) < deadX ? 0 : (abs(normRX) - deadX) * (normRX / abs(normRX)));
			rightJoyY = (abs(normRY) < deadY ? 0 : (abs(normRY) - deadY) * (normRY / abs(normRY)));

			if (deadX > 0) rightJoyX *= 1 / (1 - deadX);
			if (deadY > 0) rightJoyY *= 1 / (1 - deadY);
		}
		else {
			rightJoyX = (float)state.Gamepad.sThumbRX / 32767;
			rightJoyY = (float)state.Gamepad.sThumbRY / 32767;

			leftJoyX = (float)state.Gamepad.sThumbLX / 32767;
			leftJoyY = (float)state.Gamepad.sThumbLY / 32767;
		}

		//setting trigger values. Similar to the deadzone, these values are 8 bit int's, so dividing by 255 gives us a percentage, or a
		//value between 0 and 1.
		leftTrigger = (float)state.Gamepad.bLeftTrigger / 255;
		rightTrigger = (float)state.Gamepad.bRightTrigger / 255;

		return true;
	}
	return false;
}

bool Controller::IsPressed(WORD button) {
	return (state.Gamepad.wButtons & button) != 0;
}


void Controller::setcId(int a) {
	cId = a;
}
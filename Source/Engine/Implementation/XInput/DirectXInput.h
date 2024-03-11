
#pragma once

#include "Engine/IInput.h"
#include <windows.h>
#include <XInput.h>

// Class handles controller inputs using XInput system
class DirectXInput : public IInput
{
public:

	DirectXInput();
	virtual ~DirectXInput();

	virtual void Update();

private:

	float CalculateTriggerValue(BYTE rawValue, float threshold = XINPUT_GAMEPAD_TRIGGER_THRESHOLD, float max = 255.0f);
	float CalculateAxisValue(SHORT rawValue, float deadzone, float left = -32768.0f, float right = 32767.0f);

	XINPUT_STATE State;

};


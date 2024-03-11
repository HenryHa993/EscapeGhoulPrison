
#pragma once

enum InputAction
{
	ButtonLeft,
	ButtonRight,
	ButtonTop,
	ButtonBottom,

	DirectionPadLeft,
	DirectionPadRight,
	DirectionPadTop,
	DirectionPadBottom,

	LeftStickXAxis,
	LeftStickYAxis,
	LeftStickPress,

	RightStickXAxis,
	RightStickYAxis,
	RightStickPress,

	TriggerLeft,
	TriggerRight,
	ShoulderButtonLeft,
	ShoulderButtonRight,

	SpecialLeft,
	SpecialRight,

	NumActions,

	DefaultSelect = ButtonBottom,
	DefaultBack = ButtonRight,

};

// Interface for inputs
class IInput
{
public:

	IInput();

	// Manages updating the PreviousState and CurrentState
	virtual void Update();

	virtual float GetValue(InputAction action) const;
	virtual bool IsPressed(InputAction action, float threshold = 0.5f) const;
	virtual bool IsReleased(InputAction action, float threshold = 0.5f) const;
	virtual bool IsHeld(InputAction action, float threshold = 0.5f) const;

	// So as you add to the buttons, NumActions value increases which increases the number of possible input actions
	static const unsigned int NumInputActions = static_cast<unsigned int>(NumActions);

protected:
	
	float CurrentState[NumInputActions];
	float PreviousState[NumInputActions];

};


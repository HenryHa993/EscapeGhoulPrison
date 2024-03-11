#include "IInput.h"
#include <iostream>
#include <math.h>

IInput::IInput() : CurrentState(), PreviousState()
{
}

void IInput::Update()
{
    // Copy CurrentState to PreviousState
    std::memcpy(&PreviousState, &CurrentState, sizeof(float) * NumInputActions);
}

float IInput::GetValue(InputAction action) const
{
    return CurrentState[static_cast<unsigned int>(action)];
}

bool IInput::IsPressed(InputAction action, float threshold) const
{
    return abs(CurrentState[static_cast<unsigned int>(action)]) > threshold && abs(PreviousState[static_cast<unsigned int>(action)]) < threshold;
}

bool IInput::IsReleased(InputAction action, float threshold) const
{
    return abs(CurrentState[static_cast<unsigned int>(action)]) < threshold && abs(PreviousState[static_cast<unsigned int>(action)]) > threshold;
}

bool IInput::IsHeld(InputAction action, float threshold) const
{
    return abs(CurrentState[static_cast<unsigned int>(action)]) > threshold && abs(PreviousState[static_cast<unsigned int>(action)]) > threshold;
}

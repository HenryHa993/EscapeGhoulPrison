#pragma once
#include "Engine/Implementation/GameObjects/Sprite.h"

// Class is used to denote what stage of the puzzle the user is at
class PadlockUI : public Sprite
{
public:
	enum SpriteState
	{
		CLOSED,
		OPEN
	};

	enum IndicatorState;

	PadlockUI(IGraphics* Graphics);

	void SetUpSprites() override;

	void SetUpIndicators() override;

	void Update() override;
};


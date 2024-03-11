#pragma once
#include "Engine/Implementation/GameObjects/Sprite.h"

class BatteryUI : public Sprite
{
public:
	BatteryUI(IGraphics* GraphicsIn);

	void SetUpSprites() override;

	void SetUpIndicators() override;

	void Update() override;
};


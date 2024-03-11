#pragma once
#include "Engine/Implementation/GameObjects/Sprite.h"
#include "Game/EnemyBTs/WardenBT.h"

class Warden : public Sprite
{
public:
	enum SpriteState{
		NORTH,
		EAST,
		SOUTH,
		WEST
	};

	enum IndicatorState
	{
		SEEPLAYER,
		CANNOTSEEPLAYER
	};

	Warden(IGraphics* Graphics);
	void SetUpSprites() override;
	void SetUpIndicators() override;
	void Update() override;
	void SetOffsetY(float Offset);

	WardenBT* BehaviourTree;

private:
	float OffsetY;
	BehaviourTree::NodeTask BTCoroutine;
};


#include "Sprite.h"

#include <stdbool.h>

// Constructor requires graphics so billboards can me made here
Sprite::Sprite(IGraphics* Graphics) : IndicatorOffsetY(0), IndicatorScaleX(1), IndicatorScaleY(1)
{
	this->Graphics = Graphics;
}

// Set position of entire spritemap and active sprite
void Sprite::SetPosition(float x, float y)
{
	IRenderable::SetPosition(x, y);

	//Set all sprites in the same position
	for(auto SpritePair : SpriteMap)
	{
		SpritePair.second->SetPosition(x, y);
	}

	// Set all indicators at an offset
	if(IndicatorMap.empty())
	{
		return;
	}

	for (auto IndicatorPair : IndicatorMap)
	{
		IndicatorPair.second->SetPosition(x, y + IndicatorOffsetY);
	}
}

// Sets rotation for entire spritemap and active sprite
void Sprite::SetRotation(float r)
{
	IRenderable::SetRotation(r);

	for (auto SpritePair : SpriteMap)
	{
		SpritePair.second->SetRotation(r);
	}

	if (IndicatorMap.empty())
	{
		return;
	}

	for (auto IndicatorPair : IndicatorMap)
	{
		IndicatorPair.second->SetRotation(r);
	}

}

// Sets scale for entire spritemap and active sprite
void Sprite::SetScale(float sx, float sy)
{
	IRenderable::SetScale(sx, sy);

	// Indicators and sprites are intended to have different scales, so they are not included here
	for (auto SpritePair : SpriteMap)
	{
		SpritePair.second->SetScale(sx, sy);
	}
}

void Sprite::SetActive(bool b)
{
	IRenderable::SetActive(b);

	if(!IndicatorMap.empty())
	{
		SetActiveIndicatorMap(b);
	}

	if (!IndicatorMap.empty())
	{
		SetActiveSpriteMap(b);
	}
}

int Sprite::GetSpriteState()
{
	return SpriteState;
}

// Helper function
void Sprite::SetActiveSpriteMap(bool b)
{
	if(b == false)
	{
		for (auto SpritePair : SpriteMap)
		{
			SpritePair.second->SetActive(b);
		}
		return;
	}

	if(b == true)
	{
		CurrentSprite->SetActive(true);
	}
}

// Overridable, meant to allow derived classes to set their spritemaps
void Sprite::SetUpSprites()
{
}

// Add new sprite to spritemap
void Sprite::AddSprite(int State, IRenderable* Sprite)
{
	Sprite->SetActive(false);
	SpriteMap[static_cast<int>(State)] = Sprite;
	Sprite->SetPosition(GetTransform().PositionX, GetTransform().PositionY);
	Sprite->SetScale(GetTransform().ScaleX, GetTransform().ScaleY);
	Sprite->SetRotation(GetTransform().Rotation);
}

// Set sprite state / change sprite
void Sprite::SetSpriteState(int State)
{
	SpriteState = State;

	if(CurrentSprite != nullptr)
	{
		CurrentSprite->SetActive(false);
	}

	CurrentSprite = SpriteMap[static_cast<int>(State)];
	CurrentSprite->SetActive(true);
}

void Sprite::SetUpIndicators()
{
}


int Sprite::GetIndicatorState()
{
	return IndicatorState;
}

// Add indicator to indicatormap
void Sprite::AddIndicatorSprite(int State, IRenderable* Sprite)
{
	Sprite->SetActive(false);
	IndicatorMap[static_cast<int>(State)] = Sprite;
	Sprite->SetPosition(GetTransform().PositionX, GetTransform().PositionY + IndicatorOffsetY);
	Sprite->SetScale(IndicatorScaleX, IndicatorScaleY);
	Sprite->SetRotation(GetTransform().Rotation);
}

void Sprite::SetIndicatorState(int State)
{
	IndicatorState = State;

	if (CurrentIndicator != nullptr)
	{
		CurrentIndicator->SetActive(false);
	}

	CurrentIndicator = IndicatorMap[static_cast<int>(State)];
	CurrentIndicator->SetActive(true);

}

// Set indicator scale
void Sprite::SetIndicatorOffsetY(float offsetY)
{
	IndicatorOffsetY = offsetY;

	// Set all indicators at an offset
	for (auto IndicatorPair : IndicatorMap)
	{
		IndicatorPair.second->SetPosition(GetTransform().PositionX, GetTransform().PositionY + IndicatorOffsetY);
	}

}

// Set indicator scale
void Sprite::SetIndicatorScale(float sx, float sy)
{
	IndicatorScaleX = sx;
	IndicatorScaleY = sy;

	for (auto IndicatorPair : IndicatorMap)
	{
		IndicatorPair.second->SetScale(sx, sy);
	}
}

void Sprite::SetActiveIndicatorMap(bool b)
{
	if (b == false)
	{
		for (auto indicatorPair : IndicatorMap)
		{
			indicatorPair.second->SetActive(b);
		}
		return;
	}

	if (b == true)
	{
		CurrentIndicator->SetActive(true);
	}

}

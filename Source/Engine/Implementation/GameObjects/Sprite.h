#pragma once

#include <string>
#include "Engine/IRenderable.h"
#include "map"
#include "Engine/IGraphics.h"

class Sprite : public IRenderable
{
public:
	//enum SpriteState;
	Sprite(IGraphics* Graphics);

	void SetPosition(float x, float y) override;
	void SetRotation(float r) override;
	void SetScale(float sx, float sy) override;
	void SetActive(bool b) override;

	// Sprite map
	virtual void SetUpSprites();

	int GetSpriteState();

	void SetActiveSpriteMap(bool b);
	void AddSprite(int State, IRenderable* Sprite);
	void SetSpriteState(int State);

	// Indicator sprite map
	virtual void SetUpIndicators();

	int GetIndicatorState();

	void SetIndicatorState(int State);
	void AddIndicatorSprite(int State, IRenderable* Sprite);
	void SetIndicatorOffsetY(float offsetY);
	void SetIndicatorScale(float sx, float sy);
	void SetActiveIndicatorMap(bool b);

	IGraphics* Graphics;

private:
	std::map<int, IRenderable*> SpriteMap;
	IRenderable* CurrentSprite;
	int SpriteState;

	std::map<int, IRenderable*> IndicatorMap;
	IRenderable* CurrentIndicator;
	float IndicatorOffsetY;
	float IndicatorScaleX;
	float IndicatorScaleY;
	int IndicatorState;
};


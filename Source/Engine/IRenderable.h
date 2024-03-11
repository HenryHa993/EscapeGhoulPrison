
#pragma once

#include "Engine/IGraphics.h"
#include "Engine/Transform2D.h"

// Represents anything that can be drawn to the screen, has the appropriate setters for each renderable transform component
class IRenderable
{
public:

	virtual void Update() = 0;

	const Transform2D& GetTransform() const;
	virtual void SetPosition(float x, float y);
	virtual void SetRotation(float r);
	virtual void SetScale(float sx, float sy);
	virtual void SetActive(bool b);

	bool IsActive;

private:
	
	Transform2D Transform;
};


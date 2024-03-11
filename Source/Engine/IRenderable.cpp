#include "IRenderable.h"

const Transform2D& IRenderable::GetTransform() const
{
	return Transform;
}

void IRenderable::SetPosition(float x, float y)
{
	Transform.PositionX = x;
	Transform.PositionY = y;
}

void IRenderable::SetRotation(float r)
{
	Transform.Rotation = r;
}

void IRenderable::SetScale(float sx, float sy)
{
	Transform.ScaleX = sx;
	Transform.ScaleY = sy;
}

void IRenderable::SetActive(bool b)
{
	IsActive = b;
}

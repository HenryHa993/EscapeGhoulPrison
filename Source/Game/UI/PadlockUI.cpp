#include "PadlockUI.h"

PadlockUI::PadlockUI(IGraphics* GraphicsIn) : Sprite(GraphicsIn)
{
	SetUpSprites();
}

void PadlockUI::SetUpSprites()
{
	ITexture* ClosedPadlockTexture = Graphics->CreateTexture(L"Resource/Textures/padlock.dds");
	ITexture* OpenPadlockTexture = Graphics->CreateTexture(L"Resource/Textures/padlock-open.dds");

	IShader* ClosedPadlockShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", ClosedPadlockTexture);
	IShader* OpenPadlockShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", OpenPadlockTexture);

	AddSprite(SpriteState::CLOSED, Graphics->CreateBillboard(ClosedPadlockShader));
	AddSprite(SpriteState::OPEN, Graphics->CreateBillboard(OpenPadlockShader));

	SetSpriteState(SpriteState::CLOSED);
	SetScale(0.15f, 0.15f);
	SetRotation(3.14159265359f);
}

void PadlockUI::SetUpIndicators()
{
}

void PadlockUI::Update()
{
}

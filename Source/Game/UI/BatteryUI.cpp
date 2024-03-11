#include "BatteryUI.h"

BatteryUI::BatteryUI(IGraphics* GraphicsIn) : Sprite(GraphicsIn)
{
	SetUpSprites();
}

void BatteryUI::SetUpSprites()
{
	ITexture* FullTexture = Graphics->CreateTexture(L"Resource/Textures/battery-100.dds");
	ITexture* ThreeQuarterTexture = Graphics->CreateTexture(L"Resource/Textures/battery-75.dds");
	ITexture* HalfTexture = Graphics->CreateTexture(L"Resource/Textures/battery-50.dds");
	ITexture* QuarterTexture = Graphics->CreateTexture(L"Resource/Textures/battery-25.dds");
	ITexture* EmptyTexture = Graphics->CreateTexture(L"Resource/Textures/battery-0.dds");

	IShader* FullShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", FullTexture);
	IShader* ThreeQuarterShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", ThreeQuarterTexture);
	IShader* HalfShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", HalfTexture);
	IShader* QuarterShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", QuarterTexture);
	IShader* EmptyShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", EmptyTexture);

	AddSprite(4, Graphics->CreateBillboard(FullShader));
	AddSprite(3, Graphics->CreateBillboard(ThreeQuarterShader));
	AddSprite(2, Graphics->CreateBillboard(HalfShader));
	AddSprite(1, Graphics->CreateBillboard(QuarterShader));
	AddSprite(0, Graphics->CreateBillboard(EmptyShader));

	SetSpriteState(4);
	SetScale(0.15f, 0.15f);
	SetRotation(3.14159265359f);
}

void BatteryUI::SetUpIndicators()
{
}

void BatteryUI::Update()
{
}

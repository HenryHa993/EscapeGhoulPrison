#include "Warden.h"
#include "Engine/ITexture.h"
#include "Engine/IShader.h"

Warden::Warden(IGraphics* Graphics) : Sprite(Graphics), BehaviourTree(new WardenBT(this)), BTCoroutine(BehaviourTree->Evaluate())
{
	SetUpSprites();
	SetUpIndicators();
}

void Warden::SetUpSprites()
{
	ITexture* SouthTexture = Graphics->CreateTexture(L"Resource/Textures/south.dds");
	ITexture* NorthTexture = Graphics->CreateTexture(L"Resource/Textures/north.dds");
	ITexture* EastTexture = Graphics->CreateTexture(L"Resource/Textures/east.dds");
	ITexture* WestTexture = Graphics->CreateTexture(L"Resource/Textures/west.dds");

	IShader* SouthShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", SouthTexture);
	IShader* NorthShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", NorthTexture);
	IShader* EastShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", EastTexture);
	IShader* WestShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", WestTexture);

	AddSprite(SpriteState::SOUTH, Graphics->CreateBillboard(SouthShader));
	AddSprite(SpriteState::NORTH, Graphics->CreateBillboard(NorthShader));
	AddSprite(SpriteState::EAST, Graphics->CreateBillboard(EastShader));
	AddSprite(SpriteState::WEST, Graphics->CreateBillboard(WestShader));

	//SetActiveSpriteMap(false);
	SetSpriteState(SpriteState::NORTH);
}

void Warden::SetUpIndicators()
{
	ITexture* NotSeeTexture = Graphics->CreateTexture(L"Resource/Textures/notsee.dds");
	ITexture* SeeTexture = Graphics->CreateTexture(L"Resource/Textures/see.dds");

	IShader* NotSeeShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", NotSeeTexture);
	IShader* SeeShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", SeeTexture);

	AddIndicatorSprite(IndicatorState::CANNOTSEEPLAYER, Graphics->CreateBillboard(NotSeeShader));
	AddIndicatorSprite(IndicatorState::SEEPLAYER, Graphics->CreateBillboard(SeeShader));

	SetIndicatorScale(0.1f, 0.1f);
	SetIndicatorOffsetY(80);
	SetIndicatorState(IndicatorState::SEEPLAYER);

}

void Warden::Update()
{

	//if(BTCoroutine.Finished())
	//{
	//	BTCoroutine.Restart() ;
	//}

	// Run behaviour tree
	BTCoroutine.Run();

}

void Warden::SetOffsetY(float Offset)
{
	OffsetY = Offset;
}

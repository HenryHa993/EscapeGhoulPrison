#include "Game.h"

#include "Engine/IGraphics.h"
#include "Engine/ITexture.h"
#include "Engine/IShader.h"
#include "Engine/IRenderable.h"
#include "Engine/IInput.h"
#include "Game/NPCs/Warden.h"
#include "Game/UI/PadlockUI.h"
#include "Game/UI/BatteryUI.h"

#include <ctime>
#include <iostream>
#include <math.h>
#include <random>
#include <algorithm>

#define CLAMP(v, x, y) fmin(fmax(v, x), y)

constexpr float OnePie = 3.14159265359f;
constexpr float TwoPies = OnePie * 2.0f;
constexpr float SpinSpeed = 4.0f;
constexpr float UnlockDuration = 1.0f;
constexpr float MinDifference = OnePie / 2;
constexpr float WindowHeight = 1080.0f;
constexpr float ScrollingSpeed = 4000.0f;
constexpr float PowerInterval = 30.0f;


// Returns current game application
IApplication* GetApplication(IGraphics* Graphics, IInput* Input)
{
	return new Game(Graphics, Input);
}

// Initialise class alongside its base class and variables
Game::Game(IGraphics* GraphicsIn, IInput* InputIn) : IApplication(GraphicsIn, InputIn), Padlock(), Lockpick(), State(), CurrentStage(0), CurrentLockTimer(0), PositionOffsetY(0), GameOver(false), CurrentPowerStage(4), PowerTimer(0), DeltaTime(0.016f), WinTolerance(OnePie/10)//, Guard(new Warden(GraphicsIn)), GuardBTCoroutine(Guard->BehaviourTree->Evaluate())
{
	Keyboard = std::make_unique<DirectX::Keyboard>();
}

Game::~Game()
{
}

bool Game::IsValid()
{
	return true;
}

bool Game::Load()
{
	// Load textures and shaders
	ITexture* PadlockTexture = Graphics->CreateTexture(L"Resource/Textures/padlock.dds");
	ITexture* LockpickTexture = Graphics->CreateTexture(L"Resource/Textures/lockpick2.dds");
	ITexture* CellTexture = Graphics->CreateTexture(L"Resource/Textures/cage.dds");
	ITexture* CellBackTexture = Graphics->CreateTexture(L"Resource/Textures/cageback.dds");
	ITexture* PrisonerTexture = Graphics->CreateTexture(L"Resource/Textures/prisoner.dds");
	ITexture* ClownTexture = Graphics->CreateTexture(L"Resource/Textures/clown.dds");
	ITexture* SlimeTexture = Graphics->CreateTexture(L"Resource/Textures/slime.dds");
	ITexture* InvisTexture = Graphics->CreateTexture(L"Resource/Textures/invisible-face.dds");
	ITexture* RobotTexture = Graphics->CreateTexture(L"Resource/Textures/robot-antennas.dds");
	ITexture* GnomeTexture = Graphics->CreateTexture(L"Resource/Textures/bad-gnome.dds");
	ITexture* EscapedTexture = Graphics->CreateTexture(L"Resource/Textures/escaped.dds");
	ITexture* EatenTexture = Graphics->CreateTexture(L"Resource/Textures/eaten.dds");
	ITexture* CaughtTexture = Graphics->CreateTexture(L"Resource/Textures/caught.dds");

	IShader* PadlockShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", PadlockTexture);
	IShader* LockpickShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", LockpickTexture);
	IShader* CellShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", CellTexture);
	IShader* CellBackShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", CellBackTexture);
	IShader* PrisonerShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", PrisonerTexture);
	IShader* ClownShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", ClownTexture);
	IShader* SlimeShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", SlimeTexture);
	IShader* InvisShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", InvisTexture);
	IShader* RobotShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", RobotTexture);
	IShader* GnomeShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", GnomeTexture);
	IShader* EscapedShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", EscapedTexture);
	IShader* EatenShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", EatenTexture);
	IShader* CaughtShader = Graphics->CreateShader(L"Resource/Shaders/UnlitColor.fx", "VS_Main", "vs_4_0", "PS_Main", "ps_4_0", CaughtTexture);

	Padlock = Graphics->CreateBillboard(PadlockShader);
	Lockpick = Graphics->CreateBillboard(LockpickShader);
	Cell1 = Graphics->CreateBillboard(CellShader);
	Cell2 = Graphics->CreateBillboard(CellShader);
	Cell3 = Graphics->CreateBillboard(CellShader);
	Cell4 = Graphics->CreateBillboard(CellBackShader);
	Cell5 = Graphics->CreateBillboard(CellBackShader);
	Cell6 = Graphics->CreateBillboard(CellBackShader);
	Prisoner1 = Graphics->CreateBillboard(ClownShader);
	Prisoner2 = Graphics->CreateBillboard(RobotShader);
	Prisoner3 = Graphics->CreateBillboard(SlimeShader);
	Prisoner4 = Graphics->CreateBillboard(InvisShader);
	Prisoner5 = Graphics->CreateBillboard(PrisonerShader);
	Prisoner6 = Graphics->CreateBillboard(GnomeShader);
	EscapedIcon = Graphics->CreateBillboard(EscapedShader);
	EatenIcon = Graphics->CreateBillboard(EatenShader);
	CaughtIcon = Graphics->CreateBillboard(CaughtShader);

	PowerUI = new BatteryUI(Graphics);

	Guard = new Warden(Graphics);

	State = GameState::Setup;

	return true;
}

// Handles the game states
void Game::Update(float DeltaTimeIn)
{
	DeltaTime = DeltaTimeIn;
	Guard->BehaviourTree->WardenBB->DeltaTime = DeltaTimeIn;

	// Set up assets and textures
	if (State == GameState::Setup)
	{
		SetupLock();
		SetupUI();
		SetupJail();
		SetScrollingAssets();
		SetGameAssets();
		State = GameState::Gameplay;
	}

	// Lockpicking view
	if (State == GameState::Gameplay)
	{
		Guard->Update();
		UpdateScrollingOffset();
		UpdateScrollingAssets();
		UpdateLockpickRotation();
		CheckCaught();
		UpdatePower();
	}

	// This should be taken out
	if (State == GameState::Escaped)
	{
		SetActiveGameAssets(false);
		EscapedIcon->SetActive(true);
		AwaitRestart();
	}

	if (State == GameState::Eaten)
	{
		SetActiveGameAssets(false);
		EatenIcon->SetActive(true);
		AwaitRestart();
	}

	if(State == GameState::Caught)
	{
		SetActiveGameAssets(false);
		CaughtIcon->SetActive(true);
		AwaitRestart();
	}
}

void Game::Cleanup()
{

}

void Game::AwaitRestart()
{
	auto kb = Keyboard->GetState();

	if(kb.R)
	{
		RestartGame();
	}
}

void Game::RestartGame()
{
	// Reset game values
	CurrentStage = 0;
	CurrentLockTimer = 0;
	PositionOffsetY = 0;
	GameOver = false;
	CurrentPowerStage = 4;
	PowerTimer = 0;

	SetActiveGameAssets(true);

	EscapedIcon->SetActive(false);
	EatenIcon->SetActive(false);
	CaughtIcon->SetActive(false);

	PowerUI->SetSpriteState(CurrentPowerStage);

	for(auto lock : LevelStageUI)
	{
		lock->SetSpriteState(PadlockUI::SpriteState::CLOSED);
	}

	WinTolerance = OnePie / (10);
	Guard->BehaviourTree->WardenBB->WaitTime = 4.0f;
	Guard->BehaviourTree->WardenBB->Speed = 500.0f;

	Cell5->SetPosition(0, -350 + WindowHeight);
	LevelStageUI[0]->SetPosition(700, 400);
	LevelStageUI[1]->SetPosition(700, 300);
	LevelStageUI[2]->SetPosition(700, 200);

	SetupLock();

	State = GameState::Gameplay;

}

// Set up different stages of the lockpicking puzzle
void Game::SetupLock()
{
	Padlock->SetRotation(OnePie);
	Padlock->SetScale(0.3f, 0.3f);
	Padlock->SetPosition(0, 25);

	float prevRotation = 0;

	for(unsigned int n = 0; n < NumStages; n++)
	{
		float randRotation = static_cast<float>(fmod(rand(), TwoPies));

		if(abs(randRotation - prevRotation) < MinDifference)
		{
			randRotation = static_cast<float>(fmod(rand(), TwoPies));
		}

		StageSolutions[n] = randRotation;

		std::cout << randRotation << std::endl;

		prevRotation = randRotation;
	}

	Lockpick->SetRotation(static_cast<float>(fmod(rand(), TwoPies)));
}

// Set up jail structure for the level
void Game::SetupJail()
{
	Cell1->SetRotation(OnePie);
	Cell2->SetRotation(OnePie);
	Cell3->SetRotation(OnePie);
	Cell4->SetRotation(OnePie);
	Cell5->SetRotation(OnePie);
	Cell6->SetRotation(OnePie);

	Cell1->SetScale(0.4f, 0.4f);
	Cell2->SetScale(0.4f, 0.4f);
	Cell3->SetScale(0.4f, 0.4f);
	Cell4->SetScale(0.4f, 0.4f);
	Cell5->SetScale(0.4f, 0.4f);
	Cell6->SetScale(0.4f, 0.4f);

	Cell1->SetPosition(-500, 300 + WindowHeight);
	Cell2->SetPosition(0, 300 + WindowHeight);
	Cell3->SetPosition(500, 300 + WindowHeight);
	Cell4->SetPosition(-500, -350 + WindowHeight);
	Cell5->SetPosition(0, -350 + WindowHeight);
	Cell6->SetPosition(500, -350 + WindowHeight);

	Prisoner1->SetRotation(OnePie);
	Prisoner2->SetRotation(OnePie);
	Prisoner3->SetRotation(OnePie);
	Prisoner4->SetRotation(OnePie);
	Prisoner5->SetRotation(OnePie);
	Prisoner6->SetRotation(OnePie);

	Prisoner1->SetScale(0.1f, 0.1f);
	Prisoner2->SetScale(0.1f, 0.1f);
	Prisoner3->SetScale(0.1f, 0.1f);
	Prisoner4->SetScale(0.1f, 0.1f);
	Prisoner5->SetScale(0.1f, 0.1f);
	Prisoner6->SetScale(0.1f, 0.1f);

	Prisoner1->SetPosition(-500, 410 + WindowHeight);
	Prisoner2->SetPosition(0, 410 + WindowHeight);
	Prisoner3->SetPosition(500, 410 + WindowHeight);
	Prisoner4->SetPosition(-500, -220 + WindowHeight);
	Prisoner5->SetPosition(0, -220 + WindowHeight);
	Prisoner6->SetPosition(500, -220 + WindowHeight);

	// Some of this can be set up in the guard constructor
	Guard->SetPosition(500, 0 + WindowHeight);
	Guard->SetScale(0.2f, 0.2f);
	Guard->SetRotation(OnePie);
}

void Game::SetupUI()
{
	// Power UI
	PowerUI->SetPosition(-700, 400);

	// Game finish state UI
	EscapedIcon->SetRotation(OnePie);
	EscapedIcon->SetScale(0.7f, 0.7f);
	EscapedIcon->SetActive(false);

	EatenIcon->SetRotation(OnePie);
	EatenIcon->SetScale(0.7f, 0.7f);
	EatenIcon->SetActive(false);

	CaughtIcon->SetRotation(OnePie);
	CaughtIcon->SetScale(0.7f, 0.7f);
	CaughtIcon->SetActive(false);

	// Stage progress UI
	PadlockUI* Stage1 = new PadlockUI(Graphics);
	PadlockUI* Stage2 = new PadlockUI(Graphics);
	PadlockUI* Stage3 = new PadlockUI(Graphics);

	Stage1->SetPosition(700, 400);
	Stage2->SetPosition(700, 300);
	Stage3->SetPosition(700, 200);

	LevelStageUIOrigins[0].first = 700;
	LevelStageUIOrigins[0].second = 400;
	LevelStageUIOrigins[1].first = 700;
	LevelStageUIOrigins[1].second = 300;
	LevelStageUIOrigins[2].first = 700;
	LevelStageUIOrigins[2].second = 200;

	LevelStageUI[0] = Stage1;
	LevelStageUI[1] = Stage2;
	LevelStageUI[2] = Stage3;
}

void Game::SetScrollingAsset(IRenderable* Asset)
{
	ScrollingAssets.push_back(Asset);
	ScrollingAssetOrigins.push_back(std::make_pair(Asset->GetTransform().PositionX, Asset->GetTransform().PositionY));
}

void Game::SetScrollingAssets()
{
	SetScrollingAsset(Padlock);
	SetScrollingAsset(Lockpick);
	SetScrollingAsset(Cell1);
	SetScrollingAsset(Cell2);
	SetScrollingAsset(Cell3);
	SetScrollingAsset(Cell4);
	SetScrollingAsset(Cell5);
	SetScrollingAsset(Cell6);
	SetScrollingAsset(Prisoner1);
	SetScrollingAsset(Prisoner2);
	SetScrollingAsset(Prisoner3);
	SetScrollingAsset(Prisoner4);
	SetScrollingAsset(Prisoner5);
	SetScrollingAsset(Prisoner6);
	SetScrollingAsset(Guard);
}

void Game::SetGameAsset(IRenderable* Asset)
{
	GameAssets.push_back(Asset);
}

void Game::SetGameAssets()
{
	SetGameAsset(Padlock);
	SetGameAsset(Lockpick);
	SetGameAsset(Cell1);
	SetGameAsset(Cell2);
	SetGameAsset(Cell3);
	SetGameAsset(Cell4);
	SetGameAsset(Cell5);
	SetGameAsset(Cell6);
	SetGameAsset(Prisoner1);
	SetGameAsset(Prisoner2);
	SetGameAsset(Prisoner3);
	SetGameAsset(Prisoner4);
	SetGameAsset(Prisoner5);
	SetGameAsset(Prisoner6);
	SetGameAsset(Guard);
}

void Game::SetActiveGameAssets(bool b)
{
	for(auto gameAsset : GameAssets)
	{
		gameAsset->SetActive(b);
	}
}

// Lockpicking controls
void Game::UpdateLockpickRotation()
{
	auto kb = Keyboard->GetState();
	Guard->BehaviourTree->WardenBB->IsPlayerLockpicking = false;

	// Test solution
	if(kb.W)
	{
		int shakeMagnitude = TestRotation();

		Guard->BehaviourTree->WardenBB->IsPlayerLockpicking = true;
		ShakeAnimBase(LevelStageUI[CurrentStage], 5, LevelStageUIOrigins[CurrentStage].first, LevelStageUIOrigins[CurrentStage].second, false);
		ShakeAnimBase(Lockpick, shakeMagnitude, 0, 0, true);
		ShakeAnimBase(Cell5, 5, 0, -350 + WindowHeight, true);
		return;
	}

	Lockpick->SetPosition(0, PositionOffsetY);

	// Rotate lockpick
	if (kb.D)
	{
		float delta = -SpinSpeed * DeltaTime;
		float rotation = Lockpick->GetTransform().Rotation;
		float newRotation = fmod(rotation + delta, TwoPies);
		Lockpick->SetRotation(newRotation);
		Guard->BehaviourTree->WardenBB->IsPlayerLockpicking = true;

	}
	if(kb.A)
	{
		float delta = SpinSpeed * DeltaTime;
		float rotation = Lockpick->GetTransform().Rotation;
		float newRotation = fmod(rotation + delta, TwoPies);
		Lockpick->SetRotation(newRotation);
		Guard->BehaviourTree->WardenBB->IsPlayerLockpicking = true;

	}

}

// Change views
void Game::UpdateScrollingOffset()
{
	// Player changes view by holding space
	auto kb = Keyboard->GetState();

	if (kb.Space)
	{
		PositionOffsetY -= DeltaTime * ScrollingSpeed;
		PositionOffsetY = CLAMP(PositionOffsetY, -1080, 0);
		return;
	}

	PositionOffsetY += DeltaTime * ScrollingSpeed;
	PositionOffsetY = CLAMP(PositionOffsetY, -1080, 0);
}

// Updating scrolling assets positions
void Game::UpdateScrollingAssets()
{
	auto assetOrigin = ScrollingAssetOrigins.begin();

	// need to update this later
	for (auto asset = ScrollingAssets.begin(); asset != ScrollingAssets.end(); asset++)
	{
		//(*Asset)->SetPosition((*AssetOrigin).first, (*AssetOrigin).second + PositionOffsetY);
		(*asset)->SetPosition((*asset)->GetTransform().PositionX, (*assetOrigin).second + PositionOffsetY);
		assetOrigin++;
	}
}

// Change current stage UI
void Game::UpdateCurrentStageUI()
{
	LevelStageUI[CurrentStage]->SetSpriteState(PadlockUI::SpriteState::OPEN);
}

// Change stage when player solves a puzzle
void Game::UpdateNextStage()
{
	UpdateCurrentStageUI();
	CurrentStage++;

	// Scale difficulty by reducing win tolerance, reducing wait time and increasing speed per stage complete
	WinTolerance = OnePie / (10 + CurrentStage);
	Guard->BehaviourTree->WardenBB->WaitTime -= 1.0f;
	Guard->BehaviourTree->WardenBB->Speed += 100.0f;
	CheckWinCondition();
}

// Timer for when the power shuts off
void Game::UpdatePower()
{
	if(CurrentPowerStage < 1)
	{
		State = GameState::Eaten;
		return;
	}

	PowerTimer += DeltaTime;

	if(PowerTimer > PowerInterval)
	{
		PowerTimer = 0;
		CurrentPowerStage--;
		PowerUI->SetSpriteState(CurrentPowerStage);
	}
}

// Test solution by holding W
// Returns a magnitude which is used for the shake animations. Allows the player to know when they have hit the right spot.
int Game::TestRotation()
{
	// Check if correct position has been picked for an adequate amount of time
	if(CurrentLockTimer > UnlockDuration)
	{
		std::cout << "Next stage" << std::endl;
		CurrentLockTimer = 0;
		UpdateNextStage();
		// We also need some sort of feedback
	}

	// Check if rotation is correct for given stage
	float lockpickRotation = Lockpick->GetTransform().Rotation + TwoPies;
	float normalisedLockpickRotation = fmod(lockpickRotation, TwoPies);
	float normalisedSolutionRotation = fmod(StageSolutions[CurrentStage] + TwoPies, TwoPies);
	float totalDifference = abs(normalisedLockpickRotation - normalisedSolutionRotation);

	if (totalDifference < WinTolerance)
	{
		// Win
		//std::cout << "Win" << std::endl;
		std::cout << "Lock Progress: " << CurrentLockTimer << std::endl;
		CurrentLockTimer += DeltaTime;
		return 30;
	}

	// wrong
	//std::cout << "Lose" << std::endl;
	return 5;
}

// Test all stages passed
void Game::CheckWinCondition()
{
	if(CurrentStage >= NumStages)
	{
		std::cout << "You escaped!" << std::endl;
		CurrentStage = 0;
		State = GameState::Escaped;
	}
}

// Check if guard has caught player
void Game::CheckCaught()
{
	if(Guard->BehaviourTree->WardenBB->Caught)
	{
		Guard->BehaviourTree->WardenBB->Caught = false;
		State = GameState::Caught;
	}
}

// Shake animation
void Game::ShakeAnimBase(IRenderable* Billboard, int Magnitude, float xOrigin, float yOrigin, bool OffsetActive)
{
	int maxOffset = Magnitude;
	int minOffset = -Magnitude;

	float xOffset = static_cast<float>(rand() % (maxOffset - minOffset + 1) + minOffset);
	float yOffset = static_cast<float>(rand() % (maxOffset - minOffset + 1) + minOffset);

	if(OffsetActive)
	{
		Billboard->SetPosition(xOrigin + xOffset, yOrigin + yOffset + PositionOffsetY);
		return;
	}
	Billboard->SetPosition(xOrigin + xOffset, yOrigin + yOffset);
}

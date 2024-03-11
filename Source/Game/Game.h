
#pragma once

#include "Engine/IApplication.h"
//#include "Game/NPCs/Warden.h"

#include <Keyboard.h>
#include <list>

#include "Engine/Implementation/DirectX11/DirectX11Shader.h"

class IGraphics;
class ITexture;
class IShader;
class IRenderable;
class Warden;
class PadlockUI;
class BatteryUI;

//class Keyboard;

enum GameState { Setup, Gameplay, Escaped, Eaten, Caught, NumModes };

static const unsigned int NumStages = 3;

class Game : public IApplication
{
public:

	Game(IGraphics* Graphics, IInput* InputIn);
	virtual ~Game();

	virtual bool IsValid();
	virtual bool Load();
	virtual void Update(float DeltaTimeIn);
	virtual void Cleanup();

private:
	// Setup functions
	void AwaitRestart();
	void RestartGame();

	void SetupLock();
	void SetupJail();
	void SetupUI();

	void SetScrollingAsset(IRenderable* Asset);
	void SetScrollingAssets();

	void SetGameAsset(IRenderable* Asset);
	void SetGameAssets();
	void SetActiveGameAssets(bool b);

	void UpdateLockpickRotation();
	void UpdateScrollingOffset();
	void UpdateScrollingAssets();
	void UpdateCurrentStageUI();

	void UpdateNextStage();
	void UpdatePower();

	int TestRotation();

	void CheckWinCondition();
	void CheckCaught();

	// Shake animation, takes billboard, original location and shake amount
	void ShakeAnimBase(IRenderable* Billboard, int Magnitude, float xOrigin, float yOrigin, bool OffsetActive);

	GameState State;

	std::unique_ptr<DirectX::Keyboard> Keyboard;

	IRenderable* Padlock;
	IRenderable* Lockpick;

	IRenderable* Cell1;
	IRenderable* Cell2;
	IRenderable* Cell3;
	IRenderable* Cell4;
	IRenderable* Cell5;
	IRenderable* Cell6;

	IRenderable* Prisoner1;
	IRenderable* Prisoner2;
	IRenderable* Prisoner3;
	IRenderable* Prisoner4;
	IRenderable* Prisoner5;
	IRenderable* Prisoner6;

	Warden* Guard;

	IRenderable* EscapedIcon;
	IRenderable* EatenIcon;
	IRenderable* CaughtIcon;

	bool GameOver;
	float WinTolerance;
	float DeltaTime;

	int CurrentStage;
	float StageSolutions[NumStages];
	float CurrentLockTimer;
	float PositionOffsetY;

	// Manages game specific assets
	std::list<IRenderable*> GameAssets;

	// Manages scrolling assets
	std::list<IRenderable*> ScrollingAssets;
	std::list<std::pair<float, float>> ScrollingAssetOrigins;

	PadlockUI* LevelStageUI[3];
	std::pair<float, float> LevelStageUIOrigins[3];

	BatteryUI* PowerUI;
	int CurrentPowerStage;
	float PowerTimer;

};


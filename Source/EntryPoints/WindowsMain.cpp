// The main Windows / DirectX Graphics / XInput entry point for Star Applications

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <Windows.h>
#include "Engine/Implementation/DirectX11/DirectX11Graphics.h"
#include "Engine/Implementation/XInput/DirectXInput.h"
#include "Engine/IRenderable.h"
#include "Engine/ITexture.h"
#include "Engine/IShader.h"
#include "Engine/IApplication.h"

#include <Keyboard.h>
#include <chrono>

const char WindowClassName[] = "Star";
const char WindowTitle[] = "Escape Ghoul Prison";
const int WindowWidth = 1920;
const int WindowHeight = 1080;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
IApplication* GetApplication(IGraphics* Graphics, IInput* Input);

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Register windows class
	WNDCLASSEX wc;
	HWND hwnd;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WindowClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// Console for debugging
	//if(!AllocConsole())
	//{
	//	MessageBox(NULL, "Console Creation Failed!", NULL, MB_ICONEXCLAMATION);
	//}

	//FILE* fp;

	//freopen_s(&fp, "CONOUT$", "w", stdout);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Create window instance
	hwnd = CreateWindowEx( WS_EX_CLIENTEDGE, WindowClassName, WindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WindowWidth, WindowHeight, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Show window
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	msg.message = WM_NULL;
	msg.wParam = -1;

	// Seems like I might have to include separate views here
	IGraphics * Graphics = new DirectX11Graphics(hwnd);
	IInput* Input = new DirectXInput();
	IApplication* Application = GetApplication(Graphics, Input);

	if (Graphics && Graphics->IsValid() && Application)
	{
		Application->Load();

		// Delta time initiate
		auto currentTime = std::chrono::steady_clock::now();//high_resolution_clock::now();
		float deltaTime = 0.016f;

		// Game loop
		while (msg.message != WM_QUIT && Application->IsValid())
		{
			// I NEED DELTA TIME
			auto nextTime = std::chrono::steady_clock::now();//high_resolution_clock::now();
			//deltaTime = std::chrono::duration_cast<float>(currentTime - nextTime);
			deltaTime = std::chrono::duration<float>(nextTime - currentTime).count();
			currentTime = nextTime;
			
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			Input->Update();
			Application->Update(deltaTime);
			Graphics->Update();
		}

		Application->Cleanup();
	}

	if (Application)
	{
		delete Application;
	}

	if (Graphics)
	{
		delete Graphics;
	}

	return static_cast<int>(msg.wParam);
}

// Windows procedures, how your window behaves based on events that happen on the window
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			// This is where you'd implement the classic ALT+ENTER hotkey for fullscreen toggle
			break;
		}
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_CLOSE:
		//if (!FreeConsole())
		//	MessageBox(NULL, "Failed Free Console!", NULL, MB_ICONEXCLAMATION);
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

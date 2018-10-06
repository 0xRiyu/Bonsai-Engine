#include "System.h"
#include <iostream>

namespace bonsai {
	System::System()
	{
		m_Input = nullptr;
		m_Scene = nullptr;
	}

	System::System(const System& other)
	{
	}

	System::~System()
	{
	}

	bool System::Initialize()
	{
		int screenWidth(0);
		int screenHeight(0);

		InitializeWindows(screenWidth, screenHeight);

		m_Input = new InputHandler(m_Hwnd);
		if (!m_Input)
		{
			return false;
		}

		m_Input->Initialize();

		m_Scene = new Scene();
		if (!m_Scene)
		{
			return false;
		}

		GetSystemTime(&m_LastTime);

		bool result = m_Scene->Initialize(screenWidth, screenHeight, m_Hwnd);
		return result;
	}

	void System::Shutdown()
	{
		if (m_Scene)
		{
			m_Scene->Shutdown();
			delete m_Scene;
			m_Scene = nullptr;
		}

		if (m_Input)
		{
			delete m_Input;
			m_Input = nullptr;
		}
		ShutdownWindows();

	}

	void System::Run()
	{
		MSG msg;
		bool done = (false);

		ZeroMemory(&msg, sizeof(MSG));

		while (!done)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					done = true;
				}

				if (msg.message == WM_INPUT)
				{
					m_Input->ParseMouseInput(m_Scene->GetCamera(), msg.lParam);
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			bool result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	bool System::Frame()
	{

		if (m_Input->IsKeyDown(VK_ESCAPE))
		{
			return false;
		}
		SYSTEMTIME m_CurrentTime;
		GetSystemTime(&m_CurrentTime);
		m_NumberOfFrames++;

		//To include the case where current time seconds resets to 0
		float deltaTime = m_CurrentTime.wMilliseconds - m_LastTime.wMilliseconds;
		
		
		if (m_CurrentTime.wSecond - m_LastTime.wSecond >= 1.0f  || m_CurrentTime.wSecond - m_LastTime.wSecond < -1.0f)
		{
			char FPScount[48];
			sprintf_s(FPScount, "FPS: %d", (int)m_NumberOfFrames);

			m_Scene->m_Text->UpdateText("FPS String", FPScount, 10, 10, 1.0, 1.0, 1.0);
			m_NumberOfFrames = 0.0f;
			
		}
		
		GetSystemTime(&m_LastTime);
		//Hack for when resets, find new implimentation in future
		if (deltaTime < 0)
		{
			deltaTime = m_CurrentTime.wMilliseconds - m_LastTime.wMilliseconds;
		}

		m_Input->ParseKeyboardInput(m_Scene->GetCamera(), deltaTime);

		return m_Scene->Frame();
	}

	LRESULT System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
	{
		switch (umsg)
		{
		case WM_KEYDOWN:
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		case WM_KEYUP:
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		case WM_ACTIVATE:
			if (wparam == WA_ACTIVE) {
				RECT rect;
				GetWindowRect(m_Hwnd, &rect);
				ClipCursor(&rect);
			}
			return 0;


		default:
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}

	void System::InitializeWindows(int& screenWidth, int& screenHeight)
	{
		WNDCLASSEX wc;
		DEVMODE dmScreenSettings;
		int posX, posY;

		ApplicationHandle = this;

		m_Hinstance = GetModuleHandle(NULL);

		m_ApplicationName = L"Bonsai";

		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_Hinstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = m_ApplicationName;
		wc.cbSize = sizeof(WNDCLASSEX);

		RegisterClassEx(&wc);

		screenWidth = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);

		if (FULL_SCREEN)
		{
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
			dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			posX = posY = 0;
		}
		else
		{
			screenWidth = 800;
			screenHeight = 640;

			posX = GetSystemMetrics((SM_CXSCREEN - screenWidth) / 2);
			posY = GetSystemMetrics((SM_CYSCREEN - screenHeight) / 2);
		}

		m_Hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_ApplicationName, m_ApplicationName,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
			posX, posY, screenWidth, screenHeight, NULL, NULL, m_Hinstance, NULL);

		ShowWindow(m_Hwnd, SW_SHOW);
		SetForegroundWindow(m_Hwnd);
		SetFocus(m_Hwnd);
		RECT rect;
		GetWindowRect(m_Hwnd, &rect);
		ClipCursor(&rect);

		ShowCursor(false);

	}
	void System::ShutdownWindows()
	{
		ShowCursor(true);
		if (FULL_SCREEN)
		{
			ChangeDisplaySettings(NULL, 0);
		}

		DestroyWindow(m_Hwnd);
		m_Hwnd = nullptr;

		UnregisterClass(m_ApplicationName, m_Hinstance);
		m_Hinstance = nullptr;

		ApplicationHandle = nullptr;

	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
	{
		switch (umessage)
		{
		case WM_CREATE:
		{
			HMENU hmenu = CreateMenu();
			HMENU titlemenu = CreateMenu();
			AppendMenu(hmenu, MF_POPUP, (UINT_PTR)titlemenu, L"Bonsai Engine");
			SetMenu(hwnd, hmenu);
			return 0;
		}


		case WM_DESTROY:
			PostQuitMessage(0);

			return 0;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		default:
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}


}

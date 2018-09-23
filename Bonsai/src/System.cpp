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
		if(!m_Input)
		{
			return false;
		}

		m_Input->Initialize();

		m_Scene = new Scene();
		if(!m_Scene)
		{
			return false;
		}

		bool result = m_Scene->Initialize(screenWidth, screenHeight, m_Hwnd);
		return result;
	}

	void System::Shutdown()
	{
		if(m_Scene)
		{
			m_Scene->Shutdown();
			delete m_Scene;
			m_Scene = nullptr;
		}

		if(m_Input)
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

		while(!done)
		{
			if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if(msg.message == WM_QUIT)
			{
				done = true;
			} else
			{
				bool result = Frame();
				if(!result)
				{
					done = true;
				}
			}
		}
	}

	bool System::Frame()
	{
		bool result;
		
		
		if (m_Input->IsKeyDown(VK_ESCAPE))
		{
			return false;
		}

		

		result = m_Scene->Frame();
		return result;
	}

	LRESULT System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
	{
		switch(umsg)
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

		case WM_INPUT:
			m_Input->ParseInputs(m_Scene->GetCamera(), lparam);
			
			
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

		if(FULL_SCREEN)
		{
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
			dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			posX = posY = 0;
		} else
		{
			screenWidth = 800;
			screenHeight = 600;

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
		switch(umessage)
		{
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

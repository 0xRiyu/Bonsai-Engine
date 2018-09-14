#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include "InputHandler.h"
#include "../graphics/Scene.h"

namespace bonsai {
	class SystemClass
	{
	public:
		SystemClass();
		SystemClass(const SystemClass&);
		~SystemClass();

		bool Initialize();
		void Shutdown();
		void Run();

		LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	private:
		bool Frame();
		void InitializeWindows(int&, int&);
		void ShutdownWindows();

	private:
		LPCWSTR m_ApplicationName;
		HINSTANCE m_Hinstance;
		HWND m_Hwnd;

		InputHandler* m_Input;
		Scene* m_Scene;
	};


	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	static SystemClass* ApplicationHandle = 0;
}
#pragma once
#include "../bonsai.h"
namespace bonsai {
	class RawInput
	{
	public:

		RawInput(HWND hWnd = NULL);
		~RawInput();

		//for generic joypad support
		bool RegisterJoys(HWND hWnd);
		//disables legacy mouse and keyboard support
		bool RegisterKeyboardMouse(HWND hWnd);
		//use this in WndProc in case of WM_INPUT event
		void RetrieveData(LPARAM lParam);
		//read buffered data
		void RetrieveBufferedData();

		RAWINPUT raw;

	};
}
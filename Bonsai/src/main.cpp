#include "System.h"

	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
	{
		bonsai::System* system = new bonsai::System();

		if(!system)	return 0;

		bool result(system->Initialize());
		if(result)	system->Run();

		system->Shutdown();
		delete system;
		system = nullptr;

		return 0;
	}

#include "Systemclass.h"
namespace bonsai
{
	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
	{
		SystemClass* system = new SystemClass();
		bool result;

		if(!system)
		{
			return 0;
		}

		result = system->Initialize();
		if(result)
		{
			system.Run();
		}

		system->Shutdown();
		delete system;
		system = nullptr;

		return 0;
	}
}
#pragma once
#include <Windows.h>
namespace bonzai {
	const bool FULL_SCREEN(false);
	const bool VSYNC_ENABLED(true);
	const float SCREEN_DEPTH(1000.0f);
	const float SCREEN_NEAR(0.1f);

	class Scene
	{
	public:
		Scene();
		Scene(const Scene& scene);
		~Scene();

		bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
		void Shutdown();
		bool Frame();

	private:
		bool Render();
	};
}
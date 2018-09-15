#include "Scene.h"
namespace bonsai {
	namespace graphics {
		Scene::Scene()
		{
			m_Direct3D = nullptr;
		}

		Scene::Scene(const Scene& scene)
		{
		}

		Scene::~Scene()
		{
		}

		bool Scene::Initialize(int screenWidth, int screenHeight, HWND hwnd)
		{
			m_Direct3D = new Direct3D();
			if (!m_Direct3D) return false;
			bool result(m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR));

			if (!result)
			{
				MessageBox(hwnd, L"Could not initalize Direct3D", L"Error", MB_OK);
				return false;
			}

			return true;
		}

		void Scene::Shutdown()
		{
			if (m_Direct3D)
			{
				m_Direct3D->Shutdown();
				delete m_Direct3D;
				m_Direct3D = nullptr;
			}
		}

		bool Scene::Frame()
		{
			return Render();
		}

		bool Scene::Render()
		{
			m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);
			m_Direct3D->EndScene();
			return true;
		}
	}
}
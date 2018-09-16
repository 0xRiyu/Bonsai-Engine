#include "Scene.h"
namespace bonsai {
	namespace graphics {
		Scene::Scene()
			:m_Direct3D(nullptr), m_Camera(nullptr), m_Model(nullptr), m_ColorShader(nullptr)
		{
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
			char videoCardName[128];
			int memory;
			m_Direct3D->GetVideoCardInfo(videoCardName, memory);
			OutputDebugStringA(strcat(videoCardName,"\n"));
			char memoryStr[256];
			sprintf(memoryStr, "RAM: %d MB\n", memory);
			OutputDebugStringA(memoryStr);

			m_Camera = new Camera();
			if (!m_Camera) return false;
			m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

			m_Model = new Model();
			if (!m_Model) return false;
			result = m_Model->Initialize(m_Direct3D->GetDevice());
			if (!result)
			{
				MessageBox(hwnd, L"Could not initalize the model object.", L"Error", MB_OK);
				return false;
			}

			m_ColorShader = new Shader();
			if (!m_ColorShader) return false;
			result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
			if (!result)
			{
				MessageBox(hwnd, L"Could not initalize the Colorshader.", L"Error", MB_OK);
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

			if (m_ColorShader)
			{
				m_ColorShader->Shutdown();
				delete m_ColorShader;
				m_ColorShader = nullptr;
			}

			if (m_Camera)
			{
				delete m_Camera;
				m_Camera = nullptr;
			}

			if (m_Model)
			{
				m_Model->Shutdown();
				delete m_Model;
				m_Model = nullptr;
			}
		}

		bool Scene::Frame()
		{
			return Render();
		}

		bool Scene::Render()
		{
			XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
			bool result;

			ID3D11DeviceContext* deviceContext = m_Direct3D->GetDeviceContext();

			m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

			m_Camera->Update();

			//callbacks
			m_Direct3D->GetWorldMatrix(worldMatrix);
			m_Camera->GetViewMatrix(viewMatrix);
			m_Direct3D->GetProjectionMatrix(projectionMatrix);

			m_Model->Render(deviceContext);

			result = m_ColorShader->Render(deviceContext, m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
			if (!result) return false;
			//OutputDebugString(L"test\n");
			m_Direct3D->EndScene();
			return true;
		}
	}
}
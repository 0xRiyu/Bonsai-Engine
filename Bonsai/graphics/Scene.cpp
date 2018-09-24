#include "Scene.h"
namespace bonsai {
	namespace graphics {
		Scene::Scene()
			:m_Direct3D(nullptr), m_Camera(nullptr), m_Model(nullptr), m_TextureShader(nullptr), m_Light(nullptr),m_Image2D(nullptr)
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
			m_Camera->SetPosition(0.0f, 0.0f, -15.0f);
			m_Camera->SetRotation(25.0, 0.0f, 0.0f);
			m_Camera->Update();

			m_Model = new Model();
			if (!m_Model) return false;
		
			result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "resources/textures/bonsai_small.tga", "resources/obj/cube.txt");
			if (!result)
			{
				MessageBox(hwnd, L"Could not initalize the model object.", L"Error", MB_OK);
				return false;
			}

	

			m_TextureShader = new Shader();
			if (!m_TextureShader) return false;
			result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
			if (!result)
			{
				MessageBox(hwnd, L"Could not initalize the Texture shader.", L"Error", MB_OK);
				return false;
			}

			m_Light = new Light();
			if (!m_Light) return false;
			m_Light->SetAmbientColor(0.35f, 0.35f, 0.35f, 1.0f);
			m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
			m_Light->SetDirection(1.0f, -1.0f, 1.0f);


			m_Image2D = new Img2D();
			if (!m_Image2D) return false;
			result = m_Image2D->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, "resources/textures/bonsai_small.tga", 350,350);
			if (!result)
			{
				MessageBox(hwnd, L"Could not initalize the 2DTexture.", L"Error", MB_OK);
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

			if (m_TextureShader)
			{
				m_TextureShader->Shutdown();
				delete m_TextureShader;
				m_TextureShader = nullptr;
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

			if (m_Light)
			{
				delete m_Light;
				m_Light = nullptr;
			}

			if (m_Image2D)
			{
				m_Image2D->ShutDown();
				delete m_Image2D;
				m_Image2D = nullptr;
			}
		}

		bool Scene::Frame()
		{
			
			return Render();
		}

		bool Scene::Render()
		{
			static float rotation = 0.0f;
			static bool flip(false);

			if (flip) {
				rotation += (float)XM_PI * 0.005f;
			}
			else
			{
				rotation -= (float)XM_PI * 0.005f;
			}

			if (rotation > 360) flip = false;
			if (rotation < -360) flip = true;




			XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, worldMatrix2D, viewMatrix2D;
			bool result;

			ID3D11DeviceContext* deviceContext = m_Direct3D->GetDeviceContext();

			m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

			m_Camera->Update();

			//callbacks
			m_Direct3D->GetWorldMatrix(worldMatrix);
			m_Direct3D->GetWorldMatrix(worldMatrix2D);
			m_Camera->GetViewMatrix(viewMatrix);
			m_Camera->GetViewMatrix(viewMatrix2D);
			m_Direct3D->GetProjectionMatrix(projectionMatrix);
			m_Direct3D->GetOrthoMatrix(orthoMatrix);


			worldMatrix = worldMatrix * XMMatrixRotationY(rotation  * 0.0174533);

			//Translate the orth projection in front of the camera slightly
			XMVECTOR camloc = m_Camera->GetPositionVector();
			XMVECTOR lookatVec = m_Camera->GetLookAtVector();
			camloc +=  XMVector3Normalize(lookatVec);

			worldMatrix2D = worldMatrix2D * XMMatrixRotationRollPitchYawFromVector(m_Camera->GetRotationVectorRads());
			worldMatrix2D = worldMatrix2D * XMMatrixTranslationFromVector(camloc);

			m_Direct3D->TurnZBufferOn();
			
			m_Model->Render(deviceContext);

			result = m_TextureShader->Render(deviceContext, m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model->GetTexture(),m_Light->GetAmbientColor() ,m_Light->GetDiffuseColor(), m_Light->GetDirection());
			if (!result) return false;

			m_Direct3D->TurnZBufferOff()
			;
			result = m_Image2D->Render(deviceContext, 50, 50);
			if (!result) return false;

			result = m_TextureShader->Render(deviceContext, m_Image2D->GetIndexCount(), worldMatrix2D, viewMatrix2D, orthoMatrix,
				m_Image2D->GetTexture(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection());
			if (!result) return false;


			m_Direct3D->TurnZBufferOn();
			
			//OutputDebugString(L"test\n");
			m_Direct3D->EndScene();
			return true;
		}
	}
}
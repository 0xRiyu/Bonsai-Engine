#include "Scene.h"
namespace bonsai {
	namespace graphics {
		Scene::Scene()
			:m_Direct3D(nullptr), m_Camera(nullptr), m_Model(nullptr), m_TextureShader(nullptr), m_Light(nullptr), m_Text(nullptr)
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
			XMMATRIX baseViewMatrix;

			m_Direct3D = new Direct3D();
			if (!m_Direct3D) return false;
			bool result(m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR));

			if (!result)
			{
				MessageBox(hwnd, L"Could not initalize Direct3D", L"Error", MB_OK);
				return false;
			}

			//Outputs VideoCard Info
			char videoCardName[128];
			int memory;
			m_Direct3D->GetVideoCardInfo(videoCardName, memory);
			OutputDebugStringA(strcat(videoCardName,"\n"));
			char memoryStr[256];
			sprintf(memoryStr, "RAM: %d MB\n", memory);
			OutputDebugStringA(memoryStr);

			m_Camera = new Camera(screenWidth, screenHeight);
			if (!m_Camera) return false;
			m_Camera->SetPosition(0.0f, 0.0f, -0.0f);
			m_Camera->SetRotation(0.0, 0.0f, 0.0f);
			m_Camera->Update();
			m_Camera->GetViewMatrix(baseViewMatrix);


			m_Model = new Model();
			if (!m_Model) return false;
		
			result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "resources/obj/flower/flower.tga", "resources/obj/flower/flower.txt");
			if (!result)
			{
				MessageBox(hwnd, L"Could not initalize the model object.", L"Error", MB_OK);
				return false;
			}
			

			//TODO: Finish later
			//m_Model->LoadOBJModel("resources/obj/flower/flower.obj");

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


			m_Text = new Text();
			if (!m_Text) return false;

			result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
			if (!result)
			{
				MessageBox(hwnd, L"Could not initalize the Text.", L"Error", MB_OK);
				return false;
			}

			m_Text->PushBackText("FPS String","FPS Counter", 10, 10, 1.0f, 1.0f, 1.0f);
			m_Text->PushBackText("RenderCount", "Render Count: ", 10, 30, 1, 1, 1);

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

			if(m_Text)
			{
				m_Text->Shutdown();
				delete m_Text;
				m_Text = nullptr;
			}

		}

		bool Scene::Frame()
		{
			
			return Render();
		}

		bool Scene::Render()
		{
			static float rotation = 0.0f;

			if (rotation > 360) {
				rotation = 0;
				
			}
			//rotation += (float)XM_PI * 0.005f;
			XMMATRIX viewMatrix, projectionMatrix, orthoMatrix, worldMatrix2D, viewMatrix2D;
			bool result;

			ID3D11DeviceContext* deviceContext = m_Direct3D->GetDeviceContext();

			m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

			m_Camera->Update();

			//callbacks
			m_Direct3D->GetWorldMatrix(worldMatrix2D);
			m_Camera->GetViewMatrix(viewMatrix);
			m_Camera->GetViewMatrix(viewMatrix2D);
			m_Camera->GetProjectionMatrix(projectionMatrix);
			m_Camera->GetOrthoMatrix(orthoMatrix);


			float rads = 0.0174533;
			XMMATRIX rotMatrix = XMMatrixRotationZ(rotation  * rads) * XMMatrixRotationX(90 * rads);


			XMMATRIX scaleMatrix = XMMatrixScaling(0.35, 0.35, 0.35);

			//Translate the orth projection in front of the camera slightly
			XMVECTOR camloc = m_Camera->GetPositionVector();
			XMVECTOR lookatVec = m_Camera->GetLookAtVector();
			camloc +=  XMVector3Normalize(lookatVec);

			worldMatrix2D = worldMatrix2D * XMMatrixRotationRollPitchYawFromVector(m_Camera->GetRotationVectorRads());
			worldMatrix2D = worldMatrix2D * XMMatrixTranslationFromVector(camloc);

			m_Direct3D->TurnZBufferOn();
			
			m_Model->Render(deviceContext);

			int rendercount = 0;
			for (int i = -15; i < 15; i+=5)	{
				for (int j = -15; j < 15; j += 5) {
					for (int k = -15; k < 15; k += 5) {

						if (m_Camera->m_Frustum->CheckCube(i, j, k, 1)) {
							XMMATRIX worldMatrixInc = rotMatrix * scaleMatrix *  XMMatrixTranslation(i, j, k);

							result = m_TextureShader->Render(deviceContext, m_Model->GetIndexCount(), worldMatrixInc, viewMatrix, projectionMatrix,
								m_Model->GetTexture(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection());
							if (!result) return false;
							rendercount++;
						}

					}
				}
			}
			char counterstr[256];
			sprintf(counterstr, "Render Count: %d", rendercount);
			m_Text->UpdateText("RenderCount", counterstr, 10, 30, 1, 1, 1);
			
			m_Direct3D->TurnZBufferOff();

			m_Direct3D->TurnOnAlphaBlending();
			result = m_Text->Render(deviceContext, worldMatrix2D, viewMatrix2D, orthoMatrix);
			if (!result) return false;
			m_Direct3D->TurnOffAlphaBlending();

			m_Direct3D->TurnZBufferOn();
			
			//OutputDebugString(L"test\n");
			m_Direct3D->EndScene();
			return true;
		}
	}
}
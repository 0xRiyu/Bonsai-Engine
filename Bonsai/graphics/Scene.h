#pragma once
#include "D3D.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"

namespace bonsai {
	namespace graphics {
		const bool FULL_SCREEN(false);
		const bool VSYNC_ENABLED(false);
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

		private:
			Direct3D * m_Direct3D;
			Camera* m_Camera;
			Model* m_Model;
			Shader* m_ColorShader;
		};
	}
}
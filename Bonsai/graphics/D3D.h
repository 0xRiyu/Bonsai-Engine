#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include "../bonsai.h"

using namespace DirectX;

namespace bonsai {
	namespace graphics {
		class Direct3D
		{
		public:
			Direct3D();
			Direct3D(const Direct3D&);
			~Direct3D();

			bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
			void Shutdown();

			void BeginScene(float red, float green, float blue, float alpha);
			void EndScene();

			inline ID3D11Device* GetDevice() { return m_Device; }
			inline ID3D11DeviceContext* GetDeviceContext() { return m_DeviceContext; }

			inline void GetProjectionMatrix(XMMATRIX& proj) { proj=  m_ProjectionMatrix; }
			inline void GetWorldMatrix(XMMATRIX& world) { world = m_WorldMatrix; }
			inline void GetOrthoMatrix(XMMATRIX& orth) { orth = m_OrthoMatrix; }

			inline XMMATRIX GetProjectionMatrix() const { return m_ProjectionMatrix; }
			inline XMMATRIX GetWorldMatrix() const { return m_WorldMatrix; }
			inline XMMATRIX GetOrthoMatrix() const { return m_OrthoMatrix; }

			void GetVideoCardInfo(char*, int&);
		private:
			bool m_vsync_enabled;
			int m_VideoCardMemory;
			char m_VideoCardDescription[128];
			IDXGISwapChain* m_SwapChain;
			ID3D11Device* m_Device;
			ID3D11DeviceContext* m_DeviceContext;
			ID3D11RenderTargetView* m_RenderTargetView;
			ID3D11Texture2D* m_DepthStencilBuffer;
			ID3D11DepthStencilState* m_DepthStencilState;
			ID3D11DepthStencilView* m_DepthStencilView;
			ID3D11RasterizerState* m_RasterState;
			XMMATRIX m_ProjectionMatrix;
			XMMATRIX m_WorldMatrix;
			XMMATRIX m_OrthoMatrix;

		};
	}
}
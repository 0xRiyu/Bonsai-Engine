#pragma once
#include "../bonsai.h"
using namespace DirectX;
using namespace std;

namespace bonsai
{
	namespace graphics
	{
		class Shader
		{

		private:
			struct MatrixBufferType
			{
				XMMATRIX world;
				XMMATRIX view;
				XMMATRIX projection;
			};
			
		public:
			Shader();
			Shader(const Shader& other);
			~Shader();

			bool Initialize(ID3D11Device* device, HWND hwnd);
			void Shutdown();
			bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix);

		private:
			bool InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vert, const WCHAR* frag);
			void ShutdownShader();
			void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename);

			bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix);
			void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

		private:
			ID3D11VertexShader * m_VertexShader;
			ID3D11PixelShader * m_FragmentShader;
			ID3D11InputLayout* m_Layout;
			ID3D11Buffer* m_MatrixBuffer;
		};
	}
}
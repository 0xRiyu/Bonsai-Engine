#pragma once
#include "../bonsai.h"
#include <SimpleMath.h>
using namespace DirectX;
using namespace std;

namespace bonsai
{
	namespace graphics
	{
		class FontShader
		{

		private:
			struct MatrixBufferType
			{
				XMMATRIX world;
				XMMATRIX view;
				XMMATRIX projection;
			};

			struct PixelBufferType
			{
				XMFLOAT4 pixelColor;
			};


		public:
			FontShader();
			FontShader(const FontShader& other);
			~FontShader();

			bool Initialize(ID3D11Device* device, HWND hwnd);
			void Shutdown();
			bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix,
				ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor);

		private:
			bool InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vert, const WCHAR* frag);
			void ShutdownShader();
			void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename);

			bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix,
				ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor);

			void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

		private:
			ID3D11VertexShader * m_VertexShader;
			ID3D11PixelShader * m_FragmentShader;
			ID3D11InputLayout* m_Layout;
			ID3D11Buffer* m_MatrixBuffer;
			ID3D11SamplerState* m_SampleState;
			ID3D11Buffer* m_PixelBuffer;

		};
	}
}
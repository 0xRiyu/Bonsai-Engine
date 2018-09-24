#pragma once
#include "Texture.h"
#include "../bonsai.h"
namespace bonsai
{
	namespace graphics
	{


		class Img2D
		{

		private:
			struct VertexType
			{
				XMFLOAT3 position;
				XMFLOAT2 texture;
			};
		public:
			Img2D();
			~Img2D();

			bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, const char* textureFilename, int imgWidth, int imgHeight);
			void ShutDown();
			bool Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY);

			inline int GetIndexCount() const { return m_IndexCount; }
			inline ID3D11ShaderResourceView* GetTexture() const { return m_Texture->GetTexture(); }

		private:

			bool UpdateBuffers(ID3D11DeviceContext* deviceContext, int posX, int posY);
			void RenderBuffers(ID3D11DeviceContext* deviceContext);

			bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* textureFilename);
			void ReleaseTexture();

		private:
			ID3D11Buffer * m_VertexBuffer, * m_IndexBuffer;
			int m_VertexCount, m_IndexCount;
			Texture* m_Texture;

			int m_ScreenWidth, m_ScreenHeight;
			int m_ImgWidth, m_ImgHeight;
			int m_PreviousPosX, m_PreviousPosY;

		};
	}
}

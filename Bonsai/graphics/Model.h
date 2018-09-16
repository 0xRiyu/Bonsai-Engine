#pragma once
#include "../bonsai.h"
#include "Texture.h"
using namespace DirectX;
namespace bonsai {
	namespace graphics {
		class Model
		{
		private:
			struct VertexType
			{
				XMFLOAT3 position;
				XMFLOAT2 texture;
			};

		public:
			Model();
			Model(const Model& other);
			~Model();

			bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* textureFilePath);
			void Shutdown();
			void Render(ID3D11DeviceContext* deviceContext);

			inline int GetIndexCount() const { return m_IndexCount; }
			inline ID3D11ShaderResourceView* GetTexture() const{ return m_Texture->GetTexture(); }


		private:
			bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext,const char* filePath);
			void ReleaseTexture();
		private:
			ID3D11Buffer* m_VertexBuffer;
			ID3D11Buffer* m_IndexBuffer;
			int m_VertexCount, m_IndexCount;
			Texture* m_Texture;
		};
	}
}

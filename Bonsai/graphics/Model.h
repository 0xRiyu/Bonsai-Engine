#pragma once
#include "../bonsai.h"
#include "Texture.h"
using namespace DirectX;
using namespace std;
namespace bonsai {
	namespace graphics {
		class Model
		{
		private:
			struct VertexType
			{
				XMFLOAT3 position;
				XMFLOAT2 texture;
				XMFLOAT3 normal;
			};

			struct ModelType
			{
				float x, y, z;
				float tu, tv;
				float nx, ny, nz;
			};

		public:
			Model();
			Model(const Model& other);
			~Model();

			bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* textureFilePath, const char* modelFilePath );
			void Shutdown();
			void Render(ID3D11DeviceContext* deviceContext);

			inline int GetIndexCount() const { return m_IndexCount; }
			inline ID3D11ShaderResourceView* GetTexture() const{ return m_Texture->GetTexture(); }


		private:

			bool LoadModel(const char* modelFileName);
			void ReleaseModel();

			bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext,const char* filePath);
			void ReleaseTexture();
		private:
			ID3D11Buffer* m_VertexBuffer;
			ID3D11Buffer* m_IndexBuffer;
			int m_VertexCount, m_IndexCount;
			Texture* m_Texture;
			ModelType* m_Model;
		};
	}
}

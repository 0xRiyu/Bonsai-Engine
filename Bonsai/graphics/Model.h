#pragma once
#include "../bonsai.h"
using namespace DirectX;
namespace bonsai {
	namespace graphics {
		class Model
		{
		private:
			struct VertexType
			{
				XMFLOAT3 position;
				XMFLOAT4 color;
			};

		public:
			Model();
			Model(const Model& other);
			~Model();

			bool Initialize(ID3D11Device* device);
			void Shutdown();
			void Render(ID3D11DeviceContext* deviceContext);

			inline int GetIndexCount() const { return m_IndexCount; }

		private:
			ID3D11Buffer* m_VertexBuffer;
			ID3D11Buffer* m_IndexBuffer;
			int m_VertexCount, m_IndexCount;
		};
	}
}

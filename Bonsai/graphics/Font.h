#pragma once
#include "../bonsai.h"
#include "Texture.h"

using namespace std;
using namespace DirectX;
namespace bonsai
{
	namespace graphics
	{
		class Font
		{
		private:
			struct FontType
			{
				float left, right;
				int size;
			};
			struct VertexType
			{
				XMFLOAT3 position;
				XMFLOAT2 texture;
			};
		public:
			Font();
			Font(const Font& other);
			~Font();

			bool Initialize(ID3D11Device* device,ID3D11DeviceContext* deviceContext, const char* fontFilename, const char* textureFileName);
			void Shutdown();

			inline ID3D11ShaderResourceView* GetTexture() const { return m_Texture->GetTexture(); }
			void BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY);

		private:
			bool LoadFontData(const char* filename);
			void ReleaseFontData();
			bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* context, const char* textureFileName);
			void ReleaseTexture();
		private:
			FontType* m_Font;
			Texture* m_Texture;

		};

	}
}

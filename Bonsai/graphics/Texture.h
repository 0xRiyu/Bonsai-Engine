#pragma once
#include "../bonsai.h"
using namespace DirectX;
using namespace std;
namespace bonsai
{
	namespace graphics
	{
		class Texture
		{
		private:
			struct TargaHeader
			{
				UCHAR data1[12];
				USHORT width;
				USHORT height;
				UCHAR bpp;
				UCHAR data2;
			};
		public:
			Texture();
			Texture(const Texture& other);
			~Texture();

			bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename);
			void Shutdown();

			inline ID3D11ShaderResourceView* GetTexture() const { return m_TextureView; }

		private:
			bool LoadTarga(const char* filename, int& height, int& width);

		private:
			unsigned char* m_TargaData;
			ID3D11Texture2D* m_Texture;
			ID3D11ShaderResourceView* m_TextureView;
		};
	}
}

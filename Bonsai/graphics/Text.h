#pragma once
#include "../bonsai.h"
#include "Font.h"
#include "FontShader.h"
#include <unordered_map>

namespace bonsai
{
	namespace graphics
	{
		class Text
		{

		private:
			struct SentenceType
			{
				ID3D11Buffer *vertexBuffer, *indexBuffer;
				int vertexCount, indexCount, maxLength;
				float red, green, blue;
			};

			struct VertexType
			{
				XMFLOAT3 position;
				XMFLOAT2 texture;
			};

		public:
			Text();
			Text(const Text& other);
			~Text();

			bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int sWidth, int sHeight, XMMATRIX baseViewMatrix);
			void Shutdown();
			bool Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix);

			void PushBackText(String ID, const char* sentence, int posX, int posY, float red, float green, float blue);
			void UpdateText(String ID, const char* sentence, int posX, int posY, float red, float green, float blue);

		private:
			bool InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device);
			bool UpdateSentence(SentenceType* sentence, const char* text, int positionX, int positionY, float red, float green, float blue,
								ID3D11DeviceContext* deviceContext);

			void ReleaseSentence(SentenceType* sentence);
			bool RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix, XMMATRIX orthoMatrix);



		private:
			Font * m_Font;
			FontShader * m_FontShader;
			int m_ScreenWidth, m_ScreenHeight;
			XMMATRIX m_BaseViewMatrix;
			ID3D11Device* m_Device;
			ID3D11DeviceContext* m_DeviceContext;

			std::unordered_map<String, SentenceType*> sentenceMap;
		};

	}
}

#include "Text.h"
namespace bonsai
{
	namespace graphics
	{
		Text::Text()
			:m_Font(nullptr),m_FontShader(nullptr)
		{
		}

		Text::Text(const Text& other)
		{
		}

		Text::~Text()
		{
		}

		bool Text::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int sWidth, int sHeight,
			XMMATRIX baseViewMatrix)
		{
			bool result;
			m_ScreenWidth = sWidth;
			m_ScreenHeight = sHeight;
			m_DeviceContext = deviceContext;
			m_Device = device;

			m_BaseViewMatrix = baseViewMatrix;

			m_Font = new Font();
			if (!m_Font) return false;

			result = m_Font->Initialize(device, deviceContext, "resources/data/fontdata.txt", "resources/data/font.tga");
			if (!result) {
				MessageBox(hwnd, L"Could not init the font object in Text.cpp", L"Error", MB_OK);
				return false;
			}

			m_FontShader = new FontShader();
			result = m_FontShader->Initialize(device, hwnd);
			if (!result)
			{
				MessageBox(hwnd, L"Could not init the font shader object in Text.cpp", L"Error", MB_OK);
				return false;
			}

			return true;
		}

		void Text::Shutdown()
		{
			//TODO: Fix this later
			for (auto i = sentenceMap.begin(); i != sentenceMap.end(); ++i)
			{
				ReleaseSentence(i->second);
			}
			

			if (m_FontShader)
			{
				m_FontShader->Shutdown();
				delete m_FontShader;
				m_FontShader = nullptr;
			}

			if (m_Font)
			{
				m_Font->Shutdown();
				delete m_Font;
				m_Font = nullptr;
			}

		}

		bool Text::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,XMMATRIX viewMatrix, XMMATRIX orthoMatrix)
		{
			//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_BaseViewMatrix = viewMatrix;
			for (auto i = sentenceMap.begin(); i != sentenceMap.end(); ++i)
			{
				bool result = RenderSentence(deviceContext, i->second, worldMatrix, orthoMatrix);
				if (!result) return false;
			}
			return true;
			
		}

		void Text::PushBackText(String ID, const char* sentence, int posX, int posY, float red, float green, float blue)
		{
			sentenceMap[ID] = new SentenceType;

			InitializeSentence(&sentenceMap[ID], 50, m_Device);

			UpdateSentence(sentenceMap[ID], sentence, posX, posY, red, green, blue, m_DeviceContext);

		}
		
		void Text::UpdateText(String ID, const char* sentence, int posX, int posY, float red, float green, float blue)
		{
			UpdateSentence(sentenceMap[ID], sentence, posX, posY, red, green, blue, m_DeviceContext);
		}

		bool Text::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
		{
			VertexType* vertices;
			ULONG* indices;
			D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
			D3D11_SUBRESOURCE_DATA vertexData, indexData;
			HRESULT result;

			*sentence = new SentenceType;
			if (!*sentence) return false;

			(*sentence)->vertexBuffer = nullptr;
			(*sentence)->indexBuffer = nullptr;
			(*sentence)->maxLength = maxLength;

			const int cnts = 6 * maxLength;
			(*sentence)->vertexCount = cnts;
			(*sentence)->indexCount = cnts;

			vertices = new VertexType[cnts];
			if (!vertices) return false;

			indices = new ULONG[cnts];
			if (!indices) return false;

			memset(vertices,0, sizeof(VertexType)*cnts);

			for (int i = 0; i < cnts; i++)
			{
				indices[i] = i;
			}

			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.ByteWidth = sizeof(VertexType)*cnts;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			vertexData.pSysMem = vertices;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
			if (FAILED(result)) return false;

			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = sizeof(ULONG) * cnts;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;
			indexBufferDesc.StructureByteStride = 0;

			indexData.pSysMem = indices;
			indexData.SysMemPitch = 0;
			indexData.SysMemSlicePitch = 0;

			result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
			if (FAILED(result)) return false;

			delete[] vertices;
			vertices = nullptr;

			delete[] indices;
			indices = nullptr;

			return true;

		}

		bool Text::UpdateSentence(SentenceType* sentence, const char* text, int positionX, int positionY, float red, float green,
			float blue, ID3D11DeviceContext* deviceContext)
		{
			int numLetters;
			VertexType* vertices;
			float drawX, drawY;
			HRESULT result;
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			VertexType* verticesPtr;
			int vertexCount = sentence->vertexCount;


			sentence->red = red;
			sentence->blue = blue;
			sentence->green = green;

			numLetters = (int)strlen(text);
			if (numLetters > sentence->maxLength) return false;

			vertices = new VertexType[vertexCount];
			if (!vertices) return false;

			memset(vertices, 0, sizeof(VertexType) * vertexCount);

			// Calculate the X and Y pixel position on the screen to start drawing to.
			drawX = (float)(((m_ScreenWidth / 2) * -1) + positionX);
			drawY = (float)((m_ScreenHeight / 2) - positionY);


			m_Font->BuildVertexArray((void*)vertices, const_cast<char*>(text), drawX, drawY);

			// Lock the vertex buffer so it can be written to.
			result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(result))	return false;
			

			// Get a pointer to the data in the vertex buffer.
			verticesPtr = (VertexType*)mappedResource.pData;

			memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * vertexCount));

			deviceContext->Unmap(sentence->vertexBuffer, 0);

			delete[] vertices;
			vertices = nullptr;

			return true;

		}

		void Text::ReleaseSentence(SentenceType* sentence)
		{
			if (sentence)
			{
				if(sentence->vertexBuffer)
				{
					sentence->vertexBuffer->Release();
					sentence->vertexBuffer = nullptr;
				}

				if (sentence->indexBuffer)
				{
					sentence->indexBuffer->Release();
					sentence->indexBuffer = nullptr;
				}
				delete sentence;
				sentence = nullptr;
			}
		}

		bool Text::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix,
			XMMATRIX orthoMatrix)
		{
			UINT stride, offset;
			XMFLOAT4 pixelColor;
			bool result;

			stride = sizeof(VertexType);
			offset = 0;

			deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue,1.0f);

			result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_BaseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColor);
			if (!result) return false;

			return true;
		}
	}
}
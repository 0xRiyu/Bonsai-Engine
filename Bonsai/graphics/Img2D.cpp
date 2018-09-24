#include "Img2D.h"

namespace bonsai
{
	namespace graphics
	{
		Img2D::Img2D()
			: m_VertexBuffer(nullptr), m_IndexBuffer(nullptr), m_Texture(nullptr)
		{

		}

		Img2D::~Img2D()
		{
			
		}

		bool Img2D::Initialize(ID3D11Device* device,ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, const char* textureFilename, int imgWidth,
			int imgHeight)
		{
			m_ScreenWidth = screenWidth;
			m_ScreenHeight = screenHeight;
			m_ImgHeight = imgHeight;
			m_ImgWidth = imgWidth;

			m_PreviousPosX = -1;
			m_PreviousPosY = -1;


			VertexType* vertices;
			ULONG* indices;
			D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
			D3D11_SUBRESOURCE_DATA vertexData, indexData;
			HRESULT result;
			int i;

			//set to 6 since we are making a square from 2 triangles
			m_VertexCount = 6;
			m_IndexCount = 6;

			vertices = new VertexType[m_VertexCount];
			if (!vertices) return false;

			indices = new ULONG[m_IndexCount];
			if (!indices) return false;

			memset(vertices, 0, (sizeof(VertexType) * m_VertexCount));

			for(i = 0; i < m_IndexCount; i++)
			{
				indices[i] = i;
			}

			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_VertexCount;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			vertexData.pSysMem = vertices;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
			if (FAILED(result)) return false;

			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = sizeof(ULONG) * m_IndexCount;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;
			indexBufferDesc.StructureByteStride = 0;

			indexData.pSysMem = indices;
			indexData.SysMemPitch = 0;
			indexData.SysMemSlicePitch = 0;

			result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
			if (FAILED(result)) return false;

			delete[] vertices;
			vertices = nullptr;
			delete[] indices;
			indices = nullptr;


			//NOW WE LOAD TEXTURE

			bool result2(LoadTexture(device, deviceContext, textureFilename));
			if (!result2) return false;

			return true;


		}

		void Img2D::ShutDown()
		{
			// Release the index buffer.
			if (m_IndexBuffer)
			{
				m_IndexBuffer->Release();
				m_IndexBuffer = nullptr;
			}

			// Release the vertex buffer.
			if (m_VertexBuffer)
			{
				m_VertexBuffer->Release();
				m_VertexBuffer = nullptr;
			}
		}

		bool Img2D::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
		{
			bool result(UpdateBuffers(deviceContext, positionX, positionY));
			if (!result) return false;

			RenderBuffers(deviceContext);

			return true;
		}

		bool Img2D::UpdateBuffers(ID3D11DeviceContext* deviceContext, int posX, int posY)
		{
			float left, right, top, bottom;
			VertexType* vertices;
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			VertexType* verticesPtr;
			HRESULT result;

			//If the position we are rendering has not changed then don't update the vertex buffer
			if ((posX == m_PreviousPosX) && (posY == m_PreviousPosY)) return true;


			m_PreviousPosY = posY;
			m_PreviousPosX = posX;

			left = (float)((m_ScreenWidth / 2) * -1) + (float)posX;
			right = left + (float)m_ImgWidth;
			top = (float)(m_ScreenHeight / 2) - (float)posY;
			bottom = top - (float)m_ImgHeight;

			vertices = new VertexType[m_VertexCount];
			if (!vertices) return false;

			//Triangle 1
			vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
			vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

			vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
			vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

			vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
			vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

			// Second triangle.
			vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
			vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

			vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
			vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

			vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
			vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

			result = deviceContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(result)) return false;

			verticesPtr = (VertexType*)mappedResource.pData;
			memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_VertexCount));

			deviceContext->Unmap(m_VertexBuffer, 0);

			delete[] vertices;
			vertices = nullptr;

			return true;
		}

		void Img2D::RenderBuffers(ID3D11DeviceContext* deviceContext)
		{
			UINT stride(sizeof(VertexType));
			UINT offset(0);

			deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		bool Img2D::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* textureFilename)
		{
			m_Texture = new Texture();
			if (!m_Texture) return false;

			return m_Texture->Initialize(device, deviceContext, textureFilename);

		}

		void Img2D::ReleaseTexture()
		{
			if (m_Texture)
			{
				m_Texture->Shutdown();
				delete m_Texture;
				m_Texture = nullptr;
			}
		}
	}
}
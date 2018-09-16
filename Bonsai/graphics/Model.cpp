#include "Model.h"
 namespace bonsai
 {
	 namespace graphics
	 {
		 Model::Model()
		 {
			 m_IndexBuffer = nullptr;
			 m_VertexBuffer = nullptr;
		 }

		 Model::Model(const Model& other)
		 {
		 }

		 Model::~Model()
		 {
		 }

		 bool Model::Initialize(ID3D11Device* device)
		 {
			 VertexType* vertices;
			 ULONG* indices;
			 D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
			 D3D11_SUBRESOURCE_DATA vertexData, indexData;
			 HRESULT result;

			 m_VertexCount = 3;
			 m_IndexCount = 3;

			 vertices = new VertexType[m_VertexCount];
			 if (!vertices) return false;

			 indices = new ULONG[m_IndexCount];
			 if (!indices) return false;

			 //TRIANGLE
			 vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
			 vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

			 vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
			 vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

			 vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
			 vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

			 indices[0] = 0;
			 indices[1] = 1;
			 indices[2] = 2;

			 //Setup Desc of static vertex buffer
			 vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			 vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
			 vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			 vertexBufferDesc.CPUAccessFlags = 0;
			 vertexBufferDesc.MiscFlags = 0;
			 vertexBufferDesc.StructureByteStride = 0;

			 //Give the subresource structure a pointer to the vertex data
			 vertexData.pSysMem = vertices;
			 vertexData.SysMemPitch = 0;
			 vertexData.SysMemSlicePitch = 0;

			 //create the buffer
			 result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
			 if (FAILED(result)) return false;

			 //Setup the description of the static index buffer
			 indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			 indexBufferDesc.ByteWidth = sizeof(ULONG) * m_IndexCount;
			 indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			 indexBufferDesc.CPUAccessFlags = 0;
			 indexBufferDesc.MiscFlags = 0;
			 indexBufferDesc.StructureByteStride = 0;

			 //give subresource structure a pointer to index data
			 indexData.pSysMem = indices;
			 indexData.SysMemPitch = 0;
			 indexData.SysMemSlicePitch = 0;

			 //Create index buffer
			 result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);

			 if (FAILED(result)) return false;

			 delete[] vertices;
			 vertices = nullptr;

			 delete[] indices;
			 indices = nullptr;

			 return true;
		 }

		 void Model::Shutdown()
		 {
			 if (m_IndexBuffer)
			 {
				 m_IndexBuffer->Release();
				 m_IndexBuffer = nullptr;
			 }
			 if (m_VertexBuffer)
			 {
				 m_VertexBuffer->Release();
				 m_VertexBuffer = nullptr;
			 }
		 }

		 void Model::Render(ID3D11DeviceContext* deviceContext)
		 {
			 UINT stride(sizeof(VertexType));
		 	 UINT offset(0);

			 //Set the vertex buffer to active in the input assembler so it can be rendered
			 deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

			 //Set the index buffer to active
			 deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			 //set the type of primitive that should be rendered from the buffer - in this case triangles
			 deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		 }
	 }
 }
#include "Model.h"
 namespace bonsai
 {
	 namespace graphics
	 {
		 Model::Model()
			 :m_VertexBuffer(nullptr), m_IndexBuffer(nullptr), m_Texture(nullptr),m_Model(nullptr)
		 {

		 }

		 Model::Model(const Model& other)
		 {
		 }

		 Model::~Model()
		 {
		 }

		 bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* textureFilePath, const char* modelFilePath)
		 {
			 HRESULT result;
			 result = LoadModel(modelFilePath);
			 if (!result) return false;


			 VertexType* vertices;
			 ULONG* indices;
			 D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
			 D3D11_SUBRESOURCE_DATA vertexData, indexData;
			 int i;


			 vertices = new VertexType[m_VertexCount];
			 if (!vertices) return false;

			 indices = new ULONG[m_IndexCount];
			 if (!indices) return false;

			 for(i=0; i<m_VertexCount; i++)
			 {
				 vertices[i].position = XMFLOAT3(m_Model[i].x, m_Model[i].y, m_Model[i].z);
				 vertices[i].texture = XMFLOAT2(m_Model[i].tu, m_Model[i].tv);
				 vertices[i].normal = XMFLOAT3(m_Model[i].nx, m_Model[i].ny, m_Model[i].nz);

				 indices[i] = i;
			 }

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

			 result = LoadTexture(device, deviceContext, textureFilePath);
			 if (!result) return false;

			 

			 return true;
		 }

		 void Model::Shutdown()
		 {
			 ReleaseTexture();
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

			 ReleaseModel();
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
			 deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			 

		 }

		 bool Model::LoadModel(const char* modelFilePath)
		 {
			 ifstream fin;
			 char input;
			 int i;

			 fin.open(modelFilePath);

			 if (fin.fail()) return false;

			 fin.get(input);
			 while(input != ':')
			 {
				 fin.get(input);
			 }

			 fin >> m_VertexCount;

			 m_IndexCount = m_VertexCount;

			 m_Model = new ModelType[m_VertexCount];
			 if (!m_Model) return false;

			 fin.get(input);
			 while (input != ':')
			 {
				 fin.get(input);
			 }
			 fin.get(input);
			 fin.get(input);

			 //read in the vertex data
			 for(i = 0; i< m_VertexCount; i++)
			 {
				 fin >> m_Model[i].x >> m_Model[i].y >> m_Model[i].z;
				 fin >> m_Model[i].tu >> m_Model[i].tv;
				 fin >> m_Model[i].nx >> m_Model[i].ny >> m_Model[i].nz;
			 }
			 char temp[60];
			 sprintf(temp, "%d \n", m_VertexCount);
			 OutputDebugStringA(temp);

			 fin.close();

			 return true;
		 }

		 bool Model::LoadOBJModel(const String& modelFileName)
		 {
			 std::vector<String> lines = SplitString(ReadFile(modelFileName), '\n');
			 VertexSet inputVertices;
			 std::vector<IndexSet> indices;


			 for (String line: lines)
			 {
				 const char* cstr = line.c_str();
				 if (strstr(cstr, "#")) //comment;
				 {
					 continue;
				 }
				 else if (strstr(cstr, "v"))
				 {
					 if (strstr(cstr,"vt"))
					 {
						 XMFLOAT2 uv;
						 int result = sscanf(cstr, "%*s %f %f", &uv.x, &uv.y);
						 if (result == 0)
						 {
							 continue;
						 }
						 inputVertices.uvs.push_back(uv);
					 }
					 else if (strstr(cstr,"vn"))
					 {
						 XMFLOAT3 normal;
						 int result = sscanf(cstr, "%*s %f %f %f", &normal.x, &normal.y, &normal.z);
						 if (result == 0)
							 continue;
						 inputVertices.normals.push_back(normal);
					 }
					 else
					 {
						 XMFLOAT3 position;
						 int result = sscanf(cstr, "%*s %f %f %f", &position.x, &position.y, &position.z);
						 if (result == 0)
							 continue;
						 inputVertices.positions.push_back(position);
					 }
				 }
				 else if (strstr(cstr, "f"))
				 {
					IndexSet indexSet[3];
					int result = sscanf(cstr, "%*s %d/%d/%d %d/%d/%d %d/%d/%d", &indexSet[0].position, &indexSet[0].uv, &indexSet[0].normal, &indexSet[1].position, &indexSet[1].uv, &indexSet[1].normal, &indexSet[2].position, &indexSet[2].uv, &indexSet[2].normal);
					if (result == 0)
					{
						continue;
					}
					indices.push_back(indexSet[0]);
					indices.push_back(indexSet[1]);
				 	indices.push_back(indexSet[2]);

				 }
				 
				 
			 }
			 float tempsize = inputVertices.positions.size();
			 char temp[60];
			 sprintf(temp, "%f", tempsize);
			 OutputDebugStringA(temp);
			 m_VertexCount = inputVertices.positions.size();
			 m_IndexCount = m_VertexCount;

			 m_Model = new ModelType[m_VertexCount];
			 if (!m_Model) return false;
			 
			 for (int i = 0; i < inputVertices.positions.size(); i++)
			 {
				 m_Model[i].x = inputVertices.positions[i].x;
				 m_Model[i].y = inputVertices.positions[i].y;
				 m_Model[i].z = inputVertices.positions[i].z;
				 //Not same number of elements fix later
				 m_Model[i].tu = inputVertices.uvs[i].x;
				 m_Model[i].tv = inputVertices.uvs[i].y;
				 m_Model[i].nx = inputVertices.normals[i].x;
				 m_Model[i].ny = inputVertices.normals[i].y;
				 m_Model[i].nz = inputVertices.normals[i].z;


			 }
			 

			 return false;
		 }

		 void Model::ReleaseModel()
		 {
			 if (m_Model)
			 {
				 delete m_Model;
				 m_Model = nullptr;
			 }
		 }

		 bool Model::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filePath)
		 {

			 m_Texture = new Texture();
			 if (!m_Texture) return false;

			 return m_Texture->Initialize(device, deviceContext, filePath);
		 }

		 void Model::ReleaseTexture()
		 {
			 if(m_Texture)
			 {
				 m_Texture->Shutdown();
				 delete m_Texture;
				 m_Texture = nullptr;
			 }
		 }
	 }
 }
#include "Shader.h"
namespace bonsai
{
	namespace graphics
	{
		Shader::Shader()
			:m_VertexShader(nullptr), m_FragmentShader(nullptr), m_Layout(nullptr), m_MatrixBuffer(nullptr),m_SampleState(nullptr),m_LightBuffer(nullptr)
		{
		}

		Shader::Shader(const Shader& other)
		{
		}

		Shader::~Shader()
		{
		}

		bool Shader::Initialize(ID3D11Device* device, HWND hwnd)
		{
			return InitializeShader(device, hwnd, L"graphics/shaders/texture.vert", L"graphics/shaders/texture.frag");			
		}

		void Shader::Shutdown()
		{
			ShutdownShader();
		}

		bool Shader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
			XMMATRIX projMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffusecolor, XMFLOAT3 lightdirection)
		{
			bool result(SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projMatrix, texture, diffusecolor, lightdirection));
			if (!result) return false;

			RenderShader(deviceContext, indexCount);
			return true;
		}

		bool Shader::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vert, const WCHAR* frag)
		{
			HRESULT result;
			ID3D10Blob* errorMessage(nullptr);
			ID3D10Blob* vertexShaderBuffer(nullptr);
			ID3D10Blob* pixelShaderBuffer(nullptr);
			D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
			UINT numElements;
			D3D11_BUFFER_DESC matrixBufferDesc;
			D3D11_SAMPLER_DESC samplerDesc;
			D3D11_BUFFER_DESC lightBufferDesc;

			result = D3DCompileFromFile(vert, NULL, NULL, "TextureVertexShader", "vs_5_0",
				D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
			if (FAILED(result)) {
				if(errorMessage)
				{
					OutputShaderErrorMessage(errorMessage, hwnd, vert);
				} else
				{
					MessageBox(hwnd, vert, L"Missing Shader File", MB_OK);
				}
				return false;
			}
			result = D3DCompileFromFile(frag, NULL, NULL, "TexturePixelShader", "ps_5_0",
				D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
			if (FAILED(result))
			{
				if(errorMessage)
				{
					OutputShaderErrorMessage(errorMessage, hwnd, frag);
				} else
				{
					MessageBox(hwnd, frag, L"Missing Shader File", MB_OK);
				}
				return false;
			}
			//make shaders from buffer and store into m_files
			result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
			if (FAILED(result)) return false;

			result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_FragmentShader);
			if (FAILED(result)) return false;

			//Create vertex input layout description
			//This setup needs to match the vertextype structure in the model and in the shader
			polygonLayout[0].SemanticName = "POSITION";
			polygonLayout[0].SemanticIndex = 0;
			polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			polygonLayout[0].InputSlot = 0;
			polygonLayout[0].AlignedByteOffset = 0;
			polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			polygonLayout[0].InstanceDataStepRate = 0;

			polygonLayout[1].SemanticName = "TEXCOORD";
			polygonLayout[1].SemanticIndex = 0;
			polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
			polygonLayout[1].InputSlot = 0;
			polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			polygonLayout[1].InstanceDataStepRate = 0;

			polygonLayout[2].SemanticName = "NORMAL";
			polygonLayout[2].SemanticIndex = 0;
			polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			polygonLayout[2].InputSlot = 0;
			polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			polygonLayout[2].InstanceDataStepRate = 0;

			//get the count of the elements
			numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

			//create the vertex input layout
			result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
				vertexShaderBuffer->GetBufferSize(), &m_Layout);
			if (FAILED(result)) return false;

			//release the vertex shader and pixel shader buffers

			vertexShaderBuffer->Release();
			vertexShaderBuffer = nullptr;

			pixelShaderBuffer->Release();
			pixelShaderBuffer = nullptr;

			//Setup the description of the dynamic matrix constant buffer that is in the vertex shader
			matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
			matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			matrixBufferDesc.MiscFlags = 0;
			matrixBufferDesc.StructureByteStride = 0;

			result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer);
			if (FAILED(result)) return false;

			//Create a texture sampler state description
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samplerDesc.BorderColor[0] = 0;
			samplerDesc.BorderColor[1] = 0;
			samplerDesc.BorderColor[2] = 0;
			samplerDesc.BorderColor[3] = 0;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			result = device->CreateSamplerState(&samplerDesc, &m_SampleState);
			if (FAILED(result)) return false;

			//setup the desc of the light dynamic constant buffer that is in the pixel shader
			//note that bytewidth always needs to be a multiple of 16 if using d3d11_bind_constant_buffer or CreateBuffer will fail
			lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			lightBufferDesc.ByteWidth = sizeof(LightBufferType);
			lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			lightBufferDesc.MiscFlags = 0;
			lightBufferDesc.StructureByteStride = 0;

			//create the constant buffer pointer so we can access the vertex shader constant vuffer from within this class
			result = device->CreateBuffer(&lightBufferDesc, NULL, &m_LightBuffer);
			if (FAILED(result)) return false;

			return true;
		}

		void Shader::ShutdownShader()
		{
			if (m_VertexShader)
			{
				m_VertexShader->Release();
				m_VertexShader = nullptr;
			}
			if (m_FragmentShader)
			{
				m_FragmentShader->Release();
				m_FragmentShader = nullptr;
			}
			if (m_Layout)
			{
				m_Layout->Release();
				m_Layout = nullptr;
			}
			if(m_MatrixBuffer)
			{
				m_MatrixBuffer->Release();
				m_MatrixBuffer = nullptr;
			}
			if(m_SampleState)
			{
				m_SampleState->Release();
				m_SampleState = nullptr;
			}
			if (m_LightBuffer)
			{	
				m_LightBuffer->Release();
				m_LightBuffer = nullptr;
			}
		}

		void Shader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
		{
			char* compileErrors;
			ULONGLONG bufferSize, i;
			ofstream fout;

			//Get a pointer to the error message text buffer
			compileErrors = (char*)(errorMessage->GetBufferPointer());

			//get the length of the message
			bufferSize = errorMessage->GetBufferSize();

			//Open a file to write the message to
			fout.open("shader-error.txt");

			for (i = 0; i < bufferSize; i++) fout << compileErrors[i];

			fout.close();

			errorMessage->Release();
			errorMessage = nullptr;

			MessageBox(hwnd, L"Error Compiling Shader. Check shader-error.txt for message.", shaderFilename, MB_OK);
		}

		bool Shader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
			XMMATRIX projMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffusecolor, XMFLOAT3 lightdirection)
		{
			HRESULT result;
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			MatrixBufferType* dataPtr;
			LightBufferType* dataPtr2;
			UINT bufferNumber;

			worldMatrix = XMMatrixTranspose(worldMatrix);
			viewMatrix = XMMatrixTranspose(viewMatrix);
			projMatrix = XMMatrixTranspose(projMatrix);

			//Lock in the constant buffer so it can be written to
			result = deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(result)) return false;

			//get a pointer to the data in the constant buffer
			dataPtr = (MatrixBufferType*)mappedResource.pData;

			//copy the matrices into the buffer
			dataPtr->world = worldMatrix;
			dataPtr->view = viewMatrix;
			dataPtr->projection = projMatrix;

			//unlock
			deviceContext->Unmap(m_MatrixBuffer, 0);

			//set the position of the constant buffer in the vertex shader
			bufferNumber = 0;

			//finally set the constant buffer in the vertex shader
			deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);

			//set ps shader texture
			deviceContext->PSSetShaderResources(0, 1, &texture);


			//Lock the light constant buffer so it can be written to
			result = deviceContext->Map(m_LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(result)) return false;

			//Get a pointer to the data in the constant buffer
			dataPtr2 = (LightBufferType*)mappedResource.pData;

			//copy the lighting variables into the constant buffer
			dataPtr2->diffuseColor = diffusecolor;
			dataPtr2->lightDirection = lightdirection;
			dataPtr2->padding = 0.0f;

			//Unlock
			deviceContext->Unmap(m_LightBuffer, 0);

			//Set the position of the light const buffer in the pixel shader
			bufferNumber = 0;

			//Finally Set the light constant buffer in the pixel shader with the updated vals
			deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_LightBuffer);


			return true;
		}

		void Shader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
		{
			//Set the vertex input layout
			deviceContext->IASetInputLayout(m_Layout);

			//Set the vertex and pixel shaders that will be used to render this triangle
			deviceContext->VSSetShader(m_VertexShader, NULL, 0);
			deviceContext->PSSetShader(m_FragmentShader, NULL, 0);

			//Set the sample state in the PS
			deviceContext->PSSetSamplers(0, 1, &m_SampleState);

			//render triangle
			deviceContext->DrawIndexed(indexCount, 0, 0);
		}
	}
}
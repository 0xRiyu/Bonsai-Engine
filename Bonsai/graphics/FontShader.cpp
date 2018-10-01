#include "FontShader.h"
namespace bonsai
{
	namespace graphics
	{
		FontShader::FontShader()
			:m_VertexShader(nullptr), m_FragmentShader(nullptr), m_Layout(nullptr), m_MatrixBuffer(nullptr), m_SampleState(nullptr)
		{
		}

		FontShader::FontShader(const FontShader& other)
		{
		}

		FontShader::~FontShader()
		{
		}

		bool FontShader::Initialize(ID3D11Device* device, HWND hwnd)
		{
			return InitializeShader(device, hwnd, L"graphics/shaders/text.vert", L"graphics/shaders/text.frag");
		}

		void FontShader::Shutdown()
		{
			ShutdownShader();
		}

		bool FontShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
			XMMATRIX projMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)
		{
			bool result(SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projMatrix, texture, pixelColor));
			if (!result) return false;

			RenderShader(deviceContext, indexCount);
			return true;
		}

		bool FontShader::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vert, const WCHAR* frag)
		{
			HRESULT result;
			ID3D10Blob* errorMessage(nullptr);
			ID3D10Blob* vertexShaderBuffer(nullptr);
			ID3D10Blob* pixelShaderBuffer(nullptr);
			D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
			UINT numElements;
			D3D11_BUFFER_DESC matrixBufferDesc;
			D3D11_SAMPLER_DESC samplerDesc;
			D3D11_BUFFER_DESC pixelBufferDesc;

			result = D3DCompileFromFile(vert, NULL, NULL, "FontVertexShader", "vs_5_0",
				D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
			if (FAILED(result)) {
				if (errorMessage)
				{
					OutputShaderErrorMessage(errorMessage, hwnd, vert);
				}
				else
				{
					MessageBox(hwnd, vert, L"Missing Shader File", MB_OK);
				}
				return false;
			}
			result = D3DCompileFromFile(frag, NULL, NULL, "FontPixelShader", "ps_5_0",
				D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
			if (FAILED(result))
			{
				if (errorMessage)
				{
					OutputShaderErrorMessage(errorMessage, hwnd, frag);
				}
				else
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

			//Pixel color constant buffer
			pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
			pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			pixelBufferDesc.MiscFlags = 0;
			pixelBufferDesc.StructureByteStride = 0;

			result = device->CreateBuffer(&pixelBufferDesc, NULL, &m_PixelBuffer);
			if (FAILED(result)) return false;

			return true;
		}

		void FontShader::ShutdownShader()
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
			if (m_MatrixBuffer)
			{
				m_MatrixBuffer->Release();
				m_MatrixBuffer = nullptr;
			}
			if (m_SampleState)
			{
				m_SampleState->Release();
				m_SampleState = nullptr;
			}
			if(m_PixelBuffer)
			{
				m_PixelBuffer->Release();
				m_PixelBuffer = nullptr;
			}

		}

		void FontShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
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

			MessageBox(hwnd, L"Error Compiling FontShader. Check shader-error.txt for message.", shaderFilename, MB_OK);
		}

		bool FontShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
			XMMATRIX projMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)
		{
			HRESULT result;
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			MatrixBufferType* dataPtr;
			PixelBufferType* dataPtr2;
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

			//Set pixel color before rendering
			result = deviceContext->Map(m_PixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(result)) return false;

			dataPtr2 = (PixelBufferType*)mappedResource.pData;

			dataPtr2->pixelColor = pixelColor;

			deviceContext->Unmap(m_PixelBuffer, 0);

			bufferNumber = 0;

			deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_PixelBuffer);

			return true;
		}

		void FontShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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
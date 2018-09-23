#include "Texture.h"
 namespace bonsai
 {
	 namespace graphics
	 {
		 Texture::Texture()
			 :m_TargaData(nullptr), m_Texture(nullptr), m_TextureView(nullptr)
		 {
		 }

		 Texture::Texture(const Texture& other)
		 {
		 }

		 Texture::~Texture()
		 {
		 }

		 bool Texture::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename)
		 {
			 bool result;
			 int height, width;
			 D3D11_TEXTURE2D_DESC textureDesc;
			 HRESULT hResult;
			 UINT rowPitch;
			 D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

			 //load the targa image data into memory
			 //height and width are passbacks
			 result = LoadTarga(filename, height, width);
			 if (!result) return false;


			 textureDesc.Height = height;
			 textureDesc.Width = width;
			 textureDesc.MipLevels = 0;
			 textureDesc.ArraySize = 1;
			 textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			 textureDesc.SampleDesc.Count = 1;
			 textureDesc.SampleDesc.Quality = 0;
			 textureDesc.Usage = D3D11_USAGE_DEFAULT;
			 textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			 textureDesc.CPUAccessFlags = 0;
			 textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

			 //create empty texture
			 hResult = device->CreateTexture2D(&textureDesc, NULL, &m_Texture);
			 if (FAILED(hResult)) return false;

			 rowPitch = (width * 4) * sizeof(UCHAR);

			 //copy the targa image data into the texture - subresource used for loading once, vs map for many updates
			 deviceContext->UpdateSubresource(m_Texture, 0, NULL, m_TargaData, rowPitch, 0);

			 //Setup the shader resource view description
			 srvDesc.Format = textureDesc.Format;
			 srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			 srvDesc.Texture2D.MostDetailedMip = 0;
			 srvDesc.Texture2D.MipLevels = -1;

			 //Create the shader resource view for texture
			 hResult = device->CreateShaderResourceView(m_Texture, &srvDesc, &m_TextureView);
			 if (FAILED(hResult)) return false;

			 //Generate mipmaps
			 deviceContext->GenerateMips(m_TextureView);

			 //release targa data
			 delete[] m_TargaData;
			 m_TargaData = nullptr;

			 return true;
		 }

		 void Texture::Shutdown()
		 {
			 if (m_Texture)
			 {
				 m_Texture->Release();
				 m_Texture = nullptr;
			 }
			 if(m_TextureView)
			 {
				 m_TextureView->Release();
				 m_TextureView = nullptr;
			 }
			 if(m_TargaData)
			 {
				 delete[] m_TargaData;
				 m_TargaData = nullptr;
			 }
		 }

		 bool Texture::LoadTarga(const char* filename, int& height, int& width)
		 {
			 int error, bpp, imageSize, index, i, j, k;
			 FILE* filePtr;
			 UINT count;
			 TargaHeader targaFileHeader;
			 UCHAR* targaImage;

			 //open the targaq file for reading in binary
			 error = fopen_s(&filePtr, filename, "rb");
			 if (error != 0) {
				 OutputDebugString(L"Could Not Open Texture File in Texture.cpp\n");
				 return false;
			 }
			 //Read in the file header
			 count = (UINT)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
			 if (count != 1) return false;

			 //Get the important info from header
			 height = (int)targaFileHeader.height;
			 width = (int)targaFileHeader.width;
			 bpp = (int)targaFileHeader.bpp;

			 //Check that its 32 bit and not 24 bit
			 if (bpp != 32) {
				 OutputDebugString(L"Texture File is not 32 Bits - Texture.cpp\n");
				 return false;
			 }

			 //Calculate the size of the 32 bit image data
			 imageSize = width * height * 4;

			 //Alocate memory for the targa image data
			 targaImage = new UCHAR[imageSize];
			 if (!targaImage) return false;

			 //Read in targa image data
			 count = (UINT)fread(targaImage, 1, imageSize, filePtr);
			 if (count != imageSize) return false;

			 //close the file
			 error = fclose(filePtr);
			 if (error != 0) return false;

			 //allocate memory for the new targa destination data
			 m_TargaData = new UCHAR[imageSize];
			 if (!m_TargaData) return false;

			 //init the index into targa dest data array
			 index = 0;

			 //init the index into the targa image data
			 k = 0;

			 //now copy the targa image data into the dest aray in the correcy order since the targa format is stored upside down
			 for(j = 0; j < height; j++)
			 {
				 for (i = 0; i < width; i++) {
					 m_TargaData[index + 0] = targaImage[k + 2];	//red
					 m_TargaData[index + 1] = targaImage[k + 1];	//green
					 m_TargaData[index + 2] = targaImage[k + 0];	//blue
					 m_TargaData[index + 3] = targaImage[k + 3];	//alpha

					 k += 4;
					 index += 4;
				 }

				 //Set the targa image data index back to the preceding row at the beginning of the collumn since its reading it in upside down
				 //k += (width * 4);
			 }

			 //release the targa image data now that it was copied into the array
			 delete[] targaImage;
			 targaImage = nullptr;

			 return true;
		 }
	 }
 }
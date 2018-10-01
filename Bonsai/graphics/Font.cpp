#include "Font.h"
namespace bonsai
{
	namespace graphics
	{
		Font::Font()
			:m_Font(nullptr), m_Texture(nullptr)
		{
		}

		Font::Font(const Font& other)
		{
		}

		Font::~Font()
		{
		}

		bool Font::Initialize(ID3D11Device* device,ID3D11DeviceContext* deviceContext, const char* fontFilename, const char* textureFileName)
		{
			bool result;

			//Load in the textfile containing font data
			result = LoadFontData(fontFilename);
			if (!result) return false;

			//Load in texture that  has font characters in it
			result = LoadTexture(device, deviceContext, textureFileName);
			if (!result) return false;

			return true;
		}

		void Font::Shutdown()
		{
			ReleaseFontData();
			ReleaseTexture();
		}

		void Font::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
		{
			VertexType* vertexPtr;

			vertexPtr = (VertexType*)vertices;

			int numLetters = (int)strlen(sentence);

			int index = 0;

			for (int i = 0; i < numLetters; i++)
			{
				int letter = (int)sentence[i] - 32;

				if (letter == 0)
				{
					drawX = drawX + 3.0f;
				} else
				{
					//Triangle 1
					//Top left
					vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);
					vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
					index++;

					//bottom right
					vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, (drawY - 16), 0.0f);
					vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
					index++;
					//bottom left
					vertexPtr[index].position = XMFLOAT3(drawX, (drawY - 16), 0.0f);
					vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
					index++;

					//Triangle 2
					//top left
					vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);
					vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
					index++;

					//top right
					vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);
					vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
					index++;

					//bottom right
					vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, (drawY - 16), 0.0f);
					vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
					index++;

					//Update X location by font size shift
					drawX += m_Font[letter].size + 1.0f;
				}
			}
		}

		bool Font::LoadFontData(const char* filename)
		{
			ifstream fin;
			char temp;

			const auto bufferSize = 95;
			//Create font spacing buffer
			m_Font = new FontType[bufferSize];
			if (!m_Font) return false;

			fin.open(filename);
			if (fin.fail()) return false;

			//Read each line into font array
			for (auto i = 0; i < bufferSize; i++)
			{
				fin.get(temp);
				while (temp != ' ')
				{
					fin.get(temp);
				}
				fin.get(temp);
				while (temp != ' ')
				{
					fin.get(temp);
				}
				fin >> m_Font[i].left;
				fin >> m_Font[i].right;
				fin >> m_Font[i].size;
			}
			fin.close();

			return true;
		}

		void Font::ReleaseFontData()
		{
			if (m_Font)
			{
				delete[] m_Font;
				m_Font = nullptr;
			}
		}

		bool Font::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* context, const char* textureFileName)
		{
			bool result;

			m_Texture = new Texture();
			if (!m_Texture) return false;

			result = m_Texture->Initialize(device, context, textureFileName);
			if (!result) return false;

			return true;

		}

		void Font::ReleaseTexture()
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
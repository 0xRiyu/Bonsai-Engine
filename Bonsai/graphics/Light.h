#pragma once
#include "../bonsai.h"
using namespace DirectX;

namespace bonsai
{
	namespace graphics {

		class Light
		{
		public:
			Light();
			Light(const Light& other);
			~Light();

			inline void SetAmbientColor(float r, float g, float b, float a) { m_AmbientColor = XMFLOAT4(r, g, b, a); }
			inline void SetDiffuseColor(float r, float g, float b, float a) { m_DiffuseColor = XMFLOAT4(r, g, b, a); }
			inline void SetDirection(float x, float y, float z) { m_Direction = XMFLOAT3(x, y, z); }

			inline XMFLOAT4 GetAmbientColor() const { return m_AmbientColor; }
			inline XMFLOAT4 GetDiffuseColor() const { return m_DiffuseColor; }
			inline XMFLOAT3 GetDirection() const { return m_Direction; }


		private:
			XMFLOAT4 m_AmbientColor;
			XMFLOAT4 m_DiffuseColor;
			XMFLOAT3 m_Direction;
		};
	}
}
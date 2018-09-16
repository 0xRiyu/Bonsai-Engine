#pragma once
#include "../bonsai.h"
using namespace DirectX;

namespace bonsai {
	namespace graphics {

		class Camera
		{
		public:
			Camera();
			Camera(const Camera& other);
			~Camera();

			inline void SetPosition(XMFLOAT3 in) { m_PositionX = in.x; m_PositionY = in.y, m_PositionZ = in.z; }
			inline void SetRotation(XMFLOAT3 in) { m_RotationYaw = in.x; m_RotationPitch = in.y; m_RotationRoll = in.z; }

			inline void SetPosition(float x, float y, float z) { m_PositionX = x; m_PositionY = y, m_PositionZ; }
			inline void SetRotation(float yaw, float pitch, float roll) { m_RotationYaw = yaw; m_RotationPitch = pitch; m_RotationRoll = roll; }

			inline XMFLOAT3 GetPosition() const { return XMFLOAT3(m_PositionX, m_PositionY, m_PositionZ); }
			inline XMFLOAT3 GetRotation() const { return XMFLOAT3(m_RotationYaw, m_RotationPitch, m_RotationRoll); }

			inline void GetViewMatrix(XMMATRIX& mat) { mat = m_ViewMatrix; }
			inline XMMATRIX GetViewMatrix() const { return m_ViewMatrix; }
			void Update();
			
		private:
			float m_PositionX, m_PositionY, m_PositionZ;
			float m_RotationYaw, m_RotationPitch, m_RotationRoll;
			XMMATRIX m_ViewMatrix;
		};
	}
}

#pragma once
#include "../bonsai.h"
#include "Frustum.h"
using namespace DirectX;

namespace bonsai {
	namespace graphics {
		const float SCREEN_DEPTH(1000.0f);
		const float SCREEN_NEAR(0.1f);

		class Camera
		{
		public:
			Camera(float screenWidth, float screenHeight);
			Camera(const Camera& other);
			~Camera();

			inline void SetPosition(XMFLOAT3 in) { m_PositionX = in.x; m_PositionY = in.y, m_PositionZ = in.z; }
			inline void SetRotation(XMFLOAT3 in) { m_RotationYaw = in.x; m_RotationPitch = in.y; m_RotationRoll = in.z; }

			inline void SetPosition(float x, float y, float z) { m_PositionX = x; m_PositionY = y, m_PositionZ = z; }
			inline void SetRotation(float yaw, float pitch, float roll) { m_RotationYaw = yaw; m_RotationPitch = pitch; m_RotationRoll = roll; }

			inline XMFLOAT3 GetPosition() const { return XMFLOAT3(m_PositionX, m_PositionY, m_PositionZ); }
			inline XMFLOAT3 GetRotation() const { return XMFLOAT3(m_RotationYaw, m_RotationPitch, m_RotationRoll); }

			inline XMVECTOR GetLookAtVector() const { return m_LookAt; }
			inline XMVECTOR GetUpVector() const { return m_Up; }
			inline XMVECTOR GetPositionVector() const { return m_Position; }
			inline XMVECTOR GetRotationVectorRads() const { return m_RotationVector; }

			inline void GetViewMatrix(XMMATRIX& mat) const { mat = m_ViewMatrix; }
			inline void GetProjectionMatrix(XMMATRIX& mat) const { mat = m_ProjectionMatrix; }
			inline void GetOrthoMatrix(XMMATRIX& orth) { orth = m_OrthoMatrix; }

			inline XMMATRIX GetViewMatrix() const { return m_ViewMatrix; }
			inline XMMATRIX GetProjectionMatrix() const { return m_ProjectionMatrix; }
			inline XMMATRIX GetOthroMatrix() const { return m_OrthoMatrix; }


			void Update();
			
		private:
			float m_PositionX, m_PositionY, m_PositionZ;
			float m_RotationYaw, m_RotationPitch, m_RotationRoll;
			XMMATRIX m_ViewMatrix;
			XMMATRIX m_ProjectionMatrix;
			XMMATRIX m_OrthoMatrix;
			float m_FOV;
			float m_Aspect;

			XMVECTOR m_Position, m_LookAt, m_Up, m_RotationVector;

		public:
			Frustum * m_Frustum;
		};
	}
}

#include "Camera.h"
namespace bonsai
{
	namespace graphics
	{
		Camera::Camera()
			:m_PositionX(0), m_PositionY(0), m_PositionZ(0), m_RotationYaw(0), m_RotationPitch(0), m_RotationRoll(0)
		{
		}

		Camera::Camera(const Camera& other)
		{
			SetPosition(other.GetPosition());
			SetRotation(other.GetRotation());
			Update();
		}

		Camera::~Camera()
		{
		}

		void Camera::Update()
		{
			XMFLOAT3 up, position, lookAt;
			XMVECTOR upVector, positionVector, lookAtVector;
			float yaw, pitch, roll;
			XMMATRIX rotationMatrix;

			up.x = 0.0f;
			up.y = 1.0f;
			up.z = 0.0f;

			upVector = XMLoadFloat3(&up);

			position = GetPosition();

			positionVector = XMLoadFloat3(&position);

			lookAt.x = 0.0f;
			lookAt.y = 0.0f;
			lookAt.z = 1.0f;

			lookAtVector = XMLoadFloat3(&lookAt);

			//Convert to rads
			pitch = m_RotationPitch * 0.0174532925f;
			yaw = m_RotationYaw * 0.0174532925f;
			roll = m_RotationRoll * 0.0174532925f;

			rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

			//transform so they are rotated correctly about origin
			lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
			upVector = XMVector3TransformCoord(upVector, rotationMatrix);

			//translate the rotation camera pos to the loc of viewer
			lookAtVector = XMVectorAdd(positionVector, lookAtVector);

			m_ViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
		}
	}
}
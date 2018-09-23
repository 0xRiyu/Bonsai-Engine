#include "Camera.h"
namespace bonsai
{
	namespace graphics
	{
		Camera::Camera()
			:m_PositionX(0.0f), m_PositionY(0.0f), m_PositionZ(-5.0f), m_RotationYaw(0.0f), m_RotationPitch(0.0f), m_RotationRoll(0.0f)
		{
			Update();
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
			XMFLOAT3 up, position, lookat;
			XMVECTOR upVector, positionVector, lookAtVector;
			float yaw, pitch, roll;
			XMMATRIX rotationMatrix;


			// Setup the vector that points upwards.
			up.x = 0.0f;
			up.y = 1.0f;
			up.z = 0.0f;

			// Load it into a XMVECTOR structure.
			upVector = XMLoadFloat3(&up);

			// Setup the position of the camera in the world.
			position.x = m_PositionX;
			position.y = m_PositionY;
			position.z = m_PositionZ;

			// Load it into a XMVECTOR structure.
			positionVector = XMLoadFloat3(&position);

			// Setup where the camera is looking by default.
			lookat.x = 0.0f;
			lookat.y = 0.0f;
			lookat.z = 1.0f;

			// Load it into a XMVECTOR structure.
			lookAtVector = XMLoadFloat3(&lookat);

			// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
			pitch = m_RotationPitch * 0.0174532925f;
			yaw = m_RotationYaw * 0.0174532925f;
			roll = m_RotationRoll * 0.0174532925f;

			// Create the rotation matrix from the yaw, pitch, and roll values.
			rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

			// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
			lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
			upVector = XMVector3TransformCoord(upVector, rotationMatrix);

			// Translate the rotated camera position to the location of the viewer.
			//lookAtVector = XMVectorAdd(positionVector, lookAtVector);

			m_Position = positionVector;
			m_LookAt = lookAtVector;
			m_Up = upVector;

			// Finally create the view matrix from the three updated vectors.
			m_ViewMatrix = XMMatrixLookAtLH(positionVector, positionVector + lookAtVector, upVector);

		}
	}
}
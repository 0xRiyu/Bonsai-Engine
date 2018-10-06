#include "InputHandler.h"
namespace bonsai {
	InputHandler::InputHandler(HWND hwnd)
	{
		m_RawInput = new RawInput(hwnd);
	}

	InputHandler::InputHandler(const InputHandler&)
	{

	}

	InputHandler::~InputHandler()
	{
	}

	void InputHandler::Initialize()
	{
		for (int i = 0; i < 256; i++)
		{
			m_Keys[i] = false;
		}
	}
	void InputHandler::KeyDown(unsigned int input)
	{
		m_Keys[input] = true;
	}

	void InputHandler::KeyUp(unsigned int input)
	{
		m_Keys[input] = false;
	}

	bool InputHandler::IsKeyDown(unsigned int key)
	{
		return m_Keys[key];
	}

	void InputHandler::ParseMouseInput(bonsai::graphics::Camera* camera, LPARAM lparam)
	{
		m_RawInput->RetrieveData(lparam);

		
		float xoffset = m_RawInput->raw.data.mouse.lLastX *m_MouseSensitivity;
		float yoffset = m_RawInput->raw.data.mouse.lLastY *m_MouseSensitivity;

		if (yoffset > 0.5 || yoffset < -0.5) yoffset = 0.00;


		float cameraYaw = camera->GetRotation().x + xoffset;
		float cameraPitch = camera->GetRotation().y + yoffset;

		camera->SetRotation(cameraYaw, cameraPitch, 0);
		camera->Update();
	}

	void InputHandler::ParseKeyboardInput(bonsai::graphics::Camera* camera, float deltaTime)
	{
		XMVECTOR posVec = camera->GetPositionVector();
		XMVECTOR upVec = camera->GetUpVector();
		XMVECTOR lookatVec = camera->GetLookAtVector();


		if (IsKeyDown(0x41)) //A
		{
			posVec += (XMVector3Normalize(XMVector3Cross(lookatVec, upVec)) * deltaTime * 0.01);
		}

		if (IsKeyDown(0x44)) //D
		{
			posVec -= (XMVector3Normalize(XMVector3Cross(lookatVec, upVec))* deltaTime * 0.01);
		}

		if (IsKeyDown(0x57)) //W
		{
			posVec += XMVector3Normalize(lookatVec) * deltaTime * 0.01;
		}

		if (IsKeyDown(0x53)) //S
		{
			posVec -= XMVector3Normalize(lookatVec) * deltaTime * 0.01;
		}

		if (IsKeyDown(0x51)) //Q
		{
			posVec = XMVectorSet(XMVectorGetX(posVec), XMVectorGetY(posVec) - (0.01) * deltaTime, XMVectorGetZ(posVec), XMVectorGetW(posVec));
		}

		if (IsKeyDown(0x45)) //E
		{
			posVec = XMVectorSet(XMVectorGetX(posVec), XMVectorGetY(posVec) + (0.01) * deltaTime, XMVectorGetZ(posVec), XMVectorGetW(posVec));
		}

		XMFLOAT3 posVecUpdate;
		XMStoreFloat3(&posVecUpdate, posVec);

		camera->SetPosition(posVecUpdate);
		camera->Update();
	}
}


#include "InputHandler.h"

InputHandler::InputHandler()
{
}

InputHandler::InputHandler(const InputHandler&)
{
}

InputHandler::~InputHandler()
{
}

void InputHandler::Initialize()
{
	for (int i=0; i < 256; i++)
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



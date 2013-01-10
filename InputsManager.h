#pragma once
#include "Config.h"
#include "IInputListener.h"
#include "Vector.h"

class InputsManager
{
public:
	InputsManager();
	~InputsManager();

	static void AddInputListener(IInputListener* listener);
	static void NotifyListeners(unsigned int input_index);

private:
	static Vector<IInputListener*> m_listeners;
};

#pragma once
#include "Config.h"
#include "IInputListener.h"
#include "Vector.h"

class InputsManager
{
public:
	static InputsManager *GetInputsManager();	
	~InputsManager();

	void AddInputListener(IInputListener* listener);
	static void NotifyListeners(unsigned int input_index);

private:
	InputsManager(); // Singelton
	static Vector<IInputListener*> m_listeners;
};

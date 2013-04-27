#pragma once
#include "Config.h"
#include "IInputListener.h"
#include "CVector.h"

class InputsManager
{
public:
	static InputsManager *GetInputsManager();	
	~InputsManager();

	void AddInputListener(IInputListener* listener);
	void RemoveInputListener(IInputListener* listener);
	static void NotifyListeners(unsigned int input_index);

private:
	InputsManager(); // Singelton

	static CVector<IInputListener*> m_listeners;
	static unsigned int m_disabled_buttons;
};

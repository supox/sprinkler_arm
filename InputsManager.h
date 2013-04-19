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

	/*
	static void init_clock();
	static bool clock_enabled();
	static void enable_clock();
	*/

	static Vector<IInputListener*> m_listeners;
	static unsigned int m_disabled_buttons;
};

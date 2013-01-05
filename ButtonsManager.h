#pragma once
#include "Config.h"
#include "IButtonListener.h"
#include "Vector.h"

class ButtonsManager
{
public:
	ButtonsManager();
	~ButtonsManager();

	bool ButtonPressed(const unsigned int ButtonIndex);

	static void AddButtonListener(IButtonListener* listener);
	static void NotifyListeners(unsigned int button_index);

private:
	bool LastButtonState[NUMBER_OF_BUTTONS];
	static Vector<IButtonListener*> m_listeners;
};

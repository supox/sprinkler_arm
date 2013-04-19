#pragma once

class IInputListener
{
public:
	virtual ~IInputListener(){}
	virtual void OnInputChanged(unsigned int InputIndex, const bool InputPressed) = 0;
};

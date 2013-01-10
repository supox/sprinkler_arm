#pragma once

class IInputListener
{
public:
	virtual ~IInputListener(){}
	virtual void OnInputChanged(unsigned int InputIndex) = 0;
};

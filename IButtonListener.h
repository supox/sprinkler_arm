#pragma once

class IButtonListener
{
public:
	virtual ~IButtonListener(){}
	virtual void OnButtonPressed(unsigned int ButtonIndex) = 0;
};

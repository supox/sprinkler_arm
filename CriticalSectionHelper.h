#pragma once

class CriticalSectionHelper 
{
public:
	CriticalSectionHelper ()
	{
		__disable_irq();
	}
	~CriticalSectionHelper ()
	{
		__enable_irq();
	}
};

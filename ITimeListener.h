#pragma once

class ITimeListener
{
public:
	virtual ~ITimeListener(){};
	virtual void TimeNotification(unsigned int time) = 0;
};

#pragma once

class ITimeListener
{
public:
	virtual ~ITimeListener(){};
	virtual void TimeNotification() = 0;
};

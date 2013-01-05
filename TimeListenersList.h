#pragma once
#include "ITimeListener.h"
#include <stddef.h>

class TimeListenerData
{
public:
	TimeListenerData(const int _Time, ITimeListener* _listener) 
		: Time(_Time), listener(_listener), Next(NULL)
	{
	}
	
	int Time;
	ITimeListener* listener;
	TimeListenerData* Next;
};

class TimeListenersList
{
public:
	TimeListenersList();
	~TimeListenersList();
	
	void Add(int Time, ITimeListener* listener);
	bool Remove(int Time, ITimeListener* listener);
	void Clear();

	TimeListenerData* GetFirst();
	void RemoveFirst();
	
private:
	TimeListenerData* root;

};

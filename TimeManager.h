#pragma once
#include "ITimeListener.h"
#include "TimeListenersList.h"

class TimeManager
{
public:
	static TimeManager* GetTimeManager();
	~TimeManager();
	
	static void SetSystemTime(const int CurrentTime);
	static int GetSystemTime();

	static void NotifyAt(ITimeListener* listener, const int Time);
	static void NotifyListeners();

private:
	TimeManager(); // Singelton
	static TimeManager* Instance;

	static int m_CurrentTime;
	static TimeListenersList m_Listeners;

};

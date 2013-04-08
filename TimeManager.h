#pragma once
#include "ITimeListener.h"
#include "TimeListenersList.h"

class TimeManager
{
public:
	static TimeManager* GetTimeManager();
	~TimeManager();
	
	static void SetSystemTime(const unsigned int CurrentTime);
	static unsigned int GetSystemTime();

	static void NotifyAt(ITimeListener* listener, const unsigned int Time);
	static void RemoveNotifications(ITimeListener* listener);

	static void DelayMs(size_t ms);

private:
	TimeManager(); // Singelton
	static void UpdateNextAlarmTime();
	static void NotifyListeners();

	static TimeManager* Instance;

	static TimeListenersList m_Listeners;

};

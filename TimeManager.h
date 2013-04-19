#pragma once
#include "ITimeListener.h"
#include "TimeListenersList.h"
#include "ITask.h"

class TimeManager : private ITask
{
public:
	static TimeManager* GetTimeManager();
	virtual ~TimeManager();
	
	static void SetSystemTime(const unsigned int CurrentTime);
	static unsigned int GetSystemTime();

	static void NotifyAt(ITimeListener* listener, const unsigned int Time);
	static void RemoveNotifications(ITimeListener* listener);

	static void DelayMs(size_t ms);

private:
	TimeManager(); // Singelton
	static void UpdateNextAlarmTime();
	static void RTCHandler();
	virtual void DoTask();

	static TimeManager* Instance;
	static TimeListenersList m_Listeners;

	#ifdef _TESTS
	public:
	#endif
	static void NotifyListeners();

};

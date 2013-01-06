#include "TimeManager.h"
#include "STM32F10x.h"

/*----------------------------------------------------------------------------
  Systick Interrupt Handler
  SysTick interrupt happens every 100 ms
 *----------------------------------------------------------------------------*/
extern "C"
{
	void SysTick_Handler (void) {
		static unsigned char ticks = 0;
		ticks++;
		if(ticks == 10) {
			ticks=0;
			TimeManager::NotifyListeners();
		}
	}
}

// Static memebers
int TimeManager::m_CurrentTime = 0;
TimeListenersList TimeManager::m_Listeners;

// Singelton
TimeManager* TimeManager::Instance = new TimeManager();
TimeManager* TimeManager::GetTimeManager()
{
	return TimeManager::Instance;
}

TimeManager::TimeManager()
{
	if(SysTick_Config(SystemCoreClock/10) != 0) // Generate interrupt each 100 ms 
	{
		// CONFIG ERROR
		// printf("ERROR...");
		while(1);
	}
}

TimeManager::~TimeManager()
{
}
	
void TimeManager::SetSystemTime(const int CurrentTime)
{
	m_CurrentTime = CurrentTime;
}

int TimeManager::GetSystemTime()
{
	return m_CurrentTime;
}

void TimeManager::NotifyAt(ITimeListener* listener, const int Time)
{
	m_Listeners.Add(Time, listener);
}

void TimeManager::RemoveNotifications(ITimeListener* listener)
{
	m_Listeners.Remove(listener);
}

void TimeManager::NotifyListeners()
{
	m_CurrentTime++;

	while(true)
	{
		TimeListenerData* nextListener = m_Listeners.GetFirst();
		if(nextListener != NULL && nextListener->Time <= m_CurrentTime)
		{
			// Remove from list first, to avoid cases where the listener add itself again.
			ITimeListener* listener = nextListener->listener;
			m_Listeners.RemoveFirst();
			
			// Notify listener
			if(listener != NULL)
				listener->TimeNotification();
		}
		else
			break;
	}
}

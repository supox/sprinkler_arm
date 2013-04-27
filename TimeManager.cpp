#include "TimeManager.h"
#include "STM32F10x.h"
#include "rtc_handler.h"
#include "Scheduler.h"

static volatile unsigned int s_current_tick_counter = 0;
extern "C"
{
	void SysTick_Handler (void) {
		s_current_tick_counter++;
		if(s_current_tick_counter == 1000) {
			s_current_tick_counter=0;
		}
	}
}

// Static memebers
TimeListenersList TimeManager::m_Listeners;

// Singelton
TimeManager* TimeManager::Instance = new TimeManager();
TimeManager* TimeManager::GetTimeManager()
{
	return TimeManager::Instance;
}

TimeManager::TimeManager()
{	
	rtc_init();	
	rtc_set_on_alarm_handler(TimeManager::RTCHandler);
}

TimeManager::~TimeManager()
{
}
	
void TimeManager::SetSystemTime(const unsigned int CurrentTime)
{
	rtc_set_time(CurrentTime);
}

unsigned int TimeManager::GetSystemTime()
{
	return rtc_get_time();
}

void TimeManager::UpdateNextAlarmTime()
{
		TimeListenerData* nextListener = m_Listeners.GetFirst();
		if(nextListener != NULL)
		{
			if(nextListener->Time <= GetSystemTime())
				RTCHandler();
			else
				rtc_set_alarm(nextListener->Time);
		}
}

void TimeManager::NotifyAt(ITimeListener* listener, const unsigned int Time)
{
	m_Listeners.Add(Time, listener);
	UpdateNextAlarmTime();
}

void TimeManager::RemoveNotifications(ITimeListener* listener)
{
	m_Listeners.Remove(listener);
	UpdateNextAlarmTime();
}

void TimeManager::DoTask()
{
	NotifyListeners();
}

void TimeManager::RTCHandler()
{
	// Add task to scheduler
	Scheduler::AddTask(GetTimeManager());
}

void TimeManager::NotifyListeners()
{
	unsigned int current_time = GetSystemTime();

	TimeListenerData* nextListener = m_Listeners.GetFirst();
	for( ; nextListener != NULL && nextListener->Time <= current_time ; nextListener = m_Listeners.GetFirst() )
	{
		// Remove from list first, to avoid cases where the listener add itself again.
		ITimeListener* listener = nextListener->listener;
		m_Listeners.RemoveFirst();
		
		// Notify listener
		if(listener != NULL)
			listener->TimeNotification(current_time);
	}
	
	UpdateNextAlarmTime();
}

static inline void my_SysTick_Config(uint32_t ticks)
{
  SysTick->LOAD  = ticks - 1;                                  /* set reload register */
  NVIC_SetPriority (SysTick_IRQn, 6);  /* set Priority for Systick Interrupt */
  SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
}

void TimeManager::DelayMs(size_t ms)
{
	my_SysTick_Config(SystemCoreClock/1000);
	while(ms)
	{
		volatile size_t last_tick_counter = s_current_tick_counter;
		while(last_tick_counter == s_current_tick_counter);
		ms--;
	}		
	
	// Shut down clock:
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	
}

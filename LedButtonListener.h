#pragma once

#include "IInputListener.h"
#include "LedsManager.h"
#include "ITimeListener.h"
#include "TimeManager.h"

class LedButtonListener : public IInputListener, public ITimeListener
{
public:
	LedButtonListener(LedsManager* manager, unsigned int ButtonIndex, unsigned int LedIndex) :
		m_manager(manager), m_ButtonIndex(ButtonIndex), m_LedIndex(LedIndex), m_bRunning(false)
	{
		m_manager->SetLed(m_LedIndex, false);
	}
	
	virtual ~LedButtonListener(){}
	virtual void OnInputChanged(unsigned int ButtonIndex)
	{
		if(m_bRunning)
			return;
		
		if(ButtonIndex != m_ButtonIndex)
			return;
		
		// Turn led on for the next 3 seconds.
		m_manager->SetLed(m_LedIndex, true);
		TimeManager *timeManager = TimeManager::GetTimeManager();
		timeManager->NotifyAt(this, timeManager->GetSystemTime() + 3);
		m_bRunning = true;
	}
	
	virtual void TimeNotification(unsigned int time)
	{
		m_manager->SetLed(m_LedIndex, false);
		m_bRunning = false;
	}
	
	
private:
	LedsManager* m_manager;
	unsigned int m_ButtonIndex;
	unsigned int m_LedIndex;
	bool m_bRunning;
};

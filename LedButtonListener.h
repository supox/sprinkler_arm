#pragma once

#include "IButtonListener.h"
#include "LedsManager.h"
#include "ITimeListener.h"
#include "TimeManager.h"

class LedButtonListener : public IButtonListener, public ITimeListener
{
public:
	LedButtonListener(LedsManager* manager, unsigned int ButtonIndex) :
		m_manager(manager), m_ButtonIndex(ButtonIndex), m_bRunning(false)
	{
	}
	
	virtual ~LedButtonListener(){}
	virtual void OnButtonPressed(unsigned int ButtonIndex)
	{
		if(m_bRunning)
			return;
		
		if(ButtonIndex != m_ButtonIndex)
			return;
		
		// Turn led on for the next 3 seconds.
		m_manager->SetLed(m_ButtonIndex, true);
		TimeManager *timeManager = TimeManager::GetTimeManager();
		timeManager->NotifyAt(this, timeManager->GetSystemTime() + 3);
		m_bRunning = true;
	}
	
	virtual void TimeNotification()
	{
		m_manager->SetLed(m_ButtonIndex, false);
		m_bRunning = false;
	}
	
	
private:
	LedsManager* m_manager;
	unsigned int m_ButtonIndex;
	bool m_bRunning;
};

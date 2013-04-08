#ifdef _TEST_RTC

#include "rtc_handler.h"
#include "LedsManager.h"
#include "TestHelpers.h"
#include "TimeManager.h"

using namespace TestHelpers;


class MockTimeListener : public ITimeListener
{
public:
	virtual ~MockTimeListener(){}
	virtual void TimeNotification(unsigned int time)
	{
		// Blink:
		LedsManager::GetLedsManager()->ToggleLed(1);

		TimeManager::NotifyAt(this, time+1);
	}
};

void TestRTC()
{	
	rtc_init();
	rtc_set_time(1000);
	unsigned int time = rtc_get_time();
	assert(time >= 1000 && time <= 1003);
	rtc_set_time(31000);
	time = rtc_get_time();
	assert(time >= 31000 && time <= 31003);
	
	// Tick every second...
	MockTimeListener listener;
	TimeManager::NotifyAt(&listener, rtc_get_time()-1);
	while(true);
}

#endif

#include "STM32F10x.h"
#include "LedsManager.h"
#include "InputsManager.h"
#include "LedButtonListener.h"
#include "Sprinkler.h"
#include "Logger.h"
#include "Scheduler.h"

#include "Tests.h"

// Use alternative heap implementaion, for apps with many memory allocations.
#pragma import(__use_realtime_heap)

void DoTests()
{
	#ifdef _TESTS
		// TestHTTPParser();
		// TestIrrigationList();
		// TestValvesManager();
		// TestSensors();
		// CheckUART();
		// TestGsmStatusParser();
		// TestJsonTime();
		// TestScheduler();
		// TestJsonSensors();
		TestMemory();
	#endif
	
	#ifdef _TEST_GSM_MODEM
		TestGsmModem();
	#endif
	
	#ifdef _TEST_RTC
		TestRTC();
	#endif
	
	#ifdef _TESTS_STORAGE
		TestStorage();
	#endif
}

LedsManager *ledsManager = LedsManager::GetLedsManager();
LedButtonListener listener1(ledsManager,0,0);
LedButtonListener listener2(ledsManager,1,1);
LedButtonListener listener3(ledsManager,2,0);

static void InitLeds(void)
{
	InputsManager::GetInputsManager()->AddInputListener(&listener1);
	InputsManager::GetInputsManager()->AddInputListener(&listener2);
	InputsManager::GetInputsManager()->AddInputListener(&listener3);
}


int main(void)
{
	// Disable Watchdog during debug.
	DBGMCU->CR = DBGMCU_CR_DBG_TIM2_STOP | DBGMCU_CR_DBG_IWDG_STOP | DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_STOP | DBGMCU_CR_DBG_STANDBY;

	Logger::AddLine("Starting.", Logger::DUMP);

	DoTests();
	InitLeds();

	Sprinkler s;

	//forever do...
	while(1)
	{
		__wfi();
		Scheduler::DoTasks();
	}
}

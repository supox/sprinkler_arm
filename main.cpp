#include "STM32F10x.h"
#include "LedsManager.h"
#include "InputsManager.h"
#include "LedButtonListener.h"
#include "Sprinkler.h"
#include "Logger.h"

// #pragma import(__use_no_semihosting)

#include "Tests.h"

void DoTests()
{
	#ifdef _TESTS
		// TestIrrigationList();
		// TestValvesManager();
		TestSensors();
	#endif
}

int main(void)
{
	// Disable Watchdog during debug.
	DBGMCU->CR = DBGMCU_CR_DBG_TIM2_STOP | DBGMCU_CR_DBG_IWDG_STOP | DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_STOP | DBGMCU_CR_DBG_STANDBY;

	Logger::AddLine("Starting.", Logger::DUMP);

	// DoTests();
	
	Sprinkler s;
	LedsManager ledsManager;
	InputsManager inputsManager;
	LedButtonListener listener1(&ledsManager,0);
	LedButtonListener listener2(&ledsManager,1);
	inputsManager.AddInputListener(&listener1);
	inputsManager.AddInputListener(&listener2);
		
	//forever do...
	while(1)
	{
		__wfi();
	}
}

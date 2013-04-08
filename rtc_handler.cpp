#include "rtc_handler.h"
#include "stm32f10x_rtc.h"
#include "LedsManager.h"

#define RTC_IRQChannel 0x3

static rtc_on_alarm_func s_alarm_handler = NULL;
void rtc_set_on_alarm_handler(rtc_on_alarm_func handler)
{
	s_alarm_handler = handler;
}

void rtc_init(void)
{
	volatile uint16_t i;

	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* LSI clock stabilization time */
	for(i=0;i<5000;i++) { ; }

	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) {
		/* Backup data register value is not correct or not yet programmed (when
			 the first time the program is executed) */

		/* Allow access to BKP Domain */
		PWR_BackupAccessCmd(ENABLE);

		/* Reset Backup Domain */
		BKP_DeInit();

		/* Enable LSE */
		RCC_LSEConfig(RCC_LSE_ON);

		/* Wait till LSE is ready */

		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) { ; }

		/* Select LSE as RTC Clock Source */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

		/* Enable RTC Clock */
		RCC_RTCCLKCmd(ENABLE);

		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();

		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();

		/* Set RTC prescaler: set RTC period to 1sec */
		RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();

		/* Set initial value */
		RTC_SetCounter( (uint32_t)((11*60+55)*60) ); // here: 1st January 2000 11:55:00

		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();

		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);

		/* Lock access to BKP Domain */
		PWR_BackupAccessCmd(DISABLE);
	} else {
		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();
	}
	
	// Enable RTC interrupts:
	NVIC->ISER[0] |= (1 << (RTC_IRQChannel & 0x1F));
}

/*
	Set RTC time.
	ticks - current time in seconds.
*/
void rtc_set_time(unsigned int ticks)
{
	PWR_BackupAccessCmd(ENABLE);
  RTC_WaitForLastTask();
	RTC_SetCounter(ticks);
  RTC_WaitForLastTask();
	PWR_BackupAccessCmd(DISABLE);
}

/*
	Set RTC alarm time.
	ticks - current time in seconds.
*/
void rtc_set_alarm(unsigned int ticks)
{
	PWR_BackupAccessCmd(ENABLE);
	RTC_WaitForLastTask();
	RTC_SetAlarm(ticks);
	RTC_WaitForLastTask();
	PWR_BackupAccessCmd(DISABLE);
	
	RTC_ITConfig(RTC_IT_ALR, ENABLE);
}

/*
	Get RTC alarm time.
	return value in seconds.
*/
unsigned int rtc_get_time(void)
{
	uint32_t ticks;
	while ( ( ticks = RTC_GetCounter() ) != RTC_GetCounter() ) ;
	return ticks;
}


/*------------------------------------------------------------------------------
  RTC Interrupt Handler
 *------------------------------------------------------------------------------*/
extern "C"
{	
	void RTC_IRQHandler(void)
	{
		if (RTC_GetITStatus(RTC_IT_SEC) == SET) {                       // check second flag
			RTC_ClearITPendingBit(RTC_IT_SEC);
			// Handle seconds interrupt here.
		}
		if (RTC_GetITStatus(RTC_IT_OW) == SET) {                       // check second flag
			RTC_ClearITPendingBit(RTC_IT_OW);
			// Handle overflow interrupt here.
		}

		if (RTC_GetITStatus(RTC_IT_ALR) == SET) {                       // check alarm flag
			RTC_ClearITPendingBit(RTC_IT_ALR); // clear alarm flag
			// Handle alarm interrupt here.
			if(s_alarm_handler != NULL)
				s_alarm_handler();
		}
	} // end RTC_IRQHandler
} // extern "C"

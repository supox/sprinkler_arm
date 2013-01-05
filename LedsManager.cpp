#include "LedsManager.h"
#include "STM32F10x.h"

LedsManager::LedsManager() : m_iNumberOfLeds(2)
{
	// Turn on IO Port B
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

	//set PB08-PB09 ports as output
	GPIOB->CRH		&= ~0x000000FF;               /* PB.8..9 clear                 */
	GPIOB->CRH		|=  0x00000033;               /* PB.8..9 Output                */
}

LedsManager::~LedsManager()
{
}

void LedsManager::SetLed(const size_t led_index, const bool led_on)
{
	if(led_index < m_iNumberOfLeds) {
		int mask = GetMask(led_index);

		if(led_on)
			GPIOB->ODR &= ~mask;
		else
			GPIOB->ODR |= mask;
	}
}

void LedsManager::ToggleLed(const size_t led_index)
{
	if(led_index < m_iNumberOfLeds) {
		int mask = GetMask(led_index);
		GPIOB->ODR ^= mask;
	}
}

bool LedsManager::GetLed(const size_t led_index)
{
	if(led_index < m_iNumberOfLeds) {
		int mask = GetMask(led_index);
		return ( GPIOB->ODR & mask ) != 0;
	}
	return false;
}

int LedsManager::GetMask(const size_t led_index)
{
	switch(led_index)
	{
		case 0: default:
			return 0x00000200;
		case 1:
			return 0x00000100;
	}
}

#include "ButtonsManager.h"
#include "STM32F10x.h"

ButtonsManager::ButtonsManager()
{
	// Turn on IO Port C
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;        /* Enable GPIOC clock            */

  GPIOC->CRL   &= ~0x000000FF;               /* PC.0..1 clear                 */
  GPIOC->CRL   |=  0x00000088;               /* PC.0..1 is Floating Input     */
	GPIOC->ODR	 |=	 0x03;							 			 /* Configure as a pull down 			*/	
	
	
	// Enable interrupts:
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function
	AFIO->EXTICR[0] = 0x0022;                              	// Set events to port C

	EXTI->IMR = 0x03;																				// Unmask all interrupts.
	EXTI->RTSR = 0x0;																				// Disable rising edge
	EXTI->FTSR = 0x03; 																			// set falling edge

	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);	
}

ButtonsManager::~ButtonsManager()
{
}

bool ButtonsManager::ButtonPressed(const unsigned int ButtonIndex)
{	
	int mask;
	switch(ButtonIndex) // todo - optimize with static array
	{
		case 0: default:
			mask = 0x2;
			break;
		case 1:
			mask = 0x1;
			break;
	}
	
	uint8_t currentButtonState = (GPIOC->IDR & mask);
	uint8_t returnValue = (!currentButtonState && LastButtonState[ButtonIndex]);
	LastButtonState[ButtonIndex] = currentButtonState;
	
	return returnValue;
}

Vector<IButtonListener*> ButtonsManager::m_listeners;
void ButtonsManager::AddButtonListener(IButtonListener* listener)
{
	m_listeners.Add(listener);
}

void ButtonsManager::NotifyListeners(unsigned int button_index)
{
	const unsigned int listeners_size = m_listeners.size();
	for(unsigned int index = 0 ; index < listeners_size ; ++index ) {
		if(m_listeners[index] != NULL)
			m_listeners[index]->OnButtonPressed(button_index);
	}
}


extern "C"
{
	/*----------------------------------------------------------*\
	 | EXTI0 Interrupt Handler                                  |
	\*----------------------------------------------------------*/
	void EXTI0_IRQHandler(void)
	{
		if(EXTI->PR & (1 << 0)) {
			// EXTI0 interrupt pending?
			EXTI->PR |= (1 << 0);
			
			ButtonsManager::NotifyListeners(0);
		}
	}

	/*----------------------------------------------------------*\
	 | EXTI1 Interrupt Handler                             |
	\*----------------------------------------------------------*/
	void EXTI1_IRQHandler(void)
	{
		if(EXTI->PR & (1 << 1)) {
			// EXTI0 interrupt pending?
			EXTI->PR |= (1 << 1);
			
			ButtonsManager::NotifyListeners(1);
		}
	}
}

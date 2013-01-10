#include "InputsManager.h"
#include "STM32F10x.h"

InputsManager::InputsManager()
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

InputsManager::~InputsManager()
{
}

Vector<IInputListener*> InputsManager::m_listeners;
void InputsManager::AddInputListener(IInputListener* listener)
{
	m_listeners.Add(listener);
}

void InputsManager::NotifyListeners(unsigned int input_index)
{
	const unsigned int listeners_size = m_listeners.size();
	for(unsigned int index = 0 ; index < listeners_size ; ++index ) {
		if(m_listeners[index] != NULL)
			m_listeners[index]->OnInputChanged(input_index);
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
			
			InputsManager::NotifyListeners(0);
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
			
			InputsManager::NotifyListeners(1);
		}
	}
}

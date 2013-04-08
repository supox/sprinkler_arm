#include "InputsManager.h"
#include "STM32F10x.h"


InputsManager* InputsManager::GetInputsManager()
{
	static InputsManager* s_instance = new InputsManager();
	return s_instance;
}

InputsManager::InputsManager()
{
	// Turn on IO Port C
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;        /* Enable GPIOC clock            */

  GPIOC->CRL   &= ~0x00000FFF;               /* PC.0..2 clear                 */
  GPIOC->CRL   |=  0x00000888;               /* PC.0..2 is Floating Input     */
	GPIOC->ODR	 |=	 0x07;							 			 /* Configure as a pull down 			*/	
	
	
	// Enable interrupts:
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function
	AFIO->EXTICR[0] = 0x0222;                              	// Set events to port C

	EXTI->IMR = 0x07;																				// Unmask all interrupts.
	EXTI->RTSR = 0x0;																				// Disable rising edge
	EXTI->FTSR = 0x07; 																			// set falling edge

	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI2_IRQn);
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
	#define EXT_HADNLER(n) \
	void EXTI ## n ## _IRQHandler(void) { \
		if(EXTI->PR & (1 << n)) { \
			EXTI->PR |= (1 << n); \
			InputsManager::NotifyListeners(n); \
		} \
	}

	EXT_HADNLER(0)
	EXT_HADNLER(1)
	EXT_HADNLER(2)	
}

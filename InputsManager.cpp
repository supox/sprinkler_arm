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
	
	// init_clock();

}

InputsManager::~InputsManager()
{
}

CVector<IInputListener*> InputsManager::m_listeners;
unsigned int InputsManager::m_disabled_buttons = 0;

void InputsManager::AddInputListener(IInputListener* listener)
{
	m_listeners.Add(listener);
}

void InputsManager::RemoveInputListener(IInputListener* listener)
{
	m_listeners.Remove(listener);
}

void InputsManager::NotifyListeners(unsigned int input_index)
{
	const unsigned int mask = (1 << input_index);
	const bool input_value = (GPIOC->IDR & mask) != 0;

	const unsigned int listeners_size = m_listeners.size();
	for(unsigned int index = 0 ; index < listeners_size ; ++index ) {
		if(m_listeners[index] != NULL)
			m_listeners[index]->OnInputChanged(input_index, input_value);
	}
}

/*
bool InputsManager::clock_enabled()
{
	return TIM1->CNT > 0;
}

void InputsManager::enable_clock()
{
	TIM1->CR1 |= 0x0001;                              // enable timer
}

void InputsManager::init_clock()
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;                     // enable clock for TIM1
	TIM1->PSC = 0x1C1F;                               // set prescaler
	TIM1->ARR = 0x270F;                               // set auto-reload
	TIM1->RCR = 0;

	TIM1->CCR1  = 0x0;
	TIM1->CCR2  = 0x0;
	TIM1->CCR3  = 0x0;
	TIM1->CCR4  = 0x0;
	TIM1->CCMR1 = 0x0061;
	TIM1->CCMR2 = 0x0068;
	TIM1->CCER  = 0x0;
	TIM1->SMCR  = 0x0;
	TIM1->CR1 = 0x001C;
	TIM1->CR2 = 0x0000;
}
*/

extern "C"
{
	#define EXT_HADNLER(n) \
	void EXTI ## n ## _IRQHandler(void) { \
		const unsigned int mask = (1 << n); \
		if(EXTI->PR & mask) { \
			EXTI->PR |= mask; \
			InputsManager::NotifyListeners(n); \
		} \
	}

	EXT_HADNLER(0)
	EXT_HADNLER(1)
	EXT_HADNLER(2)	
}

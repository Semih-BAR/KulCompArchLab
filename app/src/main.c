#include <stdint.h>
#include <stm32l4xx.h>

void delay(unsigned int n) {					// een functie om te vertragen/wachten met een gekozen waarde als lengte.  delay(s) = n / (frequentie MCU)
    volatile unsigned int delay = n;
    while (delay--);
}

int main(void) {
	// AHB2 peripheral clock enable register
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;		// enable IO port B clock -- clock activeren voor GPIO B
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;		// enable IO port C clock -- clock activeren voor GPIO C


    //LED 1
    GPIOB->MODER &= ~GPIO_MODER_MODE9_Msk;		// port mode register mask van GPIOB pin 9 laag zetten

    GPIOB->MODER |= GPIO_MODER_MODE9_0;			// port mode register van GPIOB pin 9 op 01 zetten -> general purpose output mode
    GPIOB->MODER &= ~GPIO_MODER_MODE9_1;

    GPIOB->OTYPER &= ~GPIO_OTYPER_OT9;			// output type register van GPIOB pin 9 laag zetten -> output push-pull (reset state)


    //LED 2
    GPIOC->MODER &= ~GPIO_MODER_MODE13_Msk;		// analoog aan LED1

    GPIOC->MODER |= GPIO_MODER_MODE13_0;
    GPIOB->MODER &= ~GPIO_MODER_MODE13_1;

    GPIOC->OTYPER &= ~GPIO_OTYPER_OT13;


    //Knop A
    GPIOB->MODER &= ~GPIO_MODER_MODE13_Msk;		// port mode register mask van GPIOB pin 13 laag zetten

    GPIOB->MODER &= ~GPIO_MODER_MODE13_0;		// port mode register van GPIOB pin 13 op 00 zetten -> input mode
    GPIOB->MODER &= ~GPIO_MODER_MODE13_1;

    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;		// port pull-up/pull-down register mask van GPIOB pin 13 laag zetten

	GPIOB->PUPDR |= GPIO_PUPDR_PUPD13_0;		// port pull-up/pull-down register van GPIOB pin 13 op 01 zetten -> pull-up
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD13_1;


    //Knop B
    GPIOB->MODER &= ~GPIO_MODER_MODE14_Msk;		// analoog aan knop A

    GPIOB->MODER &= ~GPIO_MODER_MODE14_0;
	GPIOB->MODER &= ~GPIO_MODER_MODE14_1;

    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14_Msk;

	GPIOB->PUPDR |= GPIO_PUPDR_PUPD14_0;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14_1;


    while (1) {
    	GPIOB->ODR &= ~GPIO_ODR_OD9;			// port output data register van GPIOC laag zetten -> LED 1 uitschakelen
    	GPIOC->ODR &= ~GPIO_ODR_OD13;			// LED 2 uitschakelen

        if (!(GPIOB->IDR & GPIO_IDR_ID13) & !(GPIOB->IDR & GPIO_IDR_ID14)) {
        	GPIOB->ODR &= ~GPIO_ODR_OD9;
        	GPIOC->ODR &= ~GPIO_ODR_OD13;		// beide LEDs uitschakelen als beide knoppen tegelijk worden ingedrukt
        }

        else if (!(GPIOB->IDR & GPIO_IDR_ID14)) {
        	GPIOB->ODR |= GPIO_ODR_OD9;
			GPIOC->ODR |= GPIO_ODR_OD13;
			delay(750000);
			GPIOB->ODR &= ~GPIO_ODR_OD9;
			GPIOC->ODR &= ~GPIO_ODR_OD13;
			delay(750000);						// LEDs traag knipperen als knop B wordt ingedrukt
		}

        else if (!(GPIOB->IDR & GPIO_IDR_ID13)) {
        	GPIOB->ODR |= GPIO_ODR_OD9;
            GPIOC->ODR |= GPIO_ODR_OD13;
            delay(2500000);
            GPIOB->ODR &= ~GPIO_ODR_OD9;
            GPIOC->ODR &= ~GPIO_ODR_OD13;
            delay(2500000);
            }									// LEDs snel knipperen als knop A wordt ingedrukt
    }
}

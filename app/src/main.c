#include <stdint.h>
#include <stm32l4xx.h>

int mux = 0;
int tick = 0;
int uren = 0;
int minuten = 0;
int toggle = 0;
int stop = 1;

void delay(unsigned int n){
	volatile unsigned int delay = n;
	while (delay | toggle){
    	if (!(GPIOB->IDR & GPIO_IDR_ID14)) {
			toggle++;
			break;
		}
		if (tick){
			delay--;
			tick = 0;
		}
	}
}

void delay2(unsigned int n){
	volatile unsigned int delay = n;
	while (delay){
		if (tick){
			delay--;
			tick = 0;
		}
	}
}

void clear(){
	GPIOA->ODR &= ~(GPIO_ODR_OD7 | GPIO_ODR_OD5);
	GPIOB->ODR &= ~(GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
}

void seg7(int n){
	switch (n){
		case 0:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1);
			break;
		case 1:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			break;
		case 2:
			GPIOA->ODR |= (GPIO_ODR_OD7);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD2 | GPIO_ODR_OD15 | GPIO_ODR_OD12);
			break;
		case 3:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD2 | GPIO_ODR_OD12);
			break;
		case 4:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD1 | GPIO_ODR_OD2);
			break;
		case 5:
			GPIOA->ODR |= (GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD1 | GPIO_ODR_OD2 | GPIO_ODR_OD12);
			break;
		case 6:
			GPIOA->ODR |= (GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD1 | GPIO_ODR_OD15 | GPIO_ODR_OD12 | GPIO_ODR_OD2);
			break;
		case 7:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0);
			break;
		case 8:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
			break;
		case 9:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
			break;
	}
}

void SysTick_Handler(void){
	tick++;
	switch(mux & 0x3){
	case 0:
		clear();
		GPIOA->ODR &= ~(GPIO_ODR_OD8);
		GPIOA->ODR &= ~(GPIO_ODR_OD15);		// 00
		seg7(uren / 10);
		break;

	case 1:
		clear();
		GPIOA->ODR |= (GPIO_ODR_OD8);
		GPIOA->ODR &= ~(GPIO_ODR_OD15);		// 10
		seg7(uren % 10);
		break;

	case 2:
		clear();
		GPIOA->ODR &= ~(GPIO_ODR_OD8);
		GPIOA->ODR |= (GPIO_ODR_OD15);		// 01
		seg7(minuten / 10);
		break;

	case 3:
		clear();
		GPIOA->ODR |= (GPIO_ODR_OD8);
		GPIOA->ODR |= (GPIO_ODR_OD15);		// 11
		seg7(minuten % 10);
		break;
	}
	mux++;
}

int main(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;		// enable IO port C clock -- clock activeren voor GPIO C

    //7seg leds
	GPIOA->MODER &= ~GPIO_MODER_MODE7_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE7_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT7;

	GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE5_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;

	GPIOB->MODER &= ~GPIO_MODER_MODE0_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE0_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT0;

	GPIOB->MODER &= ~GPIO_MODER_MODE12_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE12_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT12;

	GPIOB->MODER &= ~GPIO_MODER_MODE15_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE15_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT15;

	GPIOB->MODER &= ~GPIO_MODER_MODE1_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE1_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT1;

	GPIOB->MODER &= ~GPIO_MODER_MODE2_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE2_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT2;

	//mux
	GPIOA->MODER &= ~GPIO_MODER_MODE8_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE8_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT8;

	GPIOA->MODER &= ~GPIO_MODER_MODE15_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE15_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT15;

	GPIOA->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE6_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT6;

	// CPU Frequentie = 48 MHz
	// Systick interrupt elke 1 ms (1kHz)  --> 48000000 Hz / 1000 Hz --> Reload = 48000
	SysTick_Config(48000);

	// Interrupt aanzetten met een prioriteit van 128
	NVIC_SetPriority(SysTick_IRQn, 128);
	NVIC_EnableIRQ(SysTick_IRQn);

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

    while (1) {
    	while (toggle == 1){
    		while (stop == 1){
    			minuten--;
				GPIOC->ODR |= GPIO_ODR_OD13;
				delay2(250);
				stop = 0;
    		}
    		if (!(GPIOB->IDR & GPIO_IDR_ID14)) {
    			toggle++;
    			GPIOC->ODR &= ~GPIO_ODR_OD13;
    			stop = 1;
        	}
    		else {
				if (!(GPIOB->IDR & GPIO_IDR_ID13)){
					delay2(250);
					uren++;
					if (uren >= 24){
						clear();
						uren = 0;
					}
				}
    		}
    	}
    	while (toggle == 2){
    		while (stop == 1){
        		GPIOB->ODR |= GPIO_ODR_OD9;
    			delay2(250);
				stop = 0;
    		}
			if (!(GPIOB->IDR & GPIO_IDR_ID14)) {
				toggle = 0;
				GPIOB->ODR &= ~GPIO_ODR_OD9;
				stop = 1;
				delay2(250);
			}
			else{
				if (!(GPIOB->IDR & GPIO_IDR_ID13)){
					delay2(250);
					minuten++;
					if (minuten >= 60){
						clear();
						minuten = 0;
						delay2(500);
					}
				}
			}
		}

    	if (minuten >= 60){
			clear();
			minuten = 0;
			uren ++;
		}
		if (uren >= 24){
			clear();
			uren = 0;
		}

    	delay(60000);
    	minuten++;
    }
}


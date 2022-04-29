#include <stdint.h>
#include <stm32l4xx.h>
#include <math.h>

int mux = 0;
int tick = 0;
float temperatuur = 0.0f;
float value;
float V;
float R;

void delay(unsigned int n){
	volatile unsigned int delay = n;
	while (delay != 0){
		if (tick){
			delay--;
			tick = 0;
		}
	}
}

int readNTC(){
	ADC1->SQR1 &= ~(ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_1 | ADC_SQR1_SQ1_2 | ADC_SQR1_SQ1_3);
	ADC1->SQR1 |= (ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_2);

	// Start de ADC en wacht tot de sequentie klaar is
	ADC1->CR |= ADC_CR_ADSTART;
	while(!(ADC1->ISR & ADC_ISR_EOC));

	return ADC1->DR;
}

int readPOT(){
	ADC1->SQR1 &= ~(ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_1 | ADC_SQR1_SQ1_2 | ADC_SQR1_SQ1_3);
	ADC1->SQR1 |= (ADC_SQR1_SQ1_1 | ADC_SQR1_SQ1_2);

	// Start de ADC en wacht tot de sequentie klaar is
	ADC1->CR |= ADC_CR_ADSTART;
	while(!(ADC1->ISR & ADC_ISR_EOC));

	return ADC1->DR;
}

void SysTick_Handler(void){
	tick++;
	/*switch(mux & 0x3){
	case 0:
		clear();
		GPIOA->ODR &= ~(GPIO_ODR_OD8);
		GPIOA->ODR &= ~(GPIO_ODR_OD15);		// 00
		seg7(temperatuur / 1000);
		break;

	case 1:
		clear();
		GPIOA->ODR |= (GPIO_ODR_OD8);
		GPIOA->ODR &= ~(GPIO_ODR_OD15);		// 10
		seg7((temperatuur / 100) % 10);
		break;

	case 2:
		clear();
		GPIOA->ODR |= (GPIO_ODR_OD6);
		GPIOA->ODR &= ~(GPIO_ODR_OD8);
		GPIOA->ODR |= (GPIO_ODR_OD15);		// 01
		seg7((temperatuur % 100) / 10);
		break;

	case 3:
		clear();
		GPIOA->ODR |= (GPIO_ODR_OD8);
		GPIOA->ODR |= (GPIO_ODR_OD15);		// 11
		seg7((temperatuur % 100) % 10);
		break;
	}*/
	mux++;
}

int __io_putchar(int ch){
    while(!(USART1->ISR & USART_ISR_TXE));
    USART1->TDR = ch;
}

int main(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;		// enable IO port C clock -- clock activeren voor GPIO C

    GPIOA->MODER &= ~GPIO_MODER_MODE9_Msk;
    GPIOA->MODER |=  GPIO_MODER_MODE9_1;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT9;
    GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL9_Msk)) | (0x7 << GPIO_AFRH_AFSEL9_Pos);

    GPIOA->MODER &= ~GPIO_MODER_MODE10_Msk;
    GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL10_Msk)) | (0x7 << GPIO_AFRH_AFSEL10_Pos);

    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;
    USART1->BRR = 417;
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

	// CPU Frequentie = 48 MHz
	// Systick interrupt elke 1 ms (1kHz)  --> 48000000 Hz / 1000 Hz --> Reload = 48000
	SysTick_Config(48000);

	// Interrupt aanzetten met een prioriteit van 128
	NVIC_SetPriority(SysTick_IRQn, 128);
	NVIC_EnableIRQ(SysTick_IRQn);

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;		// enable IO port C clock -- clock activeren voor GPIO C

	// Klok aanzetten
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

	// Klok selecteren, hier gebruiken we sysclk
	RCC->CCIPR &= ~RCC_CCIPR_ADCSEL_Msk;
	RCC->CCIPR |= RCC_CCIPR_ADCSEL_0 | RCC_CCIPR_ADCSEL_1;

	// Deep powerdown modus uitzetten
	ADC1->CR &= ~ADC_CR_DEEPPWD;

	// ADC voltage regulator aanzetten
	ADC1->CR |= ADC_CR_ADVREGEN;

	// Delay a few miliseconds, see datasheet for exact timing
	delay(4);

	// Kalibratie starten
	ADC1->CR |= ADC_CR_ADCAL;
	while(ADC1->CR & ADC_CR_ADCAL);

	// ADC aanzetten
	ADC1->CR |= ADC_CR_ADEN;

	// kanalen instellen
	ADC1->SMPR1 |= (ADC_SMPR1_SMP5_0 | ADC_SMPR1_SMP5_1 | ADC_SMPR1_SMP5_2);
	ADC1->SMPR1 |= (ADC_SMPR1_SMP6_0 | ADC_SMPR1_SMP6_1 | ADC_SMPR1_SMP6_2);
	//ADC1->SQR1 &= ~(ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_1 | ADC_SQR1_SQ1_2 | ADC_SQR1_SQ1_3);

	//NTC
    GPIOA->MODER &= ~GPIO_MODER_MODE0_Msk;		// port mode register mask van GPIOA pin 0 laag zetten

    GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE0_1;		// port mode register van GPIOA pin 0 op 11 zetten -> analog mode



    while (1) {
    	value = readNTC();
    	V = (value*3.0f)/4096.0f;
    	R = (10000.0f*V)/(3.0f-V);
    	temperatuur = ((1.0f/((logf(R/10000.0f)/3936.0f)+(1.0f/298.15f)))-273.15f);

    	printf("%2.2f",temperatuur);
    	printf("°C\n\r");

    	delay(1000);
    }
}

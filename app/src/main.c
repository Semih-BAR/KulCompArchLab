#include <stdint.h>
#include <stm32l4xx.h>
#include <math.h>

int mux = 0;
int tick = 0;
double hoek = 0;

void delay(unsigned int n){
	volatile unsigned int delay = n;
	while (delay != 0){
		if (tick){
			delay--;
			tick = 0;
		}
	}
}

void SysTick_Handler(void){
	tick++;
	mux++;
}

int __io_putchar(int ch){
    while(!(USART1->ISR & USART_ISR_TXE));
    USART1->TDR = ch;
}

void write_accel(int data, int reg){
	    I2C1->CR2 &= ~(1<<10);//enable write mode
		I2C1->CR2 |= I2C_CR2_NACK_Msk;
	    I2C1->CR2 |=  (1 << 13)|(2 << 16)|(0x53 << 1); //grote te verzende paket, conencted device,
	    while((I2C1->ISR & (1<<4)) == 0 && (I2C1->ISR & (1<<1)) == 0);
	     //NACKF = 0, TXIS = 0
	    if((I2C1->ISR & (1<<4)) != 0){ //NACKF = 1
	        return;
	    }

	    I2C1->TXDR = reg;//register doorsturen

	    while(I2C1->ISR & (1<<4) == 0 && I2C1->ISR & (1<<1) == 0){};
	         //NACKF = 0, TXIS = 0
	    if((I2C1->ISR & (1<<4)) != 0){ //NACKF = 1
	        return;
	    }
	    I2C1->TXDR = data;
		while((I2C1->ISR & I2C_ISR_STOPF) == 0);
}

int read_accel(int reg){
	while((I2C1->ISR & I2C_ISR_BUSY));
	I2C1->CR2 &= ~(1<<10);//enable write mode
	I2C1->CR2 &= ~I2C_CR2_AUTOEND_Msk;
	I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
	I2C1->CR2 |= I2C_CR2_NACK_Msk;
	I2C1->CR2 |=  (1 << 13)|(1 << 16)|(0x53 << 1); //grote te verzende paket, conencted device,
	while(((I2C1->ISR & (1<<4)) == 0) && ((I2C1->ISR & (1<<1)) == 0)){};
	 //NACKF = 0, TXIS = 0
	if((I2C1->ISR & (1<<4)) != 0){ //NACKF = 1
		return;
	}

	I2C1->TXDR = reg;//register doorsturen
	while((I2C1->ISR & (1<<6)) == 0);


	I2C1->CR2 |= I2C_CR2_AUTOEND_Msk;
	I2C1->CR2 |= (1<<10);//enable read mode
	//read
	I2C1->CR2 |=  (1 << 16)|(0x53 << 1); //grote te verzende paket, conencted device,
	I2C1->CR2 |= (1<<13);
	while(!(I2C1->ISR & I2C_ISR_RXNE));

	return I2C1->RXDR;

}

int main(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;		// enable IO port C clock -- clock activeren voor GPIO C
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

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

	// I2C
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

	GPIOB->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE6_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT6;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL6_Msk)) | (0x4 << GPIO_AFRL_AFSEL6_Pos);

	GPIOB->MODER &= ~GPIO_MODER_MODE7_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE7_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT7;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL7_Msk)) | (0x4 << GPIO_AFRL_AFSEL7_Pos);

	I2C1->TIMINGR = 0x20303E5D;
	I2C1->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);
	I2C1->CR1 |= I2C_CR1_PE;

	volatile int16_t array[3];
	write_accel(1<<3,0x2D);
	array[0] =  read_accel(0x2D);

    while (1) {
    	for (int i = 0; i<3; i++){
			array[i] = read_accel(0x32+i*2)<<8+read_accel(0x32+i*2+1);
		}

    	int xy = sqrt(array[0]^2+array[1]^2);
		int xyz = sqrt(xy^2+array[2]^2);
		hoek = (acos(array[2]/(sqrt(array[0]*array[0]+array[1]*array[1]+array[2]*array[2]))))*(180/3.14);

    	printf("%2.2f",hoek);
    	printf("\n\r");

    	delay(1000);
    }
}

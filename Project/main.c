#include "stm32f10x.h"
#include "Value.h"

void UpdateClock72MHz()
{
	//Enable HSI.
	RCC->CR |= (1 << 0);
	//Wait till HSI is ready.
	while(!(RCC->CR & (1 << 1)));
	
	//HSI selected 	as system clock.
	RCC->CFGR &= ~(3 << 0);
	
	//Wait SW select HSI clock as system clock.
	while((RCC->CFGR & (3 << 2)) != 0);
	
	//Disable PLL.
	RCC->CR &= ~(1 << 24);
	
	//Clear bit PLLMUL.
	RCC->CFGR &= ~(15 << 18);
	//PLLMUL = 9.
	RCC->CFGR |= (7 << 18);
	
	//HSE not divided.
	RCC->CFGR &= ~(1 << 17);
	
	//HSE clock selected as PLL input clock.
	RCC->CFGR |= (1 << 16);
	
	//AHB not divided.
	RCC->CFGR  &= ~(15 << 4);
	
	//APB1 divided 4.
	RCC->CFGR &= ~(7 << 8);
	RCC->CFGR |= (5 << 8);
	
	//Enable HSE.
	RCC->CR |= (1 << 16);
	//Wait till HSE is ready.
	while(!(RCC->CR & (1 << 17)));
	
	//Enable PLL.
	RCC->CR |= (1 << 24);
	//Wait till PLL is ready.
	while(!(RCC->CR & (1 << 25)));
	
	//PLL clock as SW input clock.
	RCC->CFGR |= (2 << 0);
	//Wait till SW is ready.
	while((RCC->CFGR & (3 << 2)) != (2 << 2));
}
void ADC1_Config()
{
	/****** Config ADC1_IN1 NVIC ******/
	
	//Priority ADC1 = 8.
	NVIC->IP[4] |= (8 << 20);
	
	//Enable ADC1 NVIC.
	NVIC->ISER[0] |= (1 << 18);
	
	/******Enable Clock gate GPIOA, ADC1 *****/
	
	RCC->APB2ENR |= (1 << 2) | (1 << 9);
	
	/*******Config ADC1_CH1 pin (PTA1) ********/
	
	//Input mode PTA1
	GPIOA->CRL &= ~(3 << 4);
	//Analog mode PTA1.
	GPIOA->CRL &= ~(3 << 6);
	
	/*******Config clock *********/
	
	//ADC1 Clock = 9MHz.
	RCC->CFGR |= (3 << 14);
	
	//Enable EOC (End of Conversion interrupt)
	ADC1->CR1 |= (1 << 5);
	
	//Conversion on external event enabled.
	ADC1->CR2 |= (1 << 20);
	//SWSTART is triger of regular.
	ADC1->CR2 |= (7 << 17);
	
	//Right Alignment data.
	ADC1->CR2 &= ~(1 << 11);
	
	//Single conversion mode.
	ADC1->CR2 &= ~(1 << 1);
	
	//1 conversion.
	ADC1->SQR1 &= ~(15 << 20);
	
	//Clear bit SQR1(SQR3)
	ADC1->SQR3 &= ~(31 << 0);
	//SQR1(SQR3) = ADC1-IN1
	ADC1->SQR3 |= (1 << 0);
	
	//Enable ADC1.
	ADC1->CR2 |= (1 << 0);
	
}
void I2C_Config()
{
	//Enable clock gate I2C1 and GPIOB.
	RCC->APB2ENR |= (1 << 3);
	RCC->APB1ENR |= (1 << 21);
	
	/**** Config I2C1 pin ****/
	
	// Output mode, max speed 50 MHz PTB6.
	GPIOB->CRL |= (3 << 24);
	//Alternate function output Open-drain
	GPIOB->CRL |= (3 << 26);
	
	// Output mode, max speed 50 MHz PTB7.
	GPIOB->CRL |= (3 << 28);
	//Alternate function output Open-drain
	GPIOB->CRL |= (3 << 30);
	
	/**** Config I2C1 ****/
	
	//Pclk1 = 18MHz
	I2C1->CR2 &= ~(63 << 0);
	I2C1->CR2 |= (18 << 0);
	
	//Standard Mode I2C.
	I2C1->CCR &= ~(1 << 15);
	
	//Speed = 100KHz -> CCR = 5us / (1 / 18 MHz) = 90.
	I2C1->CCR &= ~(4095 << 0);
	I2C1->CCR |= (90 << 0);
	
	//Rise time = 0.5us
	I2C1->TRISE &= ~(63 << 0);
	I2C1->TRISE |= (91 << 0);
	
	//Enable I2C1.
	I2C1->CR1 |= (1 << 0);
}

char Read_DHT12(char cmd)
{
	char Data;
	
	//Send cmd
	
	//Start generation.
	I2C1->CR1 |= (1 << 8);
	
	//Wait SB bit on.
	while(!(I2C1->SR1 & (1 << 0)));
	
	//Clear SB bit.
	I2C1->SR1;
	
	//Send Address.
	I2C1->DR = 0xB8;
	
	//Wait ADDR bit on.
	while(!(I2C1->SR1 & (1 << 1)));
	
	//Clear ADDR bit.
	I2C1->SR1;
	I2C1->SR2;
	
	//Wait TxE bit on.
	while(!(I2C1->SR1 & (1 << 7)));
	
	//Send cmd.
	I2C1->DR = cmd;
	
	//Wait TxE anf BTF bit on.
	while((!(I2C1->SR1 & (1 << 7))) && (!(I2C1->SR1 & (1 << 2))));
	
	//Get data.
	
	//Start generation.
	I2C1->CR1 |= (1 << 8);
	
	//Wait SB bit on.
	while(!(I2C1->SR1 & (1 << 0)));
	
	//Clear SB bit.
	I2C1->SR1;
	
	//Send Address.
	I2C1->DR = 0xB9;
	
	//Wait ADDR bit on.
	while(!(I2C1->SR1 & (1 << 1)));
	
	//Clear ADDR bit.
	I2C1->SR1;
	I2C1->SR2;
	
	//Wait RxE bit on.
	while(!(I2C1->SR1 & (1 << 6)));
	
	Data = I2C1->DR;
	
	//Stop generation.
	I2C1->CR1 |= (1 << 9);

	return Data;
}
void UART1_Config()
{
	/**** Config UASRT1 NVIC ****/
	
	//Priotiry = 1.
	NVIC->IP[9] |= (1 << 12);
	
	//Enable NVIC.
	NVIC->ISER[1] |= (1 << 5);
	
	/**** Enable Clock gate GPIOA, USART1 ****/	
	RCC->APB2ENR |= (1 << 2) | (1 << 14);
	
	/**** Config USART pin ****/
	
	/**** Config PTA9 - USART-TX. ****/
	
	//Output mode, Speed max 50MHz.
	GPIOA->CRH |= (3 << 4);
	//Alternate function output Push-pull.
	GPIOA->CRH &= ~(3 << 6);
	GPIOA->CRH |= (2 << 6);
	
	/****Config PTA10 - USART - RX.****/
	
	//Input mode
	GPIOA->CRH &= ~(3 << 8);
	
	//Floating input.
	GPIOA->CRH &= ~(3 << 10);
	GPIOA->CRH |= (1 << 10);
	
	/**** Config USART1 ****/
	
	//Baud rate : 9600 -> BRR = 72MHZ / (9600 * 16) = 468.75.
	USART1->BRR = (468 << 4) | (12 << 0);
	
	
	// 1 Start bit, 8 Data bits.
	USART1->CR1 &= ~(1 << 12);
	
	//1 bit Stop.
	USART1->CR2 &= ~(3 << 12);
	
	//Disable Parity.
	USART1->CR1 &= ~(1 << 10);
	
	//Enable USART1.
	USART1->CR1 |= (1 << 13);
		
	//RXNE interrupt enable.
	USART1->CR1 |= (1 << 5);
	
	//Transmitter enable.
	USART1->CR1 |= (1 << 3);
	
	//Receiver enable.
	USART1->CR1 |= (1 << 2);
}
void Send_for_ESP8266()
{
	//Send
	USART1->DR = '#';
	//Wait TxE and TC on
	while((!(USART1->SR & (1 << 6))) && (!(USART1->SR & (1 << 7))));
	
	//Clear TC bit.
	USART1->SR &= ~(1 << 6);
	//Clear TxE bit.
	USART1->SR;
	
	//Send
	USART1->DR = Value.DoAmDat;
	//Wait TxE and TC on
	while((!(USART1->SR & (1 << 6))) && (!(USART1->SR & (1 << 7))));
	
	//Clear TC bit.
	USART1->SR &= ~(1 << 6);
	//Clear TxE bit.
	USART1->SR;
	
	//Send
	USART1->DR = Value.DoAmKK;
	//Wait TxE and TC on
	while((!(USART1->SR & (1 << 6))) && (!(USART1->SR & (1 << 7))));
	
	//Clear TC bit.
	USART1->SR &= ~(1 << 6);
	//Clear TxE bit.
	USART1->SR;
	
	//Send
	USART1->DR = Value.NhietDoKK;
	//Wait TxE and TC on
	while((!(USART1->SR & (1 << 6))) && (!(USART1->SR & (1 << 7))));
	
	//Clear TC bit.
	USART1->SR &= ~(1 << 6);
	//Clear TxE bit.
	USART1->SR;
	
	//Send
	USART1->DR = '@';
	//Wait TxE and TC on
	while((!(USART1->SR & (1 << 6))) && (!(USART1->SR & (1 << 7))));
	
	//Clear TC bit.
	USART1->SR &= ~(1 << 6);
	//Clear TxE bit.
	USART1->SR;
}

int Check_String(char *KT)
{
	uint8_t i = 0;
	while(i < RX_leng)
	{
		if(Data_Receiver[i] != *(KT + i))
		{
			return FALSE;
		}
		i++;
	}
	return TRUE;
}
void RTC_Config()
{
	// Enable PWR and BKP clocks 
	RCC->APB1ENR |= (1 << 27) | (1 << 28);

	// Allow access to BKP Domain 
	PWR->CR |= (1 << 8);

	// Reset Backup Domain 
	RCC->BDCR |= (1 << 16);
	RCC->BDCR &= ~(1 << 16);
	
	// Select the RTC Clock Source is HSE / 128 = 62,500 KHz.
	RCC->BDCR |= (3 << 8);

	// Enable RTC Clock 
	RCC->BDCR |= (1 << 15);

	// Wait for RTC registers synchronization 
	while(!(RTC->CRL & (1 << 3)));
	RTC->CRL &= ~(1 << 3);
	
	// Wait until last write operation on RTC registers has finished 
	while(!(RTC->CRL & (1 << 5)));
	//Enter configuration mode
	RTC->CRL |= (1 << 4);
	
	//Precaler load = 62500.
	RTC->PRLL = 62500;
	
	//Count = 0.

	RTC->CNTL = 0;
	
	// Exit configuration mode
	RTC->CRL &= ~(1 << 4);
	
	// Wait until last write operation on RTC registers has finished 
	while(!(RTC->CRL & (1 << 5)));
}
void Delay_ms(uint32_t time)
{
	Count = time;
	while(Count != 0);
}
void SysTick_Configuration()
{
	//Disable SysTick.
	SysTick->CTRL &= ~(1 << 0);
	
	//Use funtion SysTick_Handler.
	SysTick->CTRL |= (1 << 1);
	
	//External clock.
	SysTick->CTRL &= ~(1 << 2);
	
	//Clear Current.
	SysTick->VAL =0;
	
	//Reload: 1ms -> 72.000.000 / 1000 / 8 = 9000.
	SysTick->LOAD = 9000;
	
	//Enable SysTick.
	SysTick->CTRL |= (1 << 0);
}
void Check_Light()
{
	uint32_t Temp = 0;
	Temp |= (RTC->CNTH << 16);
	Temp |= RTC->CNTL;
	if(Temp > 86400)
		Temp = Temp - (Temp / 86400) * Temp;
	if(((Temp >= 1 ) && (Temp < 21600)) && (AUTO == TRUE))
	{
		if(GPIOA->IDR & (1 << 2))
		{
			GPIOA->ODR |= (1 << 6);
		}
		else
		{
			GPIOA->ODR &= ~(1 << 6);
		}
	}
}

void Config_Check_Light()
{
	//Enable Clock gate.
	RCC->APB2ENR |= (1 << 2);

	//Input mode pin PTA2.
	GPIOA->CRL &= ~(3 << 8);
	
	//Clear bit CNF.
	GPIOA->CRL &= ~(3 << 10);
	//Input with pull-up PTA2.
	GPIOA->CRL |= (2 << 10);
	GPIOA->ODR |= (1 << 2);
	
}
void GPIO_Config()
{
	//Enable clock gate GPIOA.
	RCC->APB2ENR |= (1 << 2);
	
	/****Config pin Led Control is PTA6 ****/
	//Output mode, speed max 50MHZ.
	GPIOA->CRL |= (3 << 24);
	// General purpose output push-pull.
	GPIOA->CRL &= ~(3 << 26);
	
	/****Config pin Control is PTA7 ****/
	//Output mode, speed max 50MHZ.
	GPIOA->CRL |= (3 << 28);
	// General purpose output push-pull.
	GPIOA->CRL &= ~(3 << 30);
	
}
int main()
{
	UpdateClock72MHz();
	RTC_Config();
	UART1_Config();
	ADC1_Config();
	I2C_Config();
	GPIO_Config();
	Config_Check_Light();
	SysTick_Configuration();
	while(1)
	{
		Delay_ms(4000);
	 	Value.DoAmKK = Read_DHT12(0x00);
		Delay_ms(4000);
		Value.NhietDoKK = Read_DHT12(0x02);
	}
}
void SysTick_Handler()
{
	Count--;	
	if((RT % 1000) == 0)
	{
		Check_Light();
		//Conversion ADC1.
		ADC1->CR2 |= (1 << 22);
	}
	if(RT >= 10000)
	{
		//Send data
		Send_for_ESP8266();
		RT = 0;
	}
	else
	{
		RT++;
	}
}
void USART1_IRQHandler()
{
	//Get data
	if(USART1->DR == '#')
	{
		if(Check_String("AUTO") == TRUE) //Enable AUTO mode
		{
			AUTO = TRUE;
			
			//Off pump.
			GPIOA->ODR &= ~(1 << 7);
		}
		else if(Check_String("NOT_AUTO") == TRUE) //Disable AUTO mode
		{
			AUTO = FALSE;
			//Off light.
			GPIOA->ODR &= ~(1 << 6);
			//Off pump
			GPIOA->ODR &= ~(1 << 7);
		}
		else if((Check_String("ON_LIGHT") == TRUE) && (AUTO == FALSE)) // On light
		{
			GPIOA->ODR |= (1 << 6);
		}
		else if((Check_String("ON_PUMP") == TRUE) && (AUTO == FALSE)) //Bat may bom
		{
			GPIOA->ODR |= (1 << 7);
		}
		else if((Check_String("OFF_LIGHT") == TRUE) && (AUTO == FALSE)) // Off light
		{
			GPIOA->ODR &= ~(1 << 6);
		}
		else if((Check_String("OFF_PUMP") == TRUE) && (AUTO == FALSE)) //Tat may bom
		{
			GPIOA->ODR &= ~(1 << 7);
		}
		USART1->DR;
		RX_leng = 0;
	}
	else if(USART1->DR == '*')
	{
		RX_leng = 0;
		USART1->DR;
	}
	else
	{
		Data_Receiver[RX_leng] = USART1->DR;
		RX_leng++;
	}
	USART1->SR &= ~(1 << 5);
}
void ADC1_2_IRQHandler()
{
	//  < 30%
	if((ADC1->DR > 2867) && (AUTO == TRUE))
	{
		GPIOA->ODR |= (1 << 7);
	}
	// > 60%
	else if((ADC1->DR <= 1638) && (AUTO == TRUE))
	{
		GPIOA->ODR &= ~(1 << 7);
	}
	Value.DoAmDat = (char)(100 - (((float)(ADC1->DR) / 4095) * 100));
	ADC1->SR &= ~(1 << 1);
}

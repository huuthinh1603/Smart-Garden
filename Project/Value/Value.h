#include "stm32f10x.h"

#ifndef _VALUE_H_
#define _VALUE_H_

#define TRUE 1
#define FALSE 0
struct Sensor{
	char DoAmDat;
	char DoAmKK ;
	char NhietDoKK;
};
uint32_t RT = 0;
char i;
struct Sensor Value;
char Data_Receiver[15];//Data Receiver.
uint8_t RX_leng = 0;//String length Transmitter.
uint32_t Count; //Counter Delay_ms.
uint8_t AUTO = TRUE; //Flag AUTO mode.
char Temp_DoAmDat[10];
void I2C_Config(void);
char Read_DHT12(char);
void ADC1_Config(void);
void UART1_Config(void);
void Send_for_ESP8266(void);
int Check_String(char *);
void RTC_Config(void);
void UpdateClock72MHz(void);
void GPIO_Config(void);
void Config_Check_Light(void);
void Check_Light(void);
void Delay_ms(uint32_t);
void SysTick_Configuration(void);
#endif


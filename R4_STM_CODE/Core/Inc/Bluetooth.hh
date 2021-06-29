#ifndef BLUETOOTH_LIB
#define BLUETOOTH_LIB // spelling error



#include "stdbool.h"
#include "gpio.h"
#include "string.h"
#include "tim.h"
#include "stdlib.h"
#include "L298.hh"

#include "usart.h"
#include "stm32f1xx_it.h"




class Bluetooth_module{

	UART_HandleTypeDef *huart;

public:


	Bluetooth_module(UART_HandleTypeDef *huart);

////////////////////VARIABLES//////////////////////
	char buff [50];
	 uint8_t timcnt;
	 uint8_t buffindex;
//////////////////////////////////////////////////






/////////////////////////FUNCTIONS//////////////////////////////////

bool stringCompare(char array1[], char array2[], uint16_t lght);
void interrupt_USART(UART_HandleTypeDef *_huart);
void interrupt_TIM();
void doCommand(Motor *Mot);
void doCommand1();
void resolveCommand();
///////////////////////////////////////////////////////////////////
};
#endif


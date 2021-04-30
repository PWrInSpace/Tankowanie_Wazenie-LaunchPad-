#include "hx711.h"

int32_t HX711_Read_Value(GPIO_TypeDef *Dt_gpio, uint16_t Dt_pin, GPIO_TypeDef *Sck_gpio, uint16_t Sck_pin){
    int32_t buffer=0;

    HAL_GPIO_WritePin(Sck_gpio, Sck_pin, GPIO_PIN_RESET);

    while (HAL_GPIO_ReadPin(Dt_gpio, Dt_pin)==1)
    	;

    for (uint8_t i = 0; i < 25; i++){
    	HAL_GPIO_WritePin(Sck_gpio, Sck_pin, GPIO_PIN_SET);
        buffer = buffer << 1 ;
        buffer+=HAL_GPIO_ReadPin(Dt_gpio, Dt_pin);
        HAL_GPIO_WritePin(Sck_gpio, Sck_pin, GPIO_PIN_RESET);
        //there should be delay (i think)
    }

/* if gain is not 128: (gain(channel B)==32):gain=1; (gain==64):gain=2; */
//    for (uint8_t i=0; i < gain; i++){
//    	HAL_GPIO_WritePin(Sck_gpio, Sck_pin, GPIO_PIN_SET);
//    	HAL_GPIO_WritePin(Sck_gpio, Sck_pin, GPIO_PIN_RESET);
//    }

    while (HAL_GPIO_ReadPin(Dt_gpio, Dt_pin)==0)
    	;

    return (buffer<<7)/128;
}

int32_t HX711_Average_Value(GPIO_TypeDef *Dt_gpio, uint16_t Dt_pin, GPIO_TypeDef *Sck_gpio, uint16_t Sck_pin, uint8_t times){
	int32_t sum = 0;
    for (uint8_t i = 0; i < times; i++){
        sum += HX711_Read_Value(Dt_gpio, Dt_pin, Sck_gpio, Sck_pin);
    }
    return sum / times;
}
#include "Funkcje.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == _GPS_USART.Instance)
		GPS_CallBack();

	/************************************************/

	if (huart->Instance == USART2) {
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
		HAL_UART_DMAStop(&huart2);

		xbee_receive();
		if (xbee_rx.data_flag) {

			if (strstr(xbee_rx.data_array, "R4TN") != NULL) {

				tfsStruct.tanwaRxFlag = 1;
				strcpy(tfsStruct.tanwaStringLora, xbee_rx.data_array);
			}

		}

		HAL_UART_Receive_DMA(&huart2, (uint8_t*) xbee_rx.mess_loaded,
		DATA_LENGTH);
	}

	/************************************************/

	if (huart->Instance == USART1) {

		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		HAL_UART_DMAStop(&huart1);

		if (strstr(tfsStruct.maincompStringDma, "R4MC") != NULL) {

			tfsStruct.maincompRxFlag = 1;
			strcpy(tfsStruct.maincompStringLora, tfsStruct.maincompStringDma);

			if (tfsStruct.maincompStringDma[5] == '2') gpsPeriod = 1000;
			else gpsPeriod = 8000;
		}

		if (strstr(tfsStruct.maincompStringDma, "TNWN;DSTA") != NULL) {

			ignite = 1;
		}

		memset(tfsStruct.maincompStringDma, 0, RX_BUFFER_SIZE);
		HAL_UART_Receive_DMA(&huart1, (uint8_t*) tfsStruct.maincompStringDma,
		RX_BUFFER_SIZE);
	}

}

/*******************************************************************************************/

void initAll(void) {

	loraInit();
	GPS_Init();

	gpsPeriod = 1000;

	// Inity uartów:

	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2, (uint8_t*) xbee_rx.mess_loaded, DATA_LENGTH);

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1, (uint8_t*) tfsStruct.maincompStringDma,
	RX_BUFFER_SIZE);

	xbee_init(&xbeeIgnition, 0x0013A20041A26FA2, &huart2);

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) hallSensors, 6);

	// Inity struktury:

	tfsStruct.gpsFrameTimer = uwTick;
	tfsStruct.tanwaRxFlag = 0;
	tfsStruct.maincompRxFlag = 0;
}

/*******************************************************************************************/

void loraReaction(void) {

	// Przesyłanie wiadomości z LoRy do Maincompa:
	if (strstr(loraBuffer, "MNCP") != NULL) {
		HAL_UART_Transmit(&huart1, (uint8_t*) loraBuffer, strlen(loraBuffer),
				100);
	}

	// Przesyłanie wiadomości z LoRy do Tanwy:
	else if (strstr(loraBuffer, "TNWN") != NULL) {
		xbee_transmit_char(xbeeIgnition, loraBuffer);
	}

	HAL_Delay(20);
	memset(loraBuffer, 0, BUFFER_SIZE);
}

/*******************************************************************************************/

void sendData(void) { //tankowanie test
	int len = sprintf(tfsStruct.gpsStringLora,
			"R4GP;%i;%i;%i;%i;%i;C:%i\n",
			(int) hallSensors[0], (int) hallSensors[1], (int) hallSensors[2],
			(int) hallSensors[3], (int) hallSensors[4], (int) hallSensors[5]);

	//loraSendData((uint8_t*) tfsStruct.gpsStringLora, len);

	HAL_UART_Transmit(&huart1, (uint8_t*) tfsStruct.gpsStringLora, len, 100);

	HAL_Delay(100);
}

/*******************************************************************************************/

void sendFromMaincompToLora(void) {

	loraSendData((uint8_t*) tfsStruct.maincompStringLora,
			strlen(tfsStruct.maincompStringLora));
	HAL_Delay(100);
}

/*******************************************************************************************/

void sendFromTanwaToLora(void) {

	size_t len = strlen(tfsStruct.tanwaStringLora);

	loraSendData((uint8_t*) tfsStruct.tanwaStringLora, len);

	HAL_UART_Transmit(&huart1, (uint8_t*) tfsStruct.tanwaStringLora, len, 100);

	HAL_Delay(100);
}
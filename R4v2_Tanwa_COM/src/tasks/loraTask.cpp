#include "../include/tasks/tasks.h"

void loraTask(void *arg){
  char loraTx[LORA_TX_FRAME_SIZE] = {};
  char loraRx[LORA_RX_FRAME_SIZE] = {};

  //TanWaControl * tc = static_cast<TanWaControl*>(arg);
  
  vTaskDelay(25 / portTICK_PERIOD_MS);
  
  xSemaphoreTake(stm.spiMutex, portMAX_DELAY);

  LoRa.setSPI(stm.spi);
  LoRa.setPins(LORA_CS, LORA_RS, LORA_D0);
  
  while(LoRa.begin(LORA_FREQ_MHZ * 1E6) == 0){ //DEBUG
    Serial.println("LORA begin error!");
    xSemaphoreGive(stm.spiMutex);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xSemaphoreTake(stm.spiMutex, portMAX_DELAY);
  }

  LoRa.setSignalBandwidth(250E3);
  LoRa.disableCrc();
  LoRa.setSpreadingFactor(7);
  LoRa.setTxPower(14);
  LoRa.setTimeout(10);

  xSemaphoreGive(stm.spiMutex);

  vTaskDelay(100 / portTICK_PERIOD_MS);

  while(1){
    xSemaphoreTake(stm.spiMutex, portMAX_DELAY);
    LoRa.parsePacket();
    if (LoRa.available()) {
      String rxStr = LoRa.readString();
      strcpy(loraRx, rxStr.c_str());
     
      
      //send data to rxHandleTask
      if(xQueueSend(stm.loraRxQueue, (void*)&loraRx, 0) == pdFALSE){
      }
     
      memset(loraRx, 0, LORA_RX_FRAME_SIZE);
    }
    xSemaphoreGive(stm.spiMutex);

    if(xQueueReceive(stm.loraTxQueue, (void*)&loraTx, 0) == pdTRUE){
      xSemaphoreTake(stm.spiMutex, portMAX_DELAY);
        
      LoRa.beginPacket();
      LoRa.write((uint8_t*) loraTx, strlen(loraTx));
      LoRa.endPacket();

      xSemaphoreGive(stm.spiMutex);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
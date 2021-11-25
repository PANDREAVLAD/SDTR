#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

SemaphoreHandle_t Semaphore;
QueueHandle_t Queue;

void task_seriala(void *param);

/// Pini

int Rosu_masini   = 7;
int Galben_masini = 8;
int Verde_masini  = 9;
int Rosu_pietoni  = 10;
int Verde_pietoni = 11;

void setup() {
   Serial.begin(9600);
   Queue = xQueueCreate(10, sizeof(byte));
   Semaphore = xSemaphoreCreateMutex();
   
   pinMode(Rosu_masini, OUTPUT);
   pinMode(Galben_masini, OUTPUT);
   pinMode(Verde_masini, OUTPUT);
   pinMode(Rosu_pietoni, OUTPUT);
   pinMode(Verde_pietoni, OUTPUT);

   digitalWrite (Rosu_masini,LOW);
   digitalWrite (Galben_masini,LOW);
   digitalWrite (Verde_masini,LOW);
   digitalWrite (Rosu_pietoni,LOW);
   digitalWrite (Verde_pietoni,LOW);
   xTaskCreate(task_seriala, "Citire seriala", 128, NULL, 1, NULL);
   xSemaphoreGive(Semaphore);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void task_seriala(void *param)
{
   byte mesaj = 0x00;
   byte citire = 0x00;
   
   while(true)
   {if (xSemaphoreTake(Semaphore, 10) == pdTRUE)
   {
       if (xQueueReceive(Queue,&mesaj,10 ) == pdPASS){Serial.println(mesaj);}
       
       citire = Serial.read();
       
       if (citire != 255){
           xQueueSend(Queue, &citire, portMAX_DELAY);
       }
       xSemaphoreGive(Semaphore);
   }
   vTaskDelay(1000 / portTICK_PERIOD_MS);
   }
}

//// Testarea transmisieu si citirii valorilor prin seriala


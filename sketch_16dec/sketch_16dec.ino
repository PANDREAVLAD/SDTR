#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

SemaphoreHandle_t Semaphore;
QueueHandle_t Queue;


/// Pini

int Rosu_masini   = 7;
int Galben_masini = 8;
int Verde_masini  = 9;
int Rosu_pietoni  = 10;
int Verde_pietoni = 11;

void task_seriala(void *param);
void task_lumini(void *param);

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

   xTaskCreate(task_seriala, "Citire seriala", 128, NULL, 2, NULL);
   xTaskCreate(task_lumini, "Control lumini", 128, NULL, 1, NULL);
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
       while(xQueueReceive(Queue,&mesaj,10 ) == pdPASS){ Serial.print(mesaj);}
       Serial.println();
       citire = 0;
       while (citire != 255){
           citire = Serial.read();    // Citeste
           if (citire != 255) xQueueSend(Queue, &citire, portMAX_DELAY);   // Daca a citit ceva, transmite
       }       
       xSemaphoreGive(Semaphore);
   }
   vTaskDelay(10 / portTICK_PERIOD_MS);
   }
}

void task_lumini(void *param)
{
    int Rosu_masini_valoare = 10;
    int Galben_masini_valoare = 10;
    int Verde_masini_valoare = 10;
    
    byte valori[10];
    byte mesaj;
    int index_citit = 0;
    int index_procesare = 0;
    while(true){
    if (xSemaphoreTake(Semaphore, 10) == pdTRUE)
    {
       index_citit = 0;
       index_procesare = 0;

       // Manage queue
       while(xQueueReceive(Queue,&mesaj,10 ) == pdPASS){ valori[index_citit] = mesaj; index_citit++;}
       while(index_citit > index_procesare){
          if (valori[index_procesare] == 'R') { Rosu_masini_valoare = valori[index_procesare+1]*10;   index_procesare = index_procesare + 2; }
          if (valori[index_procesare] == 'G') { Galben_masini_valoare = valori[index_procesare+1]*10; index_procesare = index_procesare + 2; }
          if (valori[index_procesare] == 'V') { Verde_masini_valoare = valori[index_procesare+1]*10;  index_procesare = index_procesare + 2; }
       }
       ///////////////// Control
       
       
       xSemaphoreGive(Semaphore);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}





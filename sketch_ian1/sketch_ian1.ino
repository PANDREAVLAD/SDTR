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
       
       citire = 0;
       while (citire != 255){
           citire = Serial.read();    // Citeste
           if (citire != 255) xQueueSend(Queue, &citire, portMAX_DELAY);   // Daca a citit ceva, transmite
       }       
       xSemaphoreGive(Semaphore);
   }
   vTaskDelay(50 / portTICK_PERIOD_MS);
   }
}

void task_lumini(void *param)
{
    int Rosu_masini_valoare = 1;
    int Galben_masini_valoare = 1;
    int Verde_masini_valoare = 1;
    int Caz_Special_valoare = 10;

    int stare = 0;
    int contor = 0;
    
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
          if (valori[index_procesare] == 'R') { Rosu_masini_valoare = valori[index_procesare+1];   index_procesare = index_procesare + 2; }
          if (valori[index_procesare] == 'G') { Galben_masini_valoare = valori[index_procesare+1]; index_procesare = index_procesare + 2; }
          if (valori[index_procesare] == 'V') { Verde_masini_valoare = valori[index_procesare+1];  index_procesare = index_procesare + 2; }
          if (valori[index_procesare] == 'C') { Caz_Special_valoare = valori[index_procesare+1];   index_procesare = index_procesare + 2; }
       }
       ///////////////// Control
       contor = contor + 1;
       if (stare == 0 & contor == Rosu_masini_valoare) {contor = 0; stare = 1; xQueueSend(Queue, &stare, portMAX_DELAY);}    // Rosu 
       if (stare == 1 & contor == Verde_masini_valoare) {contor = 0; stare = 2; xQueueSend(Queue, &stare, portMAX_DELAY);}    // Galben
       if (stare == 2 & contor == Galben_masini_valoare) {contor = 0; stare = 0; xQueueSend(Queue, &stare, portMAX_DELAY);}    // Verde
       
       // Caz special     if (stare == 3) ??????

       // Leduri 
       if (stare == 0 | stare == 3) {digitalWrite(Rosu_masini,HIGH);digitalWrite(Verde_masini,LOW);digitalWrite(Galben_masini,LOW);}
       if (stare == 1) {digitalWrite(Verde_masini,HIGH);digitalWrite(Galben_masini,LOW);digitalWrite(Rosu_masini,LOW);}
       if (stare == 2) {digitalWrite(Galben_masini,HIGH);digitalWrite(Verde_masini,LOW);digitalWrite(Rosu_masini,LOW);}

       if (stare == 0 && contor > 2 && stare == 0 && contor < Rosu_masini_valoare - 2) {digitalWrite(Verde_pietoni,HIGH);digitalWrite(Rosu_pietoni,LOW);}
       else {digitalWrite(Verde_pietoni,HIGH);digitalWrite(Rosu_pietoni,LOW);}
       
       xSemaphoreGive(Semaphore);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}





#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

SemaphoreHandle_t Semaphore;
QueueHandle_t Queue;


/// Pini

int Rosu_masini   = 10;
int Galben_masini = 9;
int Verde_masini  = 8;
int Rosu_pietoni  = 7;
int Verde_pietoni = 6;

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

   xTaskCreate(task_seriala, "Citire seriala", 128, NULL, 1, NULL);
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
       while(xQueueReceive(Queue,&mesaj,10 ) == pdPASS){Serial.print(mesaj);}
       
       citire = Serial.read();
       while (citire != 255){
           if (citire != 255) xQueueSend(Queue, &citire, portMAX_DELAY);   // Daca a citit ceva, transmite
           citire = Serial.read();
       }       
       xSemaphoreGive(Semaphore);
   }
   vTaskDelay(1000 / portTICK_PERIOD_MS);
   }
}

void task_lumini(void *param)
{
    int Rosu_masini_valoare = 10;
    int Galben_masini_valoare = 2;
    int Verde_masini_valoare = 5;
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
       while(xQueueReceive(Queue,&valori[index_citit],10 ) == pdPASS){ index_citit = index_citit + 1;}
       while(index_citit > index_procesare){
          if (valori[index_procesare] == 'R') { Rosu_masini_valoare = (int)valori[index_procesare+1];   index_procesare = index_procesare + 2; }
          if (valori[index_procesare] == 'G') { Galben_masini_valoare = valori[index_procesare+1]; index_procesare = index_procesare + 2; }
          if (valori[index_procesare] == 'V') { Verde_masini_valoare = valori[index_procesare+1];  index_procesare = index_procesare + 2; }
          if (valori[index_procesare] == 'C') { Caz_Special_valoare = valori[index_procesare+1];   index_procesare = index_procesare + 2; }
          if (valori[index_procesare] == 'y') { stare = 3; contor = 0; xQueueSend(Queue, &stare, portMAX_DELAY); index_procesare = index_procesare + 1; }
       }
       contor = contor + 1;
       if (stare == 0 & contor == Rosu_masini_valoare) {contor = 0; stare = 1; xQueueSend(Queue, &stare, portMAX_DELAY);}    // Rosu 
       if (stare == 1 & contor == Verde_masini_valoare) {contor = 0; stare = 2; xQueueSend(Queue, &stare, portMAX_DELAY);}    // Galben
       if (stare == 2 & contor == Galben_masini_valoare) {contor = 0; stare = 0; xQueueSend(Queue, &stare, portMAX_DELAY);}    // Verde
       if (stare == 3 & contor == Caz_Special_valoare) {contor = 0; stare = 1; xQueueSend(Queue, &stare, portMAX_DELAY);}    // Verde

       // Leduri 
       if (stare == 0 | stare == 3) {digitalWrite(Rosu_masini,HIGH);digitalWrite(Verde_masini,LOW);digitalWrite(Galben_masini,LOW);}
       if (stare == 1) {digitalWrite(Verde_masini,HIGH);digitalWrite(Galben_masini,LOW);digitalWrite(Rosu_masini,LOW);}
       if (stare == 2) {digitalWrite(Galben_masini,HIGH);digitalWrite(Verde_masini,LOW);digitalWrite(Rosu_masini,LOW);}

       if (stare == 0 && contor > 2 && stare == 0 && contor < Rosu_masini_valoare - 2) {digitalWrite(Verde_pietoni,HIGH);digitalWrite(Rosu_pietoni,LOW);}
       else {digitalWrite(Rosu_pietoni,HIGH);digitalWrite(Verde_pietoni,LOW);}
       
       xSemaphoreGive(Semaphore);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}





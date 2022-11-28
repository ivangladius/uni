
#include <MapleFreeRTOS900.h>

#define LED1 PE5
#define LED2 PE6


void vTaskCounter(void *pvParameters) {

  volatile int counter = 0;

  for (;;) {
    counter++;
    Serial.println(counter);

    vTaskDelay(pdMS_TO_TICKS(250)); 
  }
}

void vTaskPrintName(void *pvParameters) {

    for (;;) {
      Serial.println("Maximilian Ivan Filipov");

      digitalWrite(LED1, LOW);
      vTaskDelay(pdMS_TO_TICKS(1000)); 
      digitalWrite(LED1, HIGH);
    }
}

void vTaskLED(void *pvParameters) {

  for (;;) {
    digitalWrite(LED2, LOW);
    vTaskDelay(pdMS_TO_TICKS(750)); 
    digitalWrite(LED2, HIGH);
  }
}

void setup() {

  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  xTaskCreate(vTaskCounter, "Task Counter", 100, NULL, 1, NULL);
  xTaskCreate(vTaskPrintName, "Task Name Print", 100, NULL, 2, NULL);
  xTaskCreate(vTaskLED, "Task LED", 100, NULL, 2, NULL);

  vTaskStartScheduler();
}

void loop() {
  // nothing
}

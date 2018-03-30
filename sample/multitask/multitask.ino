#include <M5Stack.h>

void task1(void *pvParameters)
{
  for (;;)
  {
    Serial.print("task1 Uptime (ms): ");
    Serial.println(millis());
    delay(100);
  }
}

void task2(void *pvParameters)
{
  for (;;)
  {
    Serial.print("task2 Uptime (ms): ");
    Serial.println(millis());
    delay(200);
  }
}

void task3(void *pvParameters)
{
  for (;;)
  {
    Serial.print("task3 Uptime (ms): ");
    Serial.println(millis());
    delay(1000);
  }
}


void setup()
{
  M5.begin();

  // Task 1
  xTaskCreatePinnedToCore(
    task1,
    "task1",
    4096,
    NULL,
    1,
    NULL,
    0);
  );
  // Task 2
  xTaskCreatePinnedToCore(
    task2,
    "task2",
    4096,
    NULL,
    2,
    NULL,
    0);
  );
  // Task 3
  xTaskCreatePinnedToCore(
    task3,
    "task3",
    4096,
    NULL,
    3,
    NULL,
    0);
  );
}

void loop() {
  M5.update();
}

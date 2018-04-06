#include <M5Stack.h>
#include <base64.h>

void setup()
{
  M5.begin();
  String b = base64::encode("test");
  Serial.println(b);
}

void loop()
{
  M5.update();
}

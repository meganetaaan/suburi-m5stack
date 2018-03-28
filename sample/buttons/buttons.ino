#include <M5Stack.h>
#define MY_RED 0xe8e4
#define MY_GREEN 0x2589
#define MY_BLUE 0x51d

void setup()
{
  M5.begin();
  M5.Lcd.setBrightness(100);
  M5.Lcd.setTextSize(3);

  M5.Lcd.setCursor(44, 215);
  M5.Lcd.setTextColor(MY_RED);
  M5.Lcd.printf("RED");
  M5.Lcd.drawRect(42, 212, 55, 30, MY_RED);

  M5.Lcd.setCursor(116, 215);
  M5.Lcd.setTextColor(MY_GREEN);
  M5.Lcd.printf("GREEN");
  M5.Lcd.drawRect(114, 212, 91, 30, MY_GREEN);

  M5.Lcd.setCursor(222, 215);
  M5.Lcd.setTextColor(MY_BLUE);
  M5.Lcd.printf("BLUE");
  M5.Lcd.drawRect(220, 212, 73, 30, MY_BLUE);
}

void loop()
{
  if (M5.BtnA.wasPressed())
  {
    M5.Lcd.fillCircle(160, 120, 80, MY_RED);
  }
  else if (M5.BtnB.wasPressed())
  {
    M5.Lcd.fillCircle(160, 120, 80, MY_GREEN);
  }
  else if (M5.BtnC.wasPressed())
  {
    M5.Lcd.fillCircle(160, 120, 80, MY_BLUE);
  }
  M5.update();
}

// M5Stack Text-to-Speech demo using AquesTalk pico for ESP32
// see: http://blog-yama.a-quest.com/?eid=970188
#include <M5Stack.h>
#include "driver/i2s.h"
#include "AquesTalkTTS.h"

int deg = 0;
int lastH = 0;
int16_t* gain;
int lastClosed = 0;
// #define PRIMARY_COLOR BLACK
// #define SECONDARY_COLOR 0xDFB0
#define PRIMARY_COLOR WHITE
#define SECONDARY_COLOR BLACK

void drawEye(bool open = true)
{
  M5.Lcd.fillRect(80, 80, 180, 40, SECONDARY_COLOR);
  if (open)
  {
    M5.Lcd.fillCircle(90, 93, 8, PRIMARY_COLOR);
    M5.Lcd.fillCircle(230, 96, 8, PRIMARY_COLOR);
  }
  else
  {
    M5.Lcd.fillRect(86, 93, 16, 4, PRIMARY_COLOR);
    M5.Lcd.fillRect(226, 96, 16, 4, PRIMARY_COLOR);
  }
}

void drawMouth(int h = 0)
{
  // int y = 108;
  int x = 108;
  int y = 140;
  int w = 110;
  M5.Lcd.fillRect(x + lastH, y - lastH / 2, w - 2 * lastH, lastH, SECONDARY_COLOR);
  M5.Lcd.fillRect(x + h, y - h / 2, w - 2 * h, h, PRIMARY_COLOR);
  lastH = h;
}

void clear()
{
  M5.Lcd.fillScreen(SECONDARY_COLOR);
}

void setup()
{
  gain = (int16_t*)malloc(sizeof(int16_t));
  *gain = 0;
  M5.begin();
  M5.Lcd.setBrightness(60);
  clear();
  drawEye(true);
  drawMouth(3);
  int iret = TTS.create(NULL);
}

void loop()
{
  if (deg == 11)
  {
    drawEye(false);
    lastClosed = 1;
  }
  else if (lastClosed > 0)
  {
    lastClosed++;
    if (lastClosed > 10)
    {
      drawEye(true);
      lastClosed = 0;
    }
  }
  if (*gain != 0)
  {
    uint16_t h = (*gain * 20) / 10000;
    drawMouth(h + 3);
  }
  /*
  drawMouth(17 * (sin(deg * PI / 15) + 1) + 3);
  */
  deg = (deg + 3) % 155;

  // aquestalk
  aq();
  delay(33);
  M5.update();
}

void aq()
{
  if(M5.BtnA.wasPressed())
  {
    char koe[100];
    sprintf(koe, "<NUMK VAL=%d>", random(1, 6));
    TTS.stop();
    TTS.playAsync(koe, 100, gain);
  }
  else if(M5.BtnB.wasPressed())
  {
    TTS.stop();
    TTS.playAsync("konnnichiwa.", 120, gain);
  }
  else if(M5.BtnC.wasPressed())
  {
    TTS.stop();
    TTS.playAsync("yukkuri_siteittene?", 70, gain);
  }
}

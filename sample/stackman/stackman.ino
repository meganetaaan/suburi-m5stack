// M5Stack Text-to-Speech demo using AquesTalk pico for ESP32
// see: http://blog-yama.a-quest.com/?eid=970188
#include <M5Stack.h>
#include "driver/i2s.h"
#include "AquesTalkTTS.h"

int deg = 0;
int lastH = 0;
int16_t* gain;
int lastClosed = 0;
int prime = 2;
int fontSize = 16;
bool flg = false;
bool counting = false;
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
  int y = 145;
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

  // M5.Lcd.fillCircle(10, 10, 10, WHITE);
  // aquestalk
  aq();
  delay(33);
  M5.update();
}

void aq()
{
  if(M5.BtnA.wasPressed())
  {
    sayPrime();
  }
  else if(M5.BtnB.wasPressed())
  {
    prime = 2;
    sayPrime();
  }
  else if(M5.BtnC.wasPressed())
  {
    if (counting) {
      TTS.stop();
      counting = false;
    }
    TTS.stop();
    TTS.playAsync("sosu'-wo/kazoete;otitukunnda.", 90, gain);
  }
}

int nextPrime(int n) {
  for (int i = n + 1;; i++)
  {
    if(isPrime(i)) return i;
  }
}

bool isPrime(int n)
{
  if (n == 1 || n % 2 == 0) return false;
  if (n == 2) return true;

  int lim = ceil(sqrt(n));
  Serial.printf("lim: %d, n: %d", lim, n);
  for (int i = 3; i <= lim; i += 2)
  {
    if (n % i == 0) return false;
  }
  return true;
}

int getDigit (int num) {
  if (num < 0) {
    return 0;
  }
  return log10(num) + 1;
}

void sayPrime()
{
    char koe[100];
    sprintf(koe, "<NUMK VAL=%d>", prime);
    TTS.stop();
    int speed = 100 + getDigit(prime) * 5;
    TTS.playAsync(koe, speed, gain);
    M5.Lcd.setTextColor(PRIMARY_COLOR);
    M5.Lcd.fillRect(0, 240 - fontSize, 320, fontSize, SECONDARY_COLOR);
    M5.Lcd.setTextSize(fontSize / 8);
    M5.Lcd.setCursor(0, 240 - fontSize);
    M5.Lcd.printf("%d", prime);
    prime = nextPrime(prime);
    if (prime > 20 && !flg)
    {
      prime = 202983760802;
      flg = true;
    }
}

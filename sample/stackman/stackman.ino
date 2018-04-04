// M5Stack Text-to-Speech demo using AquesTalk pico for ESP32
// see: http://blog-yama.a-quest.com/?eid=970188
#include <M5Stack.h>
#include "driver/i2s.h"
#include "AquesTalkTTS.h"
#include <avator.h>

#define PRIMARY_COLOR WHITE
#define SECONDARY_COLOR BLACK

int deg = 0;
int lastH = 0;
int16_t* gain;
int lastClosed = 0;
int16_t lastGain = 0;
int prime = 2;
int fontSize = 16;
bool flg = false;
bool counting = false;
Avator avator;

void setup()
{
  gain = (int16_t*)malloc(sizeof(int16_t));
  *gain = 0;
  M5.begin();
  M5.Lcd.setBrightness(60);
  avator.init();
  int iret = TTS.create(NULL);
}

void loop()
{
  if (deg == 11)
  {
    avator.openEye(false);
    lastClosed = 1;
  }
  else if (lastClosed > 0)
  {
    lastClosed++;
    if (lastClosed > 10)
    {
      avator.openEye(true);
      lastClosed = 0;
    }
  }
  int g = *gain;
  if (lastGain != g)
  {
    uint16_t h = min((*gain * 20) / 10000, 30) * 100 / 30;
    avator.openMouth(h);
    lastGain = g;
  }

  deg = (deg + 3) % 155;

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
    // play prime number
    char koe[100];
    sprintf(koe, "<NUMK VAL=%d>", prime);
    TTS.stop();
    int speed = 100 + getDigit(prime) * 5;
    TTS.playAsync(koe, speed, gain);

    // print prime number
    M5.Lcd.setTextColor(PRIMARY_COLOR);
    M5.Lcd.fillRect(0, 240 - fontSize, 320, fontSize, SECONDARY_COLOR);
    M5.Lcd.setTextSize(fontSize / 8);
    M5.Lcd.setCursor(0, 240 - fontSize);
    M5.Lcd.printf("%d", prime);

    // update to next prime
    prime = nextPrime(prime);
}

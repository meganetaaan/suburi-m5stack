#include <M5Stack.h>
#include <misakiUTF16.h>

#define CHAR_WIDTH 8

void printJp(int x, int y, char* chars)
{
  int n = 0;
  int maxN = (M5.Lcd.width() - x) / CHAR_WIDTH;
  byte buf[40][8];
  while(*chars && n < maxN)
  {
    chars = getFontData(&buf[n++][0], chars);
  }

  for (byte i = 0; i < 8; i++)
  {
    for (byte j = 0; j < n; j++)
    {
      for (byte k = 0; k < 8; k++)
      {
        if(bitRead(buf[j][i], 7 - k))
        {
          M5.Lcd.drawPixel(x + 8 * j + k,
          y + i, TFT_WHITE);
        }
      }
    }
  }
}
void setup()
{
  M5.begin();

  M5.Lcd.setBrightness(100);
  M5.Lcd.clear();
  printJp(0, 0, "吾輩は猫である。名前はまだ無い。");
  printJp(0, 8, "どこで生れたかとんと見当がつかぬ。");
  printJp(24, 16, "何でも薄暗いじめじめした所でニャーニャー泣いていた事だけは記憶している。");
}

void loop()
{

}

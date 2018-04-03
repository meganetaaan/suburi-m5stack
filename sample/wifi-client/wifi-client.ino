#include <M5Stack.h>
#include <WiFi.h>
#include <string.h>

String TFReadFile(String path)
{
  File file = SD.open(strToChar(path));
  String buf = "";
  if (file)
  {
    while (file.available())
    {
      buf += (char) file.read();
    }
    file.close();
  }
  return buf;
}

char* strToChar(String str)
{
  int len = str.length() + 1;
  char* buf = new char[len];
  strcpy(buf, str.c_str());
  return buf;
}

bool TFWriteFile(String path, String str)
{
  File file = SD.open(strToChar(path), FILE_WRITE);
  bool res = false;
  if (file && file.print(str))
  {
    res = true;
  }
  file.close();
  return res;
}

int cntChrs(String str, char c)
{
  int sum = 0;
  char* chars = strToChar(str);
  for(int i = 0; i < str.length(); i++)
  {
    if(chars[i] == c) sum++;
  }
  return sum;
}

bool configWifi2()
{
  char *ssid_ = strToChar("mywifi");
  char *pswd_ = strToChar("mypasswd");
  if (WiFi.begin(ssid_, pswd_))
  {
    delay(10);
    unsigned long timeout = 10000;
    unsigned long previousMillis = millis();
    while (true)
    {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis > timeout)
        break;
      if (WiFi.status() == WL_CONNECTED)
        return true;
      delay(1000);
    }
    return false;
  }
}

/*
bool configWifi()
{
  String file = TFReadFile("/system/wifi.ini");
  if (file != "")
  {
    for (int i = 0; i < cntChrs(file, '\n'); i++)
    {
      String wifi = parseString(i, '\n', file);
      wifi = wifi.substring(0, (wifi.length() - 1));
      String ssid = parseString(0, ' ', wifi);
      String pswd = parseString(1, ' ', wifi);
      char *ssid_ = strToChar(ssid);
      char *pswd_ = strToChar(pswd);
      if (WiFi.begin(ssid_, pswd_))
      {
        delay(10);
        unsigned long timeout = 10000;
        unsigned long previousMillis = millis();
        while (true)
        {
          unsigned long currentMillis = millis();
          if (currentMillis - previousMillis > timeout)
            break;
          if (WiFi.status() == WL_CONNECTED)
            return true;
          delay(1000);
        }
      }
    }
  }
}
*/

void setup()
{
  M5.begin();
  if(configWifi2())
  {
    M5.Lcd.setCursor(130, 70);
    M5.Lcd.print("success");
  }
  else
  {
    M5.Lcd.setCursor(130, 70);
    M5.Lcd.print("fail");
  }
}

void loop()
{
  M5.update();
  if(M5.BtnA.wasPressed())
  {
    M5.Lcd.clear();
    M5.Lcd.setCursor(130, 70);
    M5.Lcd.print("disconnecting");
    WiFi.disconnect();
    M5.Lcd.clear();
    M5.Lcd.setCursor(130, 70);
    M5.Lcd.print("disconnected");
  }
}

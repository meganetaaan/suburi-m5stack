#include <M5Stack.h>
#include <string.h>
#include <WiFi.h>
#include <ssl_client.h>
#include <WiFiClientSecure.h>
#include "const.h"

const char* host = "maker.ifttt.com";
const char* event = "m5stack";
const int httpsPort = 443;
WiFiClientSecure client;

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
  char *ssid_ = strToChar(ssid);
  char *pswd_ = strToChar(password);
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

void setup()
{
  M5.begin();
  while (true)
  {
    if (configWifi2())
    {
      M5.Lcd.setCursor(130, 70);
      Serial.println("success");
      M5.Lcd.print("success");
      break;
    }
    else
    {
      M5.Lcd.setCursor(130, 70);
      Serial.println("fail");
      M5.Lcd.print("fail. retry within 5 seconds...");
      delay(5000);
      M5.Lcd.clear();
    }
  }
}

void trigger(String val1)
{
  Serial.println("host connection");
  if (!client.connect(host, httpsPort)) {
    Serial.println("host connection failed");
    return;
  }
  Serial.println("host connection success");
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += secretKey;
  url += "?value1=";
  url += val1;

  String request = "GET " + url + " HTTP/1.1\r\n"
  + "Host: " + host + "\r\n"
  + "Connection: close\r\n\r\n";

  Serial.println("making request: " + request);
  client.print(request);

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
    if (line == "\r")
    {
      break;
    }
  }
}

void loop()
{
  M5.update();
  if (M5.BtnA.wasPressed())
  {
    trigger("A");
  }
  if (M5.BtnB.wasPressed())
  {
    trigger("B");
  }
  if (M5.BtnC.wasPressed())
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

#include <M5Stack.h>
#include <string.h>
#include <WiFi.h>
#include "const.h"

WiFiServer server(80);

String parseGET(String str)
{
  String tmp = "";
  for (int i = 0, j = 0; i < str.length(); i++)
  {
    if (str[i] == ' ')
      j++;
    if (j == 1 && str[i] != ' ')
      tmp += str[i];
    if (j == 2)
      break;
  }
  return tmp;
}
char* strToChar(String str)
{
  int len = str.length() + 1;
  char* buf = new char[len];
  strcpy(buf, str.c_str());
  return buf;
}

bool configWifi()
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
  M5.Lcd.setTextSize(2);
  while (true)
  {
    if (configWifi())
    {
      M5.Lcd.setCursor(130, 70);
      M5.Lcd.print("success");
      Serial.println("success");
      M5.Lcd.setCursor(130, 130);
      M5.Lcd.print(WiFi.localIP());
      server.begin();
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

void loop() {
  String currentString = "";
  bool readyResponse = false;
  WiFiClient client = server.available();
  while (client.connected())
  {
    if (client.available())
    {
      char c = client.read();
      if ((c != '\r') && (c != '\n'))
        currentString += c;
      else
        readyResponse = true;

      if (readyResponse)
      {
        String GET = parseGET(currentString);
        Serial.print(GET);
        String content = "Hello World!";
        client.flush();
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println();
        client.println("<html>");
        client.println("<head>");
        client.println("<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>");
        client.println("<title>M5Stack</title>");
        client.println("<link rel=\"icon\" type=\"image/x-icon\" href=\"http://m5stack.com/favicon.ico\">");
        client.println("</head>");
        client.println("<body>");
        client.println(content);
        client.println("</body>");
        client.print("</html>");
        client.println();
        client.println();
        readyResponse = false;
        currentString = "";
        client.stop();
      }
    }
  }
}

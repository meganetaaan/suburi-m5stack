// M5Stack Text-to-Speech demo using AquesTalk pico for ESP32
// see: http://blog-yama.a-quest.com/?eid=970188
#include <M5Stack.h>
#include "driver/i2s.h"
#include "aquestalk.h"

#define LEN_FRAME 32
uint32_t workbuf[AQ_SIZE_WORKBUF];
int deg = 0;
int lastH = 0;
bool lastClosed = false;

void drawEye(bool open = true)
{
  M5.Lcd.fillRect(80, 80, 180, 40, BLACK);
  if (open)
  {
    M5.Lcd.fillCircle(90, 93, 8, WHITE);
    M5.Lcd.fillCircle(230, 96, 8, WHITE);
  }
  else
  {
    M5.Lcd.fillRect(86, 93, 16, 4, WHITE);
    M5.Lcd.fillRect(226, 96, 16, 4, WHITE);
  }
}

void drawMouth(int h = 0)
{
  // M5.Lcd.drawCircleHelper(200, 120, 20, 6, WHITE);
  M5.Lcd.fillRect(108 + lastH, 170 - lastH / 2, 110 - 2 * lastH, lastH, BLACK);
  M5.Lcd.fillRect(108 + h, 170 - h / 2, 110 - 2 * h, h, WHITE);
  lastH = h;
}

void clear()
{
  M5.Lcd.fillScreen(BLACK);
}

void setup()
{
  M5.begin();
  M5.Lcd.setBrightness(60);
  clear();
  drawEye(true);
  drawMouth(3);
  setupAq();
}

void loop()
{
  if (deg == 11)
  {
    drawEye(false);
    lastClosed = true;
  }
  else if (lastClosed)
  {
    drawEye(true);
    lastClosed = false;
  }
  // drawMouth(17 * (sin(deg * PI / 15) + 1) + 3);
  deg = (deg + 6) % 31;

  // aquestalk
  aq();
  delay(200);
  M5.update();
}

void setupAq()
{
  int iret;
  Serial.begin(115200);

  Serial.println("Initialize AquesTalk");
  iret = CAqTkPicoF_Init(workbuf, LEN_FRAME, NULL); // set your license key
  if (iret)
  {
    Serial.println("ERR:CAqTkPicoF_Init");
  }
}

void aq()
{
  if (M5.BtnA.isPressed())
  {
    say("yukkuri_siteittene?");
  }
  else if (M5.BtnB.wasPressed())
  {
    say("haro-wa-rudo.");
  }
}

void say(const char *koe)
{
  drawMouth(20);
  DAC_Create();
  Play(koe);
  DAC_Release();
  drawMouth(3);
}

void Play(const char *koe)
{
  Serial.print("Play:");
  Serial.println(koe);

  int iret = CAqTkPicoF_SetKoe((const uint8_t *)koe, 100, 0xffffU);
  if (iret)
    Serial.println("ERR:CAqTkPicoF_SetKoe");

  for (;;)
  {
    int16_t wav[LEN_FRAME];
    uint16_t len;
    iret = CAqTkPicoF_SyntheFrame(wav, &len);
    if (iret)
      break; // EOD

    DAC_Write((int)len, wav);
  }
}

////////////////////////////////
//i2s configuration
const int i2s_num = 0; // i2s port number
i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
    .sample_rate = 24000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = 0,
    .dma_buf_count = 4,
    .dma_buf_len = 384,
    .use_apll = 0};

void DAC_Create()
{
  AqResample_Reset();

  i2s_driver_install((i2s_port_t)i2s_num, &i2s_config, 0, NULL);
  i2s_set_pin((i2s_port_t)i2s_num, NULL);
}

void DAC_Release()
{
  i2s_driver_uninstall((i2s_port_t)i2s_num); //stop & destroy i2s driver
}

// upsampling & write to I2S
int DAC_Write(int len, int16_t *wav)
{
  int i;
  for (i = 0; i < len; i++)
  {
    // upsampling x3
    int16_t wav3[3];
    AqResample_Conv(wav[i], wav3);

    // write to I2S DMA buffer
    for (int k = 0; k < 3; k++)
    {
      uint16_t sample[2];
      uint16_t us = ((uint16_t)wav3[k]) ^ 0x8000U; // signed -> unsigned data for internal DAC
      sample[0] = sample[1] = us;                  // mono -> stereo
      int iret = i2s_push_sample((i2s_port_t)i2s_num, (const char *)sample, 100);
      if (iret < 0)
        return iret; // -1:ESP_FAIL
      if (iret == 0)
        break; //  0:TIMEOUT
    }
  }
  return i;
}

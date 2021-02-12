// /***********************************************************************
//  * @file 	:	main.cpp
//  * @author	:	ch
//  * @brief	:
//  * @version:	v1.0
//  * @Copyright (C)  2020-12-12  .cdWFVCEL. all right reserved
// ***********************************************************************/

// /* ����ͷ�ļ� -------------------------------------------------------------------*/
#include <Arduino.h>
#include "WiFi.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "soc/soc.h"          // Disable brownour problems
#include "soc/rtc_cntl_reg.h" // Disable brownour problems
#include "driver/rtc_io.h"
#include <ESPAsyncWebServer.h>
#include <StringArray.h>
#include <SPIFFS.h>
#include <FS.h>
#include "ArduinoJson.h"

#include "ch/bsp.h"
#include "main.h"
#include "json_analysis.h"
#include "PubSubClient.h"
#include "_wifi.h"
#include "mqtt_client.h"
#include "camera.h"

// /* �궨�� -----------------------------------------------------------------------*/
// /* ���Ͷ��� ---------------------------------------------------------------------*/
#define LED 33
// /* ˽�б��� ---------------------------------------------------------------------*/
// /* ��չ���� ---------------------------------------------------------------------*/
// /* ˽�к������� -----------------------------------------------------------------*/
// /* �������� ---------------------------------------------------------------------*/
// /* ����ʵ�� ---------------------------------------------------------------------*/
void setup()
{
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial.println("run strat...");

  _WIFI_SetupWifi();
  MQTTCLIENT_Init();
  JA_Init();
  CAMERA_Init();
}

#define LED_TOGGLE_INTERVAL_PERIOD 100 //ָʾ����˸����

u32 led_c = 0, rev_c = 0;
void loop()
{
  JA_Scan();

  //�����û�ж���
  if (!WiFi.isConnected()) //�ȿ�WIFI�Ƿ�������
  {
    _WIFI_SetupWifi();
  }
  else //���WIFI������,
  {
    if (!mqttClient.connected()) //�ٿ�mqtt������û
    {
      Serial.println("mqtt disconnected!Try reconnect now...");
      Serial.println(mqttClient.state());
      MQTTCLIENT_ClientReconnect();
    }
  }

  // //mqtt�ͻ��˼���
  mqttClient.loop();

  if (picture.length == 0)
  {
    if (CAMERA_CapturePhotoSaveSpiffs() == 0)
    {
      File file;
      File f_pic = SPIFFS.open(FILE_PHOTO);
      picture.length = f_pic.size();
      f_pic.read((u8 *)picture.buf, picture.length);
      
    }
  }

  if (millis() - led_c > LED_TOGGLE_INTERVAL_PERIOD)
  {
    digitalRead(LED) == HIGH ? digitalWrite(LED, LOW) : digitalWrite(LED, HIGH);
    led_c = millis();
  }
}
// //main.cpp
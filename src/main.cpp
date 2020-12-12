/***********************************************************************
 * @file 	:	main.cpp
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2020-12-12  .cdWFVCEL. all right reserved
***********************************************************************/

/* 包含头文件 -------------------------------------------------------------------*/
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

/* 宏定义 -----------------------------------------------------------------------*/
/* 类型定义 ---------------------------------------------------------------------*/
#define LED 33
/* 私有变量 ---------------------------------------------------------------------*/
/* 扩展变量 ---------------------------------------------------------------------*/
/* 私有函数声明 -----------------------------------------------------------------*/
/* 函数声明 ---------------------------------------------------------------------*/
/* 函数实现 ---------------------------------------------------------------------*/
void setup()
{
    pinMode(LED, OUTPUT);
    Serial.begin(9600);
    Serial.println("run strat...");

    JA_Init();
}

#define LED_TOGGLE_INTERVAL_PERIOD 100      //指示灯闪烁周期

u32 led_c = 0, rev_c = 0;



void loop()
{
    JA_Scan();
    
    if (millis() - led_c > LED_TOGGLE_INTERVAL_PERIOD)
    {
        digitalRead(LED) == HIGH ? digitalWrite(LED, LOW) : digitalWrite(LED, HIGH);
        led_c = millis();
    }
}
//main.cpp
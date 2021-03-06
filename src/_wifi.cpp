/***********************************************************************
 * @file 	:	_wifi.cpp
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2020-12-13  .cdWFVCEL. all right reserved
***********************************************************************/

/* 包含头文件 -------------------------------------------------------------------*/
#include "_wifi.h"
/* 宏定义 -----------------------------------------------------------------------*/
/* 类型定义 ---------------------------------------------------------------------*/
/* 私有变量 ---------------------------------------------------------------------*/
/* 扩展变量 ---------------------------------------------------------------------*/
/* 私有函数声明 -----------------------------------------------------------------*/
/* 函数声明 ---------------------------------------------------------------------*/
/* 函数实现 ---------------------------------------------------------------------*/
void _WIFI_SetupWifi()
{
    delay(10);
    Serial.println("连接WIFI");
    WiFi.begin(WIFI_SSID, WIFI_PASSWD);
    while (!WiFi.isConnected())
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("OK");
    Serial.println("Wifi连接成功");
}
//_wifi.cpp
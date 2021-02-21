/***********************************************************************
 * @file 	:	json_analysis.cpp
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2020-12-12  .cdWFVCEL. all right reserved
***********************************************************************/

/* 包含头文件 -------------------------------------------------------------------*/
#include "json_analysis.h"
/* 宏定义 -----------------------------------------------------------------------*/
/* 类型定义 ---------------------------------------------------------------------*/

/* 私有变量 ---------------------------------------------------------------------*/
char payloadBuf[JSONA_BUFSIZE] = {0};
bool payloadRecved = false;
/* 扩展变量 ---------------------------------------------------------------------*/
bool JSON_Analysis::convOK = false;

uint8_t JSON_Analysis::payloadBuf[JSONA_BUFSIZE] = {0};
/* 私有函数声明 -----------------------------------------------------------------*/
/* 函数声明 ---------------------------------------------------------------------*/
/* 函数实现 ---------------------------------------------------------------------*/

//json_analysis.cpp
// void JSON_Analysis::begin(HardwareSerial s)
// {
//     _serial = s;
//     _serial.begin(JSONA_BADU_RATE);
// }

void JSON_Analysis::loop()
{
    if (JSONA_SERIAL.available())
    {
        String recvStr = JSONA_SERIAL.readStringUntil('\n');
        // Serial.println(recvStr);
        if (recvStr.length())
        {
            DeserializationError error = deserializeJson(_root, recvStr);
            if (error)
            {
                // Serial.print(F("receviced data is not complete , deserializeJson() failed: "));
                Serial.println(error.f_str());
                convOK = false;
            }
            else
            {
                if (checksum() == true)
                {
                    memset(payloadBuf, 0, sizeof(payloadBuf));
                    serializeJson(_root["payload"], payloadBuf, sizeof(payloadBuf));
                    convOK = true;
                    Serial.print("JSON analysis succeed:");
                    Serial.println((char *)payloadBuf);
                }
                else
                {
                    Serial.println("checksum failed!!!");
                    convOK = false;
                }
            }
        }
    }
}

bool JSON_Analysis::checksum()
{
    static uint8_t tmpBuf[JSONA_BUFSIZE] = {0};
    memset(tmpBuf, 0, sizeof(tmpBuf));

    u16 checksum1 = 0, checksum2 = 0;

    checksum1 = _root["checksum"];
    serializeJson(_root["payload"], tmpBuf, sizeof(tmpBuf));

    for (int i = 0; i < strlen((char *)tmpBuf); i++)
    {
        checksum2 += tmpBuf[i];
    }

    if (checksum1 == checksum2)
    {
        memset(tmpBuf, 0, sizeof(tmpBuf));
        serializeJson(_root, tmpBuf, sizeof(tmpBuf));
        return true;
    }
    else
    {
        return false;
    }
}

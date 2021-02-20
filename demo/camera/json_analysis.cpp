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
class SerialReceiveBuffer
{
#define SBUF_SIZE 1024
public:
    u8 buf[SBUF_SIZE] = {0};
    u16 i = 0;
    bool revd_f = false; //接收完成
};

/* 私有变量 ---------------------------------------------------------------------*/
SerialReceiveBuffer sRevBuf;
StaticJsonDocument<1024> root;
char tmpBuf[1024] = {0};
char payloadBuf[1024] = {0};
/* 扩展变量 ---------------------------------------------------------------------*/
/* 私有函数声明 -----------------------------------------------------------------*/
/* 函数声明 ---------------------------------------------------------------------*/
/* 函数实现 ---------------------------------------------------------------------*/
void JA_Init()
{
}

void JA_Scan()
{
    if (JA_SERIAL.available())
    {
        if (sRevBuf.revd_f == false)
        {
            sRevBuf.buf[sRevBuf.i++] = JA_SERIAL.read();
            if (sRevBuf.i == SBUF_SIZE)
            {
                sRevBuf.i = 0;
            }
            if (sRevBuf.buf[sRevBuf.i - 1] == '\n')
            {
                sRevBuf.revd_f = true;
            }
        }
        // Serial.printf("%c", Serial.read());
    }

    if (sRevBuf.revd_f)
    {
        // Serial.printf((char *)sRevBuf.buf);
        sRevBuf.revd_f = false, sRevBuf.i = 0;

        // Deserialize the JSON document
        DeserializationError error = deserializeJson(root, (char *)sRevBuf.buf);

        // Test if parsing succeeds.
        if (error)
        {
            Serial.print(F("receviced data is not complete , deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }
        else
        {
            u16 checksum1 = 0, checksum2 = 0;
            checksum1 = root["checksum"];
            memset(tmpBuf, 0, sizeof(tmpBuf));
            serializeJson(root["payload"], tmpBuf, sizeof(tmpBuf));

            for (int i = 0; i < strlen((char *)tmpBuf); i++)
            {
                checksum2 += tmpBuf[i];
            }
            if (checksum1 == checksum2)
            {
                memset(tmpBuf, 0, sizeof(tmpBuf));
                serializeJson(root, tmpBuf, sizeof(tmpBuf));
                Serial.print("receviced data : ");
                Serial.println(tmpBuf);

                memset(payloadBuf, 0, sizeof(payloadBuf));
                serializeJson(root["payload"], payloadBuf, sizeof(payloadBuf));
            }
            else
            {
                Serial.printf("receviced data checksum failed ,c1=%d c2=%d \r\n", checksum1, checksum2);
            }
        }
    }
}
//json_analysis.cpp
/***********************************************************************
 * @file 	:	json_analysis.cpp
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2020-12-12  .cdWFVCEL. all right reserved
***********************************************************************/

/* ����ͷ�ļ� -------------------------------------------------------------------*/
#include "json_analysis.h"
/* �궨�� -----------------------------------------------------------------------*/
/* ���Ͷ��� ---------------------------------------------------------------------*/

/* ˽�б��� ---------------------------------------------------------------------*/
SerialReceiveBuffer sRevBuf;
StaticJsonDocument<1024> root;
char tmpBuf[1024] = {0};
char payloadBuf[1024] = {0};
/* ��չ���� ---------------------------------------------------------------------*/
/* ˽�к������� -----------------------------------------------------------------*/
/* �������� ---------------------------------------------------------------------*/
/* ����ʵ�� ---------------------------------------------------------------------*/
void JA_Init()
{
}

void JA_Scan()
{
    if (JA_SERIAL.available())
    {
        if (sRevBuf.revd_f == false)
        {
            sRevBuf.buf[sRevBuf.rev_c++] = JA_SERIAL.read();
            if (sRevBuf.rev_c == SBUF_SIZE)
            {
                sRevBuf.rev_c = 0;
            }
            if (sRevBuf.buf[sRevBuf.rev_c - 1] == '\n')
            {
                sRevBuf.revd_f = true;
            }
        }
        // Serial.printf("%c", Serial.read());
    }

    if (sRevBuf.revd_f)
    {
        // Serial.printf((char *)sRevBuf.buf);
        sRevBuf.revd_f = false;
        // sRevBuf.rev_c = 0;

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
            // u16 checksum1 = 0, checksum2 = 0;
            // checksum1 = root["checksum"];
            // memset(tmpBuf, 0, sizeof(tmpBuf));
            // serializeJson(root["payload"], tmpBuf, sizeof(tmpBuf));

            // for (int i = 0; i < strlen((char *)tmpBuf); i++)
            // {
            //     checksum2 += tmpBuf[i];
            // }
            // if (checksum1 == checksum2)
            // {
            //     memset(tmpBuf, 0, sizeof(tmpBuf));
            //     serializeJson(root, tmpBuf, sizeof(tmpBuf));
            //     Serial.print("receviced data : ");
            //     Serial.println(tmpBuf);

            //     memset(payloadBuf, 0, sizeof(payloadBuf));
            //     serializeJson(root["payload"], payloadBuf, sizeof(payloadBuf));
            // }
            // else
            // {
            //     Serial.printf("receviced data checksum failed ,c1=%d c2=%d \r\n", checksum1, checksum2);
            // }
        }
    }
}
//json_analysis.cpp
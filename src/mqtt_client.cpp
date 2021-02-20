/***********************************************************************
 * @file 	:	client.cpp
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2020-12-13  .cdWFVCEL. all right reserved
***********************************************************************/

/* ����ͷ�ļ� -------------------------------------------------------------------*/
#include "mqtt_client.h"
#include <ArduinoJson.h>
#include <aliyun_mqtt.h>
#include "WiFi.h"
#include "Ticker.h"
#include "main.h"
#include "json_analysis.h"

#include "esp_task_wdt.h"
// #include "camera.h"
/* �궨�� -----------------------------------------------------------------------*/
/* ���Ͷ��� ---------------------------------------------------------------------*/
/* ˽�б��� ---------------------------------------------------------------------*/
int postMsgId = 0;    //��¼�Ѿ�post�˶�����
Ticker tim1;          //�����ʱ����Ϊ��ÿ5���ϴ�һ������
WiFiClient espClient; //�����������ӿͻ���
PubSubClient mqttClient(espClient); //ͨ������ͻ������Ӵ���mqtt���ӿͻ���
// MyPubSubClient mqttClient(espClient);
u8 *MyPubSubClient::mqttBuf = NULL;
/* ��չ���� ---------------------------------------------------------------------*/
/* ˽�к������� -----------------------------------------------------------------*/
/* �������� ---------------------------------------------------------------------*/
void callback(char *topic, byte *payload, unsigned int length);
void mqttPublish();
/* ����ʵ�� ---------------------------------------------------------------------*/

MyPubSubClient::MyPubSubClient(Client &client) : PubSubClient(client)
{
}

boolean MyPubSubClient::publish(const char *topic, const uint8_t *payload, u32 plength)
{
    Serial.println("my publish");
    if (connected())
    {
        this->buffer = MyPubSubClient::mqttBuf;
        this->bufferSize = MyPubSubClient::MQTTBUF_SIZE;

        if (this->bufferSize < MQTT_MAX_HEADER_SIZE + 2 + strnlen(topic, this->bufferSize) + plength)
        {
            // Too long
            return false;
        }

        // Leave room in the buffer for header and variable length field
        uint16_t length = MQTT_MAX_HEADER_SIZE;
        length = writeString(topic, this->buffer, length);

        // Add payload
        uint16_t i;
        for (i = 0; i < plength; i++)
        {
            this->buffer[length++] = payload[i];
        }

        // Write the header
        uint8_t header = MQTTPUBLISH;
        if (false)
        {
            header |= 1;
        }
        return write(header, this->buffer, length - MQTT_MAX_HEADER_SIZE);
    }
    return false;
}

void MQTTCLIENT_Init()
{
    // pinMode(LED_B, OUTPUT);

    if (connectAliyunMQTT(mqttClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET))
    {
        Serial.println("MQTT���������ӳɹ�!");
    };
    mqttClient.setCallback(callback);        //���յ�set����ʱ�Ļص�(�����·��ص�)
    tim1.attach(REPORT_PERIOD, mqttPublish); //����ÿ5�뷢��һ����Ϣ
}

//��������, ����ͻ��˶���,����ͨ���˺�������
void MQTTCLIENT_ClientReconnect()
{
    while (!mqttClient.connected()) //�������ͻ���
    {
        Serial.println("reconnect MQTT...");
        if (connectAliyunMQTT(mqttClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.println("failed");
            Serial.println(mqttClient.state());
            Serial.printf("try again in 5 sec");
            delay(5000);
        }
    }
}

// char param[1024];
// char jsonBuf[1024 + 128];
//mqtt����post��Ϣ(�ϴ�����)
void mqttPublish()
{
    if (mqttClient.connected())
    {
        mqttPulish_f = true;
        // // sprintf(param, "{\"LightSwitch\":%d}", digitalRead(LED_B)); //���ǰ�Ҫ�ϴ�������д��param��
        // strcpy(param, payloadBuf);
        // postMsgId += 1;
        // sprintf(jsonBuf, ALINK_BODY_FORMAT, postMsgId, ALINK_METHOD_PROP_POST, param);
        // //�ٴ�mqtt�ͻ����з���post��Ϣ
        // if (mqttClient.publish(ALINK_TOPIC_PROP_POST, jsonBuf))
        // {
        //     // Serial.print("Post message to cloud: ");
        //     // Serial.println(jsonBuf);

        //     Serial.printf("publish to <%s> is succeed! \r\n", ALINK_TOPIC_PROP_POST);
        // }
        // else
        // {
        //     Serial.printf("publish to <%s> is failed! \r\n", ALINK_TOPIC_PROP_POST);
        // }
        // char *sendFrameBuf = (char *)ps_malloc(PHOTO_FILE_SIZE);
        // // static char sendFrameBuf[PHOTO_FILE_SIZE] = {0};

        // if (photoFname_a.size() > 0)
        // {
        //     capturePhoto_lock = false;
        //     Serial.printf("send %d photo to server \r\n", photoFname_a.size());

        //     if (sendFrameBuf != NULL)
        //     {
        //         memset(sendFrameBuf, 0, PHOTO_FILE_SIZE);
        //         std::vector<String>::iterator it;
        //         for (it = photoFname_a.begin(); it != photoFname_a.end(); it++)
        //         {
        //             Serial.printf("open file %s \r\n", it->c_str());
        //             FILE *file = fopen(it->c_str(), "r");
        //             if (file != NULL)
        //             {
        //                 u32 readLen = fread(sendFrameBuf, 1, PHOTO_FILE_SIZE, file);

        //                 // for (int i = 0; i < readLen; i++)
        //                 // {
        //                 //     Serial.printf("%02x ", sendFrameBuf[i]);
        //                 // }
        //                 // Serial.printf("\r\nsend photo %s ,size is %d \r\n", it->c_str(), readLen);

        // if (mqttClient.publish(ALINK_TOPIC_PICTURE, (u8 *)sendFrameBuf, (u32)readLen))
        //                 {
        //                     Serial.printf("publish to <%s> is succeed, picture size is %d !\r\n", ALINK_TOPIC_PICTURE, readLen);
        //                 }
        //                 else
        //                 {
        //                     Serial.printf("publish to <%s> is failed! \r\n", ALINK_TOPIC_PICTURE);
        //                 }
        //                 remove(it->c_str());
        //                 // photoFname_a.erase(it); //send it,erase it.
        //             }
        //             else
        //             {
        //                 Serial.printf("could not open file %s \r\n", it->c_str());
        //             }
        //             fclose(file);
        //         }
        //     }
        //     else
        //     {
        //         Serial.println("memery malloc failed");
        //     }
        //     photoFname_a.clear();

        //     capturePhoto_lock = true;
        // }

        // if (videoFname_a.size() > 0)
        // {
        //     Serial.printf("send %d video to server \r\n", videoFname_a.size());

        //     std::vector<String>::iterator it;
        //     for (it = videoFname_a.begin(); it != videoFname_a.end(); it++)
        //     {
        //         FILE *file = fopen(it->c_str(), "r");
        //         u32 totalByte = 0;
        //         if (file != NULL)
        //         {
        //             for (int i = 0;; i += VIDEO_FRAME_SIZE)
        //             {
        //                 if (feof(file))
        //                 {
        //                     break;
        //                 }
        //                 u32 readLen = fread(sendFrameBuf, 1, VIDEO_FRAME_SIZE, file);

        //                 for (int i = 0; i < readLen; i++)
        //                 {
        //                     Serial.printf("%02x ", sendFrameBuf[i]);
        //                 }
        //                 memset(sendFrameBuf, 0, VIDEO_FRAME_SIZE);
        //                 totalByte += readLen;
        //                 // esp_task_wdt_feed();
        //                 // feedLoopWDT();
        //                 // esp_task_wdt_reset();
        //                 vTaskDelay(10);
        //             }
        //             Serial.printf("\r\nsend video %s ,size is %d byte \r\n", it->c_str(), totalByte);
        //             free(sendFrameBuf);
        //             remove(it->c_str());
        //         }
        //         else
        //         {
        //             Serial.printf("could not open file %s \r\n", it->c_str());
        //         }
        //         fclose(file);
        //     }

        //     videoFname_a.clear();
        //     recordVideo_lock = true; //allow start record video
        // }
        // free(sendFrameBuf);
    }
}

//�յ�set����������·�ʱ�Ļص�����,(��������)
void callback(char *topic, byte *payload, unsigned int length)
{
    if (strstr(topic, ALINK_TOPIC_PROP_SET))
    //����յ�������������ַ���ALINK_TOPIC_PROP_SET(Ҳ�����յ�/sys/a17lGhkKwXs/esp32LightHome/thing/service/property/set����)
    {
        Serial.println("�յ��·�����������:");
        Serial.println(topic);
        Serial.println("�·���������:");
        payload[length] = '\0'; //Ϊpayload���һ��������,��ֹSerial.println()������
        Serial.println((char *)payload);

        //���������յ���json�ַ����Ľ���
        DynamicJsonDocument doc(100);
        DeserializationError error = deserializeJson(doc, payload);
        if (error)
        {
            Serial.println("parse json failed");
            return;
        }
        JsonObject setAlinkMsgObj = doc.as<JsonObject>();
        serializeJsonPretty(setAlinkMsgObj, Serial);
        Serial.println();

        //������һ�����С�߼�
        int lightSwitch = setAlinkMsgObj["params"]["LightSwitch"];
        // digitalWrite(LED_B, lightSwitch);
        mqttPublish(); //���ڽ�����Ӧ�ÿ���Ҫ��ȡ�Ƶ�״̬,���������﷢��һ��
    }
}
//client.cpp
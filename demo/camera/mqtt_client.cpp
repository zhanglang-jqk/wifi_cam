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
#include "json_analysis.h"
#include "camera.h"
/* �궨�� -----------------------------------------------------------------------*/
/* ���Ͷ��� ---------------------------------------------------------------------*/
/* ˽�б��� ---------------------------------------------------------------------*/
int postMsgId = 0;                  //��¼�Ѿ�post�˶�����
Ticker tim1;                        //�����ʱ����Ϊ��ÿ5���ϴ�һ������
WiFiClient espClient;               //�����������ӿͻ���
PubSubClient mqttClient(espClient); //ͨ������ͻ������Ӵ���mqtt���ӿͻ���
/* ��չ���� ---------------------------------------------------------------------*/
/* ˽�к������� -----------------------------------------------------------------*/
/* �������� ---------------------------------------------------------------------*/
void callback(char *topic, byte *payload, unsigned int length);
void mqttPublish();
/* ����ʵ�� ---------------------------------------------------------------------*/

void MQTTCLIENT_Init()
{
    pinMode(LED_B, OUTPUT);

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

char param[1024];
char jsonBuf[1024 + 128];
//mqtt����post��Ϣ(�ϴ�����)
void mqttPublish()
{
    if (mqttClient.connected())
    {
        // sprintf(param, "{\"LightSwitch\":%d}", digitalRead(LED_B)); //���ǰ�Ҫ�ϴ�������д��param��
        strcpy(param, payloadBuf);
        postMsgId += 1;
        sprintf(jsonBuf, ALINK_BODY_FORMAT, postMsgId, ALINK_METHOD_PROP_POST, param);
        //�ٴ�mqtt�ͻ����з���post��Ϣ
        if (mqttClient.publish(ALINK_TOPIC_PROP_POST, jsonBuf))
        {
            // Serial.print("Post message to cloud: ");
            // Serial.println(jsonBuf);

            Serial.printf("publish to <%s> is succeed! \r\n", ALINK_TOPIC_PROP_POST);
        }
        else
        {
            Serial.printf("publish to <%s> is failed! \r\n", ALINK_TOPIC_PROP_POST);
        }

        if (picture.length != 0)
        {
            if (mqttClient.publish(ALINK_TOPIC_PICTURE, (u8*)picture.buf, (u32)picture.length))
            {
                Serial.printf("publish to <%s> is succeed, picture size is %d !\r\n", ALINK_TOPIC_PICTURE, picture.length);
                memset(picture.buf, 0, sizeof(picture.buf));
                picture.length = 0;
            }
            else
            {
                Serial.printf("publish to <%s> is failed! \r\n", ALINK_TOPIC_PICTURE);
            }
        }
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
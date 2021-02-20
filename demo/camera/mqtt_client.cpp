/***********************************************************************
 * @file 	:	client.cpp
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2020-12-13  .cdWFVCEL. all right reserved
***********************************************************************/

/* 包含头文件 -------------------------------------------------------------------*/
#include "mqtt_client.h"
#include <ArduinoJson.h>
#include <aliyun_mqtt.h>
#include "WiFi.h"
#include "Ticker.h"
#include "json_analysis.h"
#include "camera.h"
/* 宏定义 -----------------------------------------------------------------------*/
/* 类型定义 ---------------------------------------------------------------------*/
/* 私有变量 ---------------------------------------------------------------------*/
int postMsgId = 0;                  //记录已经post了多少条
Ticker tim1;                        //这个定时器是为了每5秒上传一次数据
WiFiClient espClient;               //创建网络连接客户端
PubSubClient mqttClient(espClient); //通过网络客户端连接创建mqtt连接客户端
/* 扩展变量 ---------------------------------------------------------------------*/
/* 私有函数声明 -----------------------------------------------------------------*/
/* 函数声明 ---------------------------------------------------------------------*/
void callback(char *topic, byte *payload, unsigned int length);
void mqttPublish();
/* 函数实现 ---------------------------------------------------------------------*/

void MQTTCLIENT_Init()
{
    pinMode(LED_B, OUTPUT);

    if (connectAliyunMQTT(mqttClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET))
    {
        Serial.println("MQTT服务器连接成功!");
    };
    mqttClient.setCallback(callback);        //绑定收到set主题时的回调(命令下发回调)
    tim1.attach(REPORT_PERIOD, mqttPublish); //启动每5秒发布一次消息
}

//重连函数, 如果客户端断线,可以通过此函数重连
void MQTTCLIENT_ClientReconnect()
{
    while (!mqttClient.connected()) //再重连客户端
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
//mqtt发布post消息(上传数据)
void mqttPublish()
{
    if (mqttClient.connected())
    {
        // sprintf(param, "{\"LightSwitch\":%d}", digitalRead(LED_B)); //我们把要上传的数据写在param里
        strcpy(param, payloadBuf);
        postMsgId += 1;
        sprintf(jsonBuf, ALINK_BODY_FORMAT, postMsgId, ALINK_METHOD_PROP_POST, param);
        //再从mqtt客户端中发布post消息
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

//收到set主题的命令下发时的回调函数,(接收命令)
void callback(char *topic, byte *payload, unsigned int length)
{
    if (strstr(topic, ALINK_TOPIC_PROP_SET))
    //如果收到的主题里包含字符串ALINK_TOPIC_PROP_SET(也就是收到/sys/a17lGhkKwXs/esp32LightHome/thing/service/property/set主题)
    {
        Serial.println("收到下发的命令主题:");
        Serial.println(topic);
        Serial.println("下发的内容是:");
        payload[length] = '\0'; //为payload添加一个结束附,防止Serial.println()读过了
        Serial.println((char *)payload);

        //接下来是收到的json字符串的解析
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

        //这里是一个点灯小逻辑
        int lightSwitch = setAlinkMsgObj["params"]["LightSwitch"];
        // digitalWrite(LED_B, lightSwitch);
        mqttPublish(); //由于将来做应用可能要获取灯的状态,所以在这里发布一下
    }
}
//client.cpp
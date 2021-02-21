/***********************************************************************
 * @file client.h
 * CLIENT
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2020-12-13  .cdWFVCEL. all right reserved
***********************************************************************/

#ifndef __MQTTCLIENT_H_
#define __MQTTCLIENT_H_
/* 包含头文件 ------------------------------------------------------------------*/
#include "ch/bsp.h"
#include "PubSubClient.h"

/* 宏定义 ----------------------------------------------------------------------*/
#define PRODUCT_KEY "a1ATmRZUn7U"                        //产品ID
#define DEVICE_NAME "k5l0KMex1DmcG0PGzKjC"               //设备名
#define DEVICE_SECRET "a776b02ea77adc5d591a0d381a32943a" //设备key

//设备下发命令的set主题
#define ALINK_TOPIC_PROP_SET "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/service/property/set"
//设备上传数据的post主题
#define ALINK_TOPIC_PROP_POST "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post"
//设备post上传数据要用到一个json字符串, 这个是拼接postJson用到的一个字符串
#define ALINK_METHOD_PROP_POST "thing.event.property.post"
//这是post上传数据使用的模板
#define ALINK_BODY_FORMAT "{\"id\":\"%u\",\"version\":\"1.0\",\"method\":\"%s\",\"params\":%s}"

//照片上传 topic
#define ALINK_TOPIC_PICTURE "/" PRODUCT_KEY "/" DEVICE_NAME "/user/picture"

#define LED_B 2            //定义LED灯的引脚
#define REPORT_PERIOD (10) //上报周期
/* 类型定义 --------------------------------------------------------------------*/
class MyPubSubClient : public PubSubClient
{
public:
    enum
    {
        // MQTTBUF_SIZE = (1 * 1024 * 1024 + 1024)
        MQTTBUF_SIZE = (2 * 1024)
    };
    MyPubSubClient(Client &client);
    boolean publish(const char *topic, const uint8_t *payload, u32 plength);

    static u8 *mqttBuf;
};
/* 变量声明 --------------------------------------------------------------------*/
extern PubSubClient mqttClient;
// extern MyPubSubClient mqttClient;
/* 函数声明 --------------------------------------------------------------------*/
void MQTTCLIENT_Init();
void MQTTCLIENT_ClientReconnect();
#endif // __CLIENT_H_

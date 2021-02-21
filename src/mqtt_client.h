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
/* ����ͷ�ļ� ------------------------------------------------------------------*/
#include "ch/bsp.h"
#include "PubSubClient.h"

/* �궨�� ----------------------------------------------------------------------*/
#define PRODUCT_KEY "a1ATmRZUn7U"                        //��ƷID
#define DEVICE_NAME "k5l0KMex1DmcG0PGzKjC"               //�豸��
#define DEVICE_SECRET "a776b02ea77adc5d591a0d381a32943a" //�豸key

//�豸�·������set����
#define ALINK_TOPIC_PROP_SET "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/service/property/set"
//�豸�ϴ����ݵ�post����
#define ALINK_TOPIC_PROP_POST "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post"
//�豸post�ϴ�����Ҫ�õ�һ��json�ַ���, �����ƴ��postJson�õ���һ���ַ���
#define ALINK_METHOD_PROP_POST "thing.event.property.post"
//����post�ϴ�����ʹ�õ�ģ��
#define ALINK_BODY_FORMAT "{\"id\":\"%u\",\"version\":\"1.0\",\"method\":\"%s\",\"params\":%s}"

//��Ƭ�ϴ� topic
#define ALINK_TOPIC_PICTURE "/" PRODUCT_KEY "/" DEVICE_NAME "/user/picture"

#define LED_B 2            //����LED�Ƶ�����
#define REPORT_PERIOD (10) //�ϱ�����
/* ���Ͷ��� --------------------------------------------------------------------*/
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
/* �������� --------------------------------------------------------------------*/
extern PubSubClient mqttClient;
// extern MyPubSubClient mqttClient;
/* �������� --------------------------------------------------------------------*/
void MQTTCLIENT_Init();
void MQTTCLIENT_ClientReconnect();
#endif // __CLIENT_H_

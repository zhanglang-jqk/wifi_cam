/***********************************************************************
 * @file json_analysis.h
 * JSON_ANALYSIS
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2020-12-12  .cdWFVCEL. all right reserved
***********************************************************************/

#ifndef __JSON_ANALYSIS_H_
#define __JSON_ANALYSIS_H_
/* ����ͷ�ļ� ------------------------------------------------------------------*/
#include "ch/bsp.h"
#include "ArduinoJson.h"
/* �궨�� ----------------------------------------------------------------------*/
/* ���Ͷ��� --------------------------------------------------------------------*/
class SerialReceiveBuffer
{
#define SBUF_SIZE 1024
public:
    u8 buf[SBUF_SIZE] = {0};
    u16 rev_c = 0;
    bool revd_f = false; //�������
};
#define JA_SERIAL Serial
/* �������� --------------------------------------------------------------------*/
extern StaticJsonDocument<1024> root;
extern SerialReceiveBuffer sRevBuf;
extern char payloadBuf[1024];
/* �������� --------------------------------------------------------------------*/
void JA_Init();
void JA_Scan();
#endif // __JSON_ANALYSIS_H_

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
/* 包含头文件 ------------------------------------------------------------------*/
#include "ch/bsp.h"
#include "ArduinoJson.h"
/* 宏定义 ----------------------------------------------------------------------*/
/* 类型定义 --------------------------------------------------------------------*/

#define JSONA_SERIAL Serial
#define JSONA_BADU_RATE 9600
#define JSONA_BUFSIZE 1024

class JSON_Analysis
{
public:
    /* variable ---------------------------------------------------------------------------------------------------------------*/
    static bool convOK;
    static uint8_t payloadBuf[JSONA_BUFSIZE];
    /* function ---------------------------------------------------------------------------------------------------------------*/
    // void begin(HardwareSerial s);
    void loop();

private:
    /* variable ---------------------------------------------------------------------------------------------------------------*/
    // HardwareSerial _serial;
    StaticJsonDocument<JSONA_BUFSIZE> _root;
    /* function ---------------------------------------------------------------------------------------------------------------*/
    bool checksum();
};
/* 变量声明 --------------------------------------------------------------------*/
extern char payloadBuf[JSONA_BUFSIZE];
extern bool payloadRecved;
/* 函数声明 --------------------------------------------------------------------*/
// void JSONA_Init();
// void JSONA_Scan();
#endif // __JSON_ANALYSIS_H_

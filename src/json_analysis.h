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
#define JA_SERIAL Serial
/* 变量声明 --------------------------------------------------------------------*/
extern StaticJsonDocument<1024> root;
extern char payloadBuf[1024];
/* 函数声明 --------------------------------------------------------------------*/
void JA_Init();
void JA_Scan();
#endif // __JSON_ANALYSIS_H_

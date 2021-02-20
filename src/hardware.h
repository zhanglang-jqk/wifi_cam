/***********************************************************************
 * @file hardware.h
 * HARDWARE
 * @author	:	ch
 * @brief	:
 * @version:	v1.0
 * @Copyright (C)  2020-12-29  .cdWFVCEL. all right reserved
***********************************************************************/

#ifndef __HARDWARE_H_
#define __HARDWARE_H_
/* 包含头文件 ------------------------------------------------------------------*/
#include "ch/bsp.h"
/* 宏定义 ----------------------------------------------------------------------*/
/* 类型定义 --------------------------------------------------------------------*/
/* 变量声明 --------------------------------------------------------------------*/
extern char localip[20];

extern long current_millis;
extern long last_capture_millis;
extern char strftime_buf[64];
extern int file_number;
extern bool internet_connected;
extern struct tm timeinfo;
extern time_t now;
/* 函数声明 --------------------------------------------------------------------*/
bool init_wifi();
void init_hardware();
#endif // __HARDWARE_H_

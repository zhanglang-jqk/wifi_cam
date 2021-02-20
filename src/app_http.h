/***********************************************************************
 * @file app_http.h
 * APP_HTTP
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2020-12-28  .cdWFVCEL. all right reserved
***********************************************************************/

#ifndef __APP_HTTP_H_
#define __APP_HTTP_H_
/* 包含头文件 ------------------------------------------------------------------*/
#include "ch/bsp.h"
/* 宏定义 ----------------------------------------------------------------------*/
/* 类型定义 --------------------------------------------------------------------*/
/* 变量声明 --------------------------------------------------------------------*/
/* 函数声明 --------------------------------------------------------------------*/
void do_time();
void do_start(char *the_message);
void do_stop(char *the_message);
void do_status(char *the_message);
void startCameraServer();
#endif // __APP_HTTP_H_

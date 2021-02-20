/***********************************************************************
 * @file camera.h
 * CAMERA
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2020-12-28  .cdWFVCEL. all right reserved
***********************************************************************/

#ifndef __AVI_H_
#define __AVI_H_
/* 包含头文件 ------------------------------------------------------------------*/
#include "ch/bsp.h"
#include "esp_camera.h"
/* 宏定义 ----------------------------------------------------------------------*/
/* 类型定义 --------------------------------------------------------------------*/
/* 变量声明 --------------------------------------------------------------------*/
/* 函数声明 --------------------------------------------------------------------*/
esp_err_t do_fb();
void make_avi();

#endif // __CAMERA_H_

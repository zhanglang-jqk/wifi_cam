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
/* ����ͷ�ļ� ------------------------------------------------------------------*/
#include "ch/bsp.h"
/* �궨�� ----------------------------------------------------------------------*/
/* ���Ͷ��� --------------------------------------------------------------------*/
/* �������� --------------------------------------------------------------------*/
extern char localip[20];

extern long current_millis;
extern long last_capture_millis;
extern char strftime_buf[64];
extern int file_number;
extern bool internet_connected;
extern struct tm timeinfo;
extern time_t now;
/* �������� --------------------------------------------------------------------*/
bool init_wifi();
void init_hardware();
#endif // __HARDWARE_H_

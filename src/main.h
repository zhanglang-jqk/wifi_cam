/***********************************************************************
 * @file main.h
 * MAIN
 * @author	:	ch
 * @brief	:
 * @version:	v1.0
 * @Copyright (C)  2020-12-28  .cdWFVCEL. all right reserved
***********************************************************************/

#ifndef __MAIN_H_
#define __MAIN_H_
/* 包含头文件 ------------------------------------------------------------------*/
#include "ch/bsp.h"

#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_camera.h"

#include <WiFi.h> // redundant

#include "ESP32FtpServer.h"
#include <HTTPClient.h>

extern FtpServer ftpSrv; //set #define FTP_DEBUG in ESP32FtpServer.h to see ftp verbose on serial

/* 宏定义 ----------------------------------------------------------------------*/
/* 类型定义 --------------------------------------------------------------------*/
/* 变量声明 --------------------------------------------------------------------*/

extern char *filename;
extern char *stream;
extern int newfile;
extern int frames_so_far;
extern FILE *myfile;
extern long bp;
extern long ap;
extern long bw;
extern long aw;
extern long totalp;
extern long totalw;
extern float avgp;
extern float avgw;
extern int overtime_count;

// these are just declarations -- look below to edit defaults

extern int capture_interval; // microseconds between captures
extern int total_frames;     // default updated below
extern int recording;        // default start recording on reboot
extern int framesize;        // vga
extern int repeat;           // capture 100 videos
extern int quality;
extern int xspeed;
extern int xlength;
extern int gray;

#define CAPTURE_PHOTO_INTERVAL (2 * 1000)      //拍照间隔
#define REPORT2SERVER_INTERVAL (5 * 60 * 1000) //上报间隔

extern std::vector<String> photoFilename_a;
extern std::vector<String> videoFilename_a;

extern bool videoAllowRecord_f;
extern bool photoAllowCapture_f;

extern bool mqttPulish_f;

#define FRAME_SIZE 1024 //fream

#define JSON_SERVERPORT 8082
#define VIDEO_SERVERPORT 8080
#define PHOTO_SERVERPORT 8081

/* 函数声明 --------------------------------------------------------------------*/
void major_fail();
#endif // __MAIN_H_

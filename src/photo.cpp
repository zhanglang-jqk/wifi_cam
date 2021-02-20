/***********************************************************************
 * @file 	:	photo.cpp
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2020-12-29  .cdWFVCEL. all right reserved
***********************************************************************/

/* 包含头文件 -------------------------------------------------------------------*/
#include "photo.h"
#include "main.h"
#include "hardware.h"
/* 宏定义 -----------------------------------------------------------------------*/
/* 类型定义 ---------------------------------------------------------------------*/
/* 私有变量 ---------------------------------------------------------------------*/
/* 扩展变量 ---------------------------------------------------------------------*/
/* 私有函数声明 -----------------------------------------------------------------*/
static esp_err_t save_photo_numbered();
static esp_err_t save_photo_dated();
/* 函数声明 ---------------------------------------------------------------------*/
/* 函数实现 ---------------------------------------------------------------------*/
// save photo stuff not currently used

static esp_err_t save_photo_numbered()
{
  file_number++;
  Serial.print("Taking picture: ");
  Serial.print(file_number);
  camera_fb_t *fb = esp_camera_fb_get();

  char *fname = (char *)malloc(21 + sizeof(int));
  sprintf(fname, "/sdcard/capture_%d.jpg", file_number);

  Serial.println(fname);
  FILE *file = fopen(fname, "w");
  if (file != NULL)
  {
    size_t err = fwrite(fb->buf, 1, fb->len, file);
    Serial.printf("File saved: %s\n", fname);
  }
  else
  {
    Serial.println("Could not open file");
  }
  fclose(file);
  esp_camera_fb_return(fb);
  free(fname);

  photoFilename_a.push_back(String(fname));
  photoAllowCapture_f = false;
}

static esp_err_t save_photo_dated()
{
  Serial.println("Taking a picture...");
  camera_fb_t *fb = esp_camera_fb_get();

  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%F__%H%M%S", &timeinfo);

  char fname[100];

  if (framesize == 6)
  {
    sprintf(fname, "/sdcard/%s_vga_%d.jpg", strftime_buf, quality);
  }
  else if (framesize == 7)
  {
    sprintf(fname, "/sdcard/%s_svga_%d.jpg", strftime_buf, quality);
  }
  else if (framesize == 10)
  {
    sprintf(fname, "/sdcard/%s_uxga_%d.jpg", strftime_buf, quality);
  }
  else if (framesize == 5)
  {
    sprintf(fname, "/sdcard/%s_cif_%d.jpg", strftime_buf, quality);
  }
  else
  {
    Serial.println("Wrong framesize");
    sprintf(fname, "/sdcard/%s_xxx_%d.jpg", strftime_buf, quality);
  }

  FILE *file = fopen(fname, "w");
  if (file != NULL)
  {
    size_t err = fwrite(fb->buf, 1, fb->len, file);
    Serial.printf("File saved: %s\n", fname);
  }
  else
  {
    Serial.printf("could not open file %s \r\n", fname);
  }
  fclose(file);
  esp_camera_fb_return(fb);

  photoFilename_a.push_back(String(fname));
  photoAllowCapture_f = false;
}

void save_photo()
{
  if (timeinfo.tm_year < (2016 - 1900) || internet_connected == false)
  {                        // if no internet or time not set
    save_photo_numbered(); // filenames in numbered order
  }
  else
  {
    save_photo_dated(); // filenames with date and time
  }
}
//photo.cpp
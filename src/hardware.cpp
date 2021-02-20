/***********************************************************************
 * @file 	:	hardware.cpp
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2020-12-29  .cdWFVCEL. all right reserved
***********************************************************************/

/* 包含头文件 -------------------------------------------------------------------*/
#include "hardware.h"
// Time
#include "time.h" 
#include "lwip/err.h"
#include "lwip/apps/sntp.h"

// MicroSD
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "main.h"
/* 宏定义 -----------------------------------------------------------------------*/
/* 类型定义 ---------------------------------------------------------------------*/
// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
/* 私有变量 ---------------------------------------------------------------------*/
//
//
// EDIT ssid and password
//
// zzz
const char *ssid = "host";
const char *password = "wocaonima88jqk";

long current_millis;
long last_capture_millis = 0;
static esp_err_t cam_err;
static esp_err_t card_err;
char strftime_buf[64];
int file_number = 0;
bool internet_connected = false;
struct tm timeinfo;
time_t now;


char localip[20];
// WiFiEventId_t eventID;
WiFiEventId_t eventID;

/* 扩展变量 ---------------------------------------------------------------------*/
/* 私有函数声明 -----------------------------------------------------------------*/
/* 函数声明 ---------------------------------------------------------------------*/
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

void init_time();
static esp_err_t init_sdcard();
/* 函数实现 ---------------------------------------------------------------------*/


void init_hardware()
{
    eventID = WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.print("WiFi lost connection. Reason: ");
        Serial.println(info.disconnected.reason);

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("*** connected/disconnected issue!   WiFi disconnected ???...");
            WiFi.disconnect();
        }
        else
        {
            Serial.println("*** WiFi disconnected ???...");
        }
    },
                           WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

    if (init_wifi())
    { // Connected to WiFi
        internet_connected = true;
        Serial.println("Internet connected");
        init_time();
        time(&now);
        //setenv("TZ", "GMT0BST,M3.5.0/01,M10.5.0/02", 1);
        // zzz
        setenv("TZ", "MST7MDT,M3.2.0/2:00:00,M11.1.0/2:00:00", 1); // mountain time zone
        tzset();
    }

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    //init with high specs to pre-allocate larger buffers
    if (psramFound())
    {
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 1;
        config.fb_count = 2;
    }
    else
    {
        config.frame_size = FRAMESIZE_SVGA; // svga 12 fails due to jpg 60000
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    // camera init
    cam_err = esp_camera_init(&config);
    if (cam_err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", cam_err);
        major_fail();
        return;
    }

    // SD camera init
    card_err = init_sdcard();
    if (card_err != ESP_OK)
    {
        Serial.printf("SD Card init failed with error 0x%x", card_err);
        major_fail();
        return;
    }
}


bool init_wifi()
{
  int connAttempts = 0;

  // zzz
  // Set your Static IP address
  IPAddress local_IP(192, 168, 1, 222);

  // Set your Gateway IP address
  IPAddress gateway(192, 168, 1, 1);

  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(114, 114, 114, 114); // optional
  IPAddress secondaryDNS(8, 8, 4, 4);       // optional

  WiFi.setHostname("ESP32CAM-222"); // does not seem to do anything with my wifi router ???

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");
    major_fail();
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (connAttempts > 10)
      return false;
    connAttempts++;
  }
  return true;
}

void init_time()
{

  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_setservername(1, "time.windows.com");
  sntp_setservername(2, "time.nist.gov");

  sntp_init();

  // wait for time to be set
  time_t now = 0;
  timeinfo = {0};
  int retry = 0;
  const int retry_count = 10;
  while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count)
  {
    Serial.printf("Waiting for system time to be set... (%d/%d) -- %d\n", retry, retry_count, timeinfo.tm_year);
    delay(2000);
    time(&now);
    localtime_r(&now, &timeinfo);
  }

  if (timeinfo.tm_year < (2016 - 1900))
  {
    major_fail();
  }
}

static esp_err_t init_sdcard()
{
  esp_err_t ret = ESP_FAIL;
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 10,
  };
  sdmmc_card_t *card;

  Serial.println("Mounting SD card...");
  ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

  if (ret == ESP_OK)
  {
    Serial.println("SD card mount successfully!");
  }
  else
  {
    Serial.printf("Failed to mount SD card VFAT filesystem. Error: %s", esp_err_to_name(ret));
    major_fail();
  }

  Serial.print("SD_MMC Begin: ");
  Serial.println(SD_MMC.begin()); // required by ftp system ??
}


//hardware.cpp
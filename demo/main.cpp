/*

  TimeLapseAvi

  ESP32-CAM Video Recorder

  This program records an AVI video on the SD Card of an ESP32-CAM.
  
  by James Zahary July 20, 2019
     jamzah.plc@gmail.com

  https://github.com/jameszah/ESP32-CAM-Video-Recorder
    jameszah/ESP32-CAM-Video-Recorder is licensed under the
    GNU General Public License v3.0

  Acknowlegements:

  1.  https://robotzero.one/time-lapse-esp32-cameras/
      Timelapse programs for ESP32-CAM version that sends snapshots of screen.
  2.  https://github.com/nailbuster/esp8266FTPServer
      ftp server (slightly modifed to get the directory function working)
  3.  https://github.com/ArduCAM/Arduino/tree/master/ArduCAM/examples/mini
      ArduCAM Mini demo (C)2017 LeeWeb: http://www.ArduCAM.com
      I copied the structure of the avi file, some calculations.

  The is Arduino code, with standard setup for ESP32-CAM
    - Board ESP32 Wrover Module
    - Partition Scheme Huge APP (3MB No OTA)
    
  This program records an AVI video on the SD Card of an ESP32-CAM.
  
  It will record realtime video at limited framerates, or timelapses with the full resolution of the ESP32-CAM.
  It is controlled by a web page it serves to stop and start recordings with many parameters, and look through the viewfinder.
  
  You can control framesize (UXGA, VGA, ...), quality, length, and fps to record, and fps to playback later, etc.

  There is also an ftp server to download the recordings to a PC.

  Instructions:

  The program uses a fixed IP of 192.168.1.222, so you can browse to it from your phone or computer.
  
  http://192.168.1.222/ -- this gives you the status of the recording in progress and lets you look through the viewfinder

  http://192.168.1.222/stop -- this stops the recording in progress and displays some sample commands to start new recordings

  ftp://192.168.1.222/ -- gives you the ftp server

  The ftp for esp32 seems to not be a full ftp.  The Chrome Browser and the Windows command line ftp's did not work with this, but
  the Raspbarian command line ftp works fine, and an old Windows ftp I have called CoffeeCup Free FTP also works, which is what I have been using.
  You can download at about 450 KB/s -- which is better than having to retreive the SD chip if you camera is up in a tree!
  
  http://192.168.1.222/start?framesize=VGA&length=1800&interval=250&quality=10&repeat=100&speed=1&gray=0  -- this is a sample to start a new recording

  framesize can be UXGA, SVGA, VGA, CIF (default VGA)
  length is length in seconds of the recording 0..3600 (default 1800)
  interval is the milli-seconds between frames (default 200)
  quality is a number 5..50 for the jpeg  - smaller number is higher quality with bigger and more detailed jpeg (default 10)
  repeat is a number of who many of the same recordings should be made (default 100)
  speed is a factor to speed up realtime for a timelapse recording - 1 is realtime (default 1)
  gray is 1 for a grayscale video (default 0 - color)

  These factors have to be within the limit of the SD chip to receive the data.
  For example, using a LEXAR 300x 32GB microSDHC UHS-I, the following works for me:

  UXGA quality 10,  2 fps (or interval of 500ms)
  SVGA quality 10,  5 fps (200ms)
  VGA  quality 10, 10 fps (100ms)
  CIG  quality 10, 20 fps (50ms)

  If you increase fps, you might have to reduce quality or framesize to keep it from dropping frames as it writes all the data to the SD chip.

  Also, other SD chips will be faster or slower.  I was using a SanDisk 16GB microSDHC "Up to 653X" - which was slower and more unpredictable than the LEXAR ???

  Search for "zzz" to find places to modify the code for:
    1.  Your wifi name and password
    2.  Your preferred ip address (with default gateway, etc)
    3.  Your Timezone for use in filenames
    4.  Defaults for framesize, quality, ... and if the recording should start on reboot of the ESP32 without receiving a command
  
*/

//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include "main.h"
#include "avi.h"
#include "app_http.h"
#include "hardware.h"
#include "photo.h"
// #include "_wifi.h"
#include "mqtt_client.h"
#include <sys/stat.h>

FtpServer ftpSrv; //set #define FTP_DEBUG in ESP32FtpServer.h to see ftp verbose on serial

char *filename;
char *stream;
int newfile = 0;
int frames_so_far = 0;
FILE *myfile;
long bp;
long ap;
long bw;
long aw;
long totalp;
long totalw;
float avgp;
float avgw;
int overtime_count = 0;

// these are just declarations -- look below to edit defaults

int capture_interval = 200; // microseconds between captures
int total_frames = 300;     // default updated below
int recording = 1;          // default start recording on reboot
int framesize = 6;          // vga
int repeat = 100;           // capture 100 videos
int quality = 10;
int xspeed = 1;
int xlength = 3;
int gray = 0;

// u8 *videoBuf = NULL;
u8 *fileBuf = NULL;
//
// if we have no camera, or sd card, then flash rear led on and off to warn the human SOS - SOS
//
void major_fail()
{
  while (1)
  {
    digitalWrite(33, LOW);
    delay(100);
    digitalWrite(33, HIGH);
    delay(100);

    digitalWrite(33, LOW);
    delay(100);
    digitalWrite(33, HIGH);
    delay(100);

    digitalWrite(33, LOW);
    delay(100);
    digitalWrite(33, HIGH);
    //delay(100);

    delay(1000);

    digitalWrite(33, LOW);
    delay(500);
    digitalWrite(33, HIGH);
    delay(500);

    digitalWrite(33, LOW);
    delay(500);
    digitalWrite(33, HIGH);
    delay(500);

    digitalWrite(33, LOW);
    delay(500);
    digitalWrite(33, HIGH);
    //delay(500);

    delay(1000);
  }
}
void setup()
{
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector  // creates other problems

  // Serial.begin(115200);
  Serial.begin(460800);

  Serial.setDebugOutput(true);
#if 1
  // zzz
  Serial.println("                                    ");
  Serial.println("-------------------------------------");
  Serial.println("ESP-CAM Video Recorder v23");
  Serial.println(" ip 192.168.1.222 ");
  Serial.println("-------------------------------------");

  pinMode(33, OUTPUT); // little red led on back of chip

  digitalWrite(33, LOW); // turn on the red LED on the back of chip

  if (psramFound())
  {
    Serial.println("psram founed!");

    // videoBuf = (u8 *)ps_malloc(1 * 1024 * 1024);
    // if (videoBuf == NULL)
    // {
    //   Serial.println("video buffer malloc failed!");
    // }

    MyPubSubClient::mqttBuf = (u8 *)ps_malloc(MyPubSubClient::MQTTBUF_SIZE);
    if (MyPubSubClient::mqttBuf == NULL)
    {
      Serial.println("mqtt buffer malloc failed!");
    }

    fileBuf = (u8 *)ps_malloc(MAXIMUM_FILE_SIZE);
    if (fileBuf == NULL)
    {
      Serial.println("file buffer malloc failed!");
    }

    init_hardware();

    // 200 ms x 150 frames = 30 seconds     is 3 MB indoors
    // 200 ms x 300 frames = 1 minute       is about 6MD indoor
    // 20 ms x 3000 frames = 10 minute      is 60 MB indoor
    // burst 1000 frames gives 8 fps rather than 5, so 2 minutues 20 MB indoor

    startCameraServer();

    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");

    sprintf(localip, "%s", WiFi.localIP().toString().c_str());
    Serial.print("localip ");
    Serial.println(localip);

    // zzz username and password for ftp server

    ftpSrv.begin("esp", "esp");
    digitalWrite(33, HIGH);

    //
    //  startup defaults  -- EDIT HERE
    //  zzz

    sensor_t *s = esp_camera_sensor_get();
    // s->set_framesize(s, FRAMESIZE_VGA);
    s->set_framesize(s, FRAMESIZE_QQVGA);
    s->set_quality(s, 10);
    do_fb(); // do a couple captures to make sure camera has new config
    do_fb();

    framesize = 6; // vga
    repeat = 100;  // 100 videos
    xspeed = 1;    // playback at 1 x realtime
    gray = 0;      // not gray

    quality = 10;           // quality 10 - pretty good.  Goes from 0..63, but 0-5 sometimes fails on bright scenery (jpg too big for ESP32CAM system)
    capture_interval = 200; // 200 milli-secconds per frame
    total_frames = 100;     // 9000 frames x 200 ms = 1800 sec = 30 min

    xlength = total_frames * capture_interval / 1000;

    newfile = 0;   // no file is open  // don't fiddle with this!
    recording = 1; // start recording on reboot without sending a command

#endif
    MQTTCLIENT_Init();
  }
}

////////////////////////////////////////////////////////////////////////////////////
//
// some globals for the loop()
//

long wakeup;
long last_wakeup = 0;

u32 capturePhoto_c = 0;
bool capturePhoto_lock = true; //ture : allow capture photo
//the first video record finish after set false
bool recordVideo_lock = true; //true : allow recording video

std::vector<String> photoFname_a;
std::vector<String> videoFname_a;

char param[1024];
char jsonBuf[1024 + 128];

bool mqttPulish_f = false;

void loop()
{

  if (WiFi.status() != WL_CONNECTED)
  {
    init_wifi();
    Serial.println("***** WiFi reconnect *****");
  }

  MQTTCLIENT_ClientReconnect();

  wakeup = millis();
  if (wakeup - last_wakeup > (10 * 60 * 1000))
  { // 10 minutes
    last_wakeup = millis();

    //init_wifi();
    //Serial.println("... wakeup call ...");
    //do_time();
  }

  ftpSrv.handleFTP();

  // if (millis() - capturePhoto_c > CAPTURE_PHOTO_TIME)
  // {
  //   if (capturePhoto_lock)
  //   {
  //     save_photo();
  //   }
  //   capturePhoto_c = millis();
  // }

  if (recordVideo_lock)
  {
    make_avi();
  }

  // static u16 frame_c = 0;
  // static u32 frame_t = 0;

  // camera_fb_t *fb = esp_camera_fb_get();
  // if (fb != NULL)
  // {
  //   esp_camera_fb_return(fb);
  //   frame_c++;
  // }

  // if (millis() - frame_t > 1000)
  // {
  //   Serial.printf("camera ov2640 maximun is %d \r\n", frame_c);
  //   frame_c = 0;
  //   frame_t = millis();
  // }

  if (mqttPulish_f)
  {
    // sprintf(param, "{\"LightSwitch\":%d}", digitalRead(LED_B)); //���ǰ�Ҫ�ϴ�������д��param��
    // strcpy(param, payloadBuf);
    // postMsgId += 1;
    // sprintf(jsonBuf, ALINK_BODY_FORMAT, postMsgId, ALINK_METHOD_PROP_POST, param);
    // //�ٴ�mqtt�ͻ����з���post��Ϣ
    // if (mqttClient.publish(ALINK_TOPIC_PROP_POST, jsonBuf))
    // {
    //   // Serial.print("Post message to cloud: ");
    //   // Serial.println(jsonBuf);

    //   Serial.printf("publish to <%s> is succeed! \r\n", ALINK_TOPIC_PROP_POST);
    // }
    // else
    // {
    //   Serial.printf("publish to <%s> is failed! \r\n", ALINK_TOPIC_PROP_POST);
    // }

    char *sendFrameBuf = (char *)ps_malloc(PHOTO_FILE_SIZE);
    // static char sendFrameBuf[PHOTO_FILE_SIZE] = {0};

    if (photoFname_a.size() > 0)
    {
      capturePhoto_lock = false;
      Serial.printf("send %d photo to server \r\n", photoFname_a.size());

      if (sendFrameBuf != NULL)
      {
        memset(sendFrameBuf, 0, PHOTO_FILE_SIZE);
        std::vector<String>::iterator it;
        for (it = photoFname_a.begin(); it != photoFname_a.end(); it++)
        {
          Serial.printf("open file %s \r\n", it->c_str());
          FILE *file = fopen(it->c_str(), "r");
          if (file != NULL)
          {
            u32 readLen = fread(sendFrameBuf, 1, PHOTO_FILE_SIZE, file);

            // for (int i = 0; i < readLen; i++)
            // {
            //     Serial.printf("%02x ", sendFrameBuf[i]);
            // }
            // Serial.printf("\r\nsend photo %s ,size is %d \r\n", it->c_str(), readLen);

            if (mqttClient.publish(ALINK_TOPIC_PICTURE, (u8 *)sendFrameBuf, (u32)readLen))
            {
              Serial.printf("publish to <%s> is succeed, picture size is %d !\r\n", ALINK_TOPIC_PICTURE, readLen);
            }
            else
            {
              Serial.printf("publish to <%s> is failed! \r\n", ALINK_TOPIC_PICTURE);
            }
            remove(it->c_str());
          }
          else
          {
            Serial.printf("could not open file %s \r\n", it->c_str());
          }
          fclose(file);
        }
      }
      else
      {
        Serial.println("memery malloc failed");
      }
      photoFname_a.clear();

      capturePhoto_lock = true;
    }

    if (videoFname_a.size() > 0)
    {
      Serial.printf("send %d video to server \r\n", videoFname_a.size());

      std::vector<String>::iterator it;
      for (it = videoFname_a.begin(); it != videoFname_a.end(); it++)
      {
        FILE *file = fopen(it->c_str(), "r");
        // u32 fileSize = get_file_size((const char *)(it->c_str()));
        // float frameRedio = fileSize / VIDEO_FRAME_SIZE;
        // u8 uploadingProgress = 0;
        u32 totalByte = 0;
        if (file != NULL)
        {
          for (int i = 0;; i += VIDEO_FRAME_SIZE)
          {
            if (feof(file))
            {
              break;
            }
            u32 readLen = fread(sendFrameBuf, 1, VIDEO_FRAME_SIZE, file);

            // for (int i = 0; i < readLen; i++)
            // {
            //   Serial.printf("%02x ", sendFrameBuf[i]);
            // }
            // Serial.print("start uploading video");
            if (!mqttClient.publish(ALINK_TOPIC_PICTURE, (u8 *)sendFrameBuf, (u32)readLen))
            {
              Serial.printf("publish to <%s> is failed! \r\n", ALINK_TOPIC_PICTURE);
            }
            else
            {
              // uploadingProgress += frameRedio;
              // Serial.printf("%d ", uploadingProgress);
            }

            memset(sendFrameBuf, 0, VIDEO_FRAME_SIZE);
            totalByte += readLen;
          }
          Serial.printf("\r\nsend video %s ,size is %d byte \r\n", it->c_str(), totalByte);
          free(sendFrameBuf);
          remove(it->c_str());
        }
        else
        {
          Serial.printf("could not open file %s \r\n", it->c_str());
        }
        // u32 readLen = fread(sendFrameBuf, 1, VIDEO_FRAME_SIZE, file);

        // u32 readLen = fread(fileBuf, 1, MAXIMUM_FILE_SIZE, file);
        // if (mqttClient.publish(ALINK_TOPIC_PICTURE, (u8 *)fileBuf, (u32)readLen))
        // {
        //   Serial.printf("publish to <%s> is succeed,file size is %d \r\n", ALINK_TOPIC_PICTURE, readLen);
        // }
        // else
        // {
        //   Serial.printf("publish to <%s> is failed, file %s size %d \r\n", ALINK_TOPIC_PICTURE, it->c_str(), readLen);
        // }
        fclose(file);
      }

      videoFname_a.clear();
      recordVideo_lock = true; //allow start record video
    }
    free(sendFrameBuf);
    mqttPulish_f = false;
  }
}

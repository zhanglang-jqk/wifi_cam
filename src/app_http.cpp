/***********************************************************************
 * @file 	:	app_http.cpp
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2020-12-28  .cdWFVCEL. all right reserved
***********************************************************************/

/* 包含头文件 -------------------------------------------------------------------*/
#include "app_http.h"
#include "main.h"
#include "hardware.h"
#include "avi.h"

/* 宏定义 -----------------------------------------------------------------------*/
/* 类型定义 ---------------------------------------------------------------------*/
/* 私有变量 ---------------------------------------------------------------------*/
httpd_handle_t camera_httpd = NULL;
char the_page[3000];


/* 扩展变量 ---------------------------------------------------------------------*/
/* 私有函数声明 -----------------------------------------------------------------*/
/* 函数声明 ---------------------------------------------------------------------*/
static esp_err_t capture_handler(httpd_req_t *req);
static esp_err_t stop_handler(httpd_req_t *req);
static esp_err_t start_handler(httpd_req_t *req);
static esp_err_t index_handler(httpd_req_t *req);


/* 函数实现 ---------------------------------------------------------------------*/

void do_time()
{
  //Serial.println(" ... wake up wifi stack ... ");

  //WiFi.reconnect();

  /*
    int numberOfNetworks = WiFi.scanNetworks();

    Serial.print("Number of networks found: ");
    Serial.println(numberOfNetworks);


    HTTPClient http_wake;

    http_wake.begin("http://postman-echo.com/time/now");
    //http_wake.begin("http://192.168.1.254");

    //http_wake.begin("http://google.com");

    int httpCode = http_wake.GET();

    if (httpCode > 0) { //Check for the returning code

    String payload = http_wake.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    }

    else {
    Serial.println("Error on HTTP request");
    }

    http_wake.end(); //Free the resources)
  */
}
////////////////////////////////////////////////////////////////////////////////////
static esp_err_t capture_handler(httpd_req_t *req)
{
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  char fname[100];

  fb = esp_camera_fb_get();
  if (!fb)
  {
    Serial.println("Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  file_number++;

  sprintf(fname, "inline; filename=capture_%d.jpg", file_number);

  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Content-Disposition", fname);

  size_t out_len, out_width, out_height;
  size_t fb_len = 0;
  fb_len = fb->len;
  res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);

  return res;
}

////////////////////////////////////////////////////////////////////////////////////
static esp_err_t stop_handler(httpd_req_t *req)
{

  esp_err_t res = ESP_OK;

  recording = 0;
  Serial.println("stopping recording");

  do_stop("Stopping previous recording");

  httpd_resp_send(req, the_page, strlen(the_page));
  return ESP_OK;
}

////////////////////////////////////////////////////////////////////////////////////
static esp_err_t start_handler(httpd_req_t *req)
{

  esp_err_t res = ESP_OK;

  char buf[80];
  size_t buf_len;
  char new_res[20];

  if (recording == 1)
  {
    const char *resp = "You must Stop recording, before starting a new one.  Start over ...";
    httpd_resp_send(req, resp, strlen(resp));

    return ESP_OK;
    return res;
  }
  else
  {
    recording = 1;
    Serial.println("starting recording");

    sensor_t *s = esp_camera_sensor_get();

    int new_interval = capture_interval;
    int new_length = capture_interval * total_frames;

    int new_framesize = s->status.framesize;
    int new_quality = s->status.quality;
    int new_repeat = 0;
    int new_xspeed = 1;
    int new_xlength = 3;
    int new_gray = 0;

    Serial.println("");
    Serial.println("Current Parameters :");
    Serial.print("  Capture Interval = ");
    Serial.print(capture_interval);
    Serial.println(" ms");
    Serial.print("  Length = ");
    Serial.print(capture_interval * total_frames / 1000);
    Serial.println(" s");
    Serial.print("  Quality = ");
    Serial.println(new_quality);
    Serial.print("  Framesize = ");
    Serial.println(new_framesize);
    Serial.print("  Repeat = ");
    Serial.println(repeat);
    Serial.print("  Speed = ");
    Serial.println(xspeed);

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1)
    {
      if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
      {
        ESP_LOGI(TAG, "Found URL query => %s", buf);
        char param[32];
        /* Get value of expected key from query string */
        Serial.println(" ... parameters");
        if (httpd_query_key_value(buf, "length", param, sizeof(param)) == ESP_OK)
        {

          int x = atoi(param);
          if (x > 1 && x <= 3600)
          {
            new_length = x;
          }

          ESP_LOGI(TAG, "Found URL query parameter => length=%s", param);
        }
        if (httpd_query_key_value(buf, "repeat", param, sizeof(param)) == ESP_OK)
        {
          int x = atoi(param);
          if (x >= 0)
          {
            new_repeat = x;
          }

          ESP_LOGI(TAG, "Found URL query parameter => repeat=%s", param);
        }
        if (httpd_query_key_value(buf, "framesize", new_res, sizeof(new_res)) == ESP_OK)
        {
          if (strcmp(new_res, "UXGA") == 0)
          {
            new_framesize = 10;
          }
          else if (strcmp(new_res, "SVGA") == 0)
          {
            new_framesize = 7;
          }
          else if (strcmp(new_res, "VGA") == 0)
          {
            new_framesize = 6;
          }
          else if (strcmp(new_res, "CIF") == 0)
          {
            new_framesize = 5;
          }
          else
          {
            Serial.println("Only UXGA, SVGA, VGA, and CIF are valid!");
          }
          ESP_LOGI(TAG, "Found URL query parameter => framesize=%s", new_res);
        }
        if (httpd_query_key_value(buf, "quality", param, sizeof(param)) == ESP_OK)
        {

          int x = atoi(param);
          if (x > 1 && x <= 50)
          {
            new_quality = x;
          }

          ESP_LOGI(TAG, "Found URL query parameter => quality=%s", param);
        }

        if (httpd_query_key_value(buf, "speed", param, sizeof(param)) == ESP_OK)
        {

          int x = atoi(param);
          if (x >= 1 && x <= 100)
          {
            new_xspeed = x;
          }

          ESP_LOGI(TAG, "Found URL query parameter => speed=%s", param);
        }

        if (httpd_query_key_value(buf, "gray", param, sizeof(param)) == ESP_OK)
        {

          int x = atoi(param);
          if (x == 1)
          {
            new_gray = x;
          }

          ESP_LOGI(TAG, "Found URL query parameter => gray=%s", param);
        }

        if (httpd_query_key_value(buf, "interval", param, sizeof(param)) == ESP_OK)
        {

          int x = atoi(param);
          if (x >= 1 && x <= 100000)
          {
            new_interval = x;
          }
          ESP_LOGI(TAG, "Found URL query parameter => interval=%s", param);
        }
      }
    }

    s->set_quality(s, new_quality);
    s->set_framesize(s, (framesize_t)new_framesize);
    if (new_gray == 1)
    {
      s->set_special_effect(s, 2); // 0 regular, 2 grayscale
    }
    else
    {
      s->set_special_effect(s, 0); // 0 regular, 2 grayscale
    }

    framesize = new_framesize;
    capture_interval = new_interval;
    xlength = new_length;
    total_frames = new_length * 1000 / capture_interval;
    repeat = new_repeat;
    quality = new_quality;
    xspeed = new_xspeed;
    gray = new_gray;

    do_fb();
    //delay(1000);
    do_fb(); // let camera warm up

    do_start("Starting a new AVI");
    httpd_resp_send(req, the_page, strlen(the_page));
    return ESP_OK;
  }
}

////////////////////////////////////////////////////////////////////////////////////
void do_start(char *the_message)
{

  Serial.print("do_start ");
  Serial.println(the_message);

  const char msg[] PROGMEM = R"rawliteral(<!doctype html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32-CAM Video Recorder</title>
</head>
<body>
<h1>ESP32-CAM Video Recorder v23</h1><br>
 <h2>Message is <font color="red">%s</font></h2><br>
 Recording = %d (1 is active)<br>
 Capture Interval = %d ms<br>
 Length = %d seconds<br>
 Quality = %d (5 best to 63 worst)<br>
 Framesize = %d (10 UXGA, 7 SVGA, 6 VGA, 5 CIF)<br>
 Repeat = %d<br>
 Speed = %d<br>
 Gray = %d<br><br>

<br>
<br><div id="image-container"></div>

</body>
</html>)rawliteral";

  sprintf(the_page, msg, the_message, recording, capture_interval, capture_interval * total_frames / 1000, quality, framesize, repeat, xspeed, gray);
  Serial.println(strlen(msg));

  //Serial.println(msg);
  //Serial.println(strlen(the_page));
  //Serial.println(the_page);
}

////////////////////////////////////////////////////////////////////////////////////
void do_stop(char *the_message)
{

  Serial.print("do_stop ");
  Serial.println(the_message);

  const char msg[] PROGMEM = R"rawliteral(<!doctype html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32-CAM Video Recorder</title>
</head>
<body>
<h1>ESP32-CAM Video Recorder v23</h1><br>
 <h2>Message is <font color="red">%s</font></h2><br>
 <h2><a href="http://%s/">http://%s/</a></h2><br>
   Information and viewfinder<br><br>
 <h2><a href="http://%s/start?framesize=VGA&length=1800&interval=200&quality=10&repeat=100&speed=1&gray=0">http://%s/start?framesize=VGA&length=1800&interval=200&quality=10&repeat=100&speed=1&gray=0</a></h2><br>
   Gives you VGA 640x480, video of 1800 seconds (30 min), picture every 200 ms, jpeg quality 10, repeat for 100 more of the same and play back at 1x actual fps, and don't make it grayscale<br><br>      
<h2><a href="http://%s/start?framesize=UXGA&length=1800&interval=2000&quality=10&repeat=100&speed=30&gray=0">UXGA 2 sec per frame, for 30 minutes repeat, 30x playback</a></h2><br>
<h2><a href="http://%s/start?framesize=UXGA&length=1800&interval=200&quality=10&repeat=100&speed=1&gray=0">UXGA 5 fps for 30 minutes repeat</a></h2><br>
<h2><a href="http://%s/start?framesize=CIF&length=1800&interval=50&quality=10&repeat=100&speed=1&gray=0">CIF 20 fps second for 30 minutes repeat</a></h2><br>

<br>
<br><div id="image-container"></div>

</body>
</html>)rawliteral";

  sprintf(the_page, msg, the_message, localip, localip, localip, localip, localip, localip, localip);

  //Serial.println(strlen(msg));
  //Serial.println(msg);
  //Serial.println(strlen(the_page));
  //Serial.println(the_page);
}

////////////////////////////////////////////////////////////////////////////////////
void do_status(char *the_message)
{

  Serial.print("do_status ");
  Serial.println(the_message);

  const char msg[] PROGMEM = R"rawliteral(<!doctype html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32-CAM Video Recorder</title>
</head>
<body>
<h1>ESP32-CAM Video Recorder v23</h1><br>
 <h2>Message is <font color="red">%s</font></h2><br>
 Recording = %d (1 is active)<br>
 Frame %d of %d <br>
 Capture Interval = %d ms<br>
 Length = %d seconds<br>
 Quality = %d (5 best to 63 worst)<br>
 Framesize = %d (10 UXGA, 7 SVGA, 6 VGA, 5 CIF)<br>
 Repeat = %d<br>
 Playback Speed = %d<br>
 Gray = %d<br><br>
 Commands as follows for your ESP's ip address:<br><br>
 <h2><a href="http://%s/">http://%s/</a></h2><br>
   Information and viewfinder<br><br>
 <h2><a href="http://%s/stop">http://%s/stop</a></h2><br>
   You must "stop" before starting with new parameters<br><br>
 <br>
 <h2><a href="ftp://%s/">ftp://%s/</a></h2><br>
 Username: esp, Password: esp ... to download the files<br><br>
 Red LED on back of ESP will flash with every frame, and flash SOS if camera or sd card not working.<br>

<br>
Check camera position with the frames below every 5 seconds for 5 pictures  <br>
Refresh page for more.<br>
<br><div id="image-container"></div>
<script>
document.addEventListener('DOMContentLoaded', function() {
  var c = document.location.origin;
  const ic = document.getElementById('image-container');  
  var i = 1;
  
  var timing = 5000;

  function loop() {
    ic.insertAdjacentHTML('beforeend','<img src="'+`${c}/capture?_cb=${Date.now()}`+'">')
    i = i + 1;
    if ( i < 6 ) {
      window.setTimeout(loop, timing);
    }
  }
  loop();
  
})
</script><br>
</body>
</html>)rawliteral";

  //Serial.print("Serving web page, len= "); Serial.println(strlen(msg));
  //Serial.println(msg);

  sprintf(the_page, msg, the_message, recording, frames_so_far, total_frames, capture_interval, capture_interval * total_frames / 1000, quality, framesize, repeat, xspeed, gray, localip, localip, localip, localip, localip, localip);

  //Serial.println(strlen(the_page));
  //Serial.println(the_page);
}

////////////////////////////////////////////////////////////////////////////////////
static esp_err_t index_handler(httpd_req_t *req)
{

  do_status("Refresh Status");

  httpd_resp_send(req, the_page, strlen(the_page));
  return ESP_OK;
}

void startCameraServer()
{
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  httpd_uri_t index_uri = {
      .uri = "/",
      .method = HTTP_GET,
      .handler = index_handler,
      .user_ctx = NULL};
  httpd_uri_t capture_uri = {
      .uri = "/capture",
      .method = HTTP_GET,
      .handler = capture_handler,
      .user_ctx = NULL};

  httpd_uri_t file_stop = {
      .uri = "/stop",
      .method = HTTP_GET,
      .handler = stop_handler,
      .user_ctx = NULL};

  httpd_uri_t file_start = {
      .uri = "/start",
      .method = HTTP_GET,
      .handler = start_handler,
      .user_ctx = NULL};

  if (httpd_start(&camera_httpd, &config) == ESP_OK)
  {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &capture_uri);
    httpd_register_uri_handler(camera_httpd, &file_start);
    httpd_register_uri_handler(camera_httpd, &file_stop);
  }
}

//app_http.cpp
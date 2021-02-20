/***********************************************************************
 * @file 	:	camera.cpp
 * @author	:	ch
 * @brief	:
 *  Make the avi move in 4 pieces
 *  make_avi() called in every loop, which calls below, depending on conditions
 *   start_avi() - open the file and write headers
 *   another_pic_avi() - write one more frame of movie
 *   end_avi() - write the final parameters and close the file
 * @version:	v1.0
 * @Copyright (C)  2020-12-28  .cdWFVCEL. all right reserved
***********************************************************************/

/* 包含头文件 -------------------------------------------------------------------*/
#include "avi.h"
#include "main.h"
#include "hardware.h"
/* 宏定义 -----------------------------------------------------------------------*/
// GLOBALS
#define BUFFSIZE 512
/* 类型定义 ---------------------------------------------------------------------*/
/* 私有变量 ---------------------------------------------------------------------*/

#define AVIOFFSET 240 // AVI main header length

unsigned long movi_size = 0;
unsigned long jpeg_size = 0;
unsigned long idx_offset = 0;

uint8_t zero_buf[4] = {0x00, 0x00, 0x00, 0x00};
uint8_t dc_buf[4] = {0x30, 0x30, 0x64, 0x63};   // "00dc"
uint8_t avi1_buf[4] = {0x41, 0x56, 0x49, 0x31}; // "AVI1"
uint8_t idx1_buf[4] = {0x69, 0x64, 0x78, 0x31}; // "idx1"

uint8_t vga_w[2] = {0x80, 0x02};  // 640
uint8_t vga_h[2] = {0xE0, 0x01};  // 480
uint8_t cif_w[2] = {0x90, 0x01};  // 400
uint8_t cif_h[2] = {0x28, 0x01};  // 296
uint8_t svga_w[2] = {0x20, 0x03}; //
uint8_t svga_h[2] = {0x58, 0x02}; //
uint8_t uxga_w[2] = {0x40, 0x06}; // 1600
uint8_t uxga_h[2] = {0xB0, 0x04}; // 1200

const int avi_header[AVIOFFSET] PROGMEM = {
    0x52,
    0x49,
    0x46,
    0x46,
    0xD8,
    0x01,
    0x0E,
    0x00,
    0x41,
    0x56,
    0x49,
    0x20,
    0x4C,
    0x49,
    0x53,
    0x54,
    0xD0,
    0x00,
    0x00,
    0x00,
    0x68,
    0x64,
    0x72,
    0x6C,
    0x61,
    0x76,
    0x69,
    0x68,
    0x38,
    0x00,
    0x00,
    0x00,
    0xA0,
    0x86,
    0x01,
    0x00,
    0x80,
    0x66,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x10,
    0x00,
    0x00,
    0x00,
    0x64,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x80,
    0x02,
    0x00,
    0x00,
    0xe0,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x4C,
    0x49,
    0x53,
    0x54,
    0x84,
    0x00,
    0x00,
    0x00,
    0x73,
    0x74,
    0x72,
    0x6C,
    0x73,
    0x74,
    0x72,
    0x68,
    0x30,
    0x00,
    0x00,
    0x00,
    0x76,
    0x69,
    0x64,
    0x73,
    0x4D,
    0x4A,
    0x50,
    0x47,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x0A,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x73,
    0x74,
    0x72,
    0x66,
    0x28,
    0x00,
    0x00,
    0x00,
    0x28,
    0x00,
    0x00,
    0x00,
    0x80,
    0x02,
    0x00,
    0x00,
    0xe0,
    0x01,
    0x00,
    0x00,
    0x01,
    0x00,
    0x18,
    0x00,
    0x4D,
    0x4A,
    0x50,
    0x47,
    0x00,
    0x84,
    0x03,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x49,
    0x4E,
    0x46,
    0x4F,
    0x10,
    0x00,
    0x00,
    0x00,
    0x6A,
    0x61,
    0x6D,
    0x65,
    0x73,
    0x7A,
    0x61,
    0x68,
    0x61,
    0x72,
    0x79,
    0x67,
    0x6D,
    0x61,
    0x69,
    0x6C,
    0x4C,
    0x49,
    0x53,
    0x54,
    0x00,
    0x01,
    0x0E,
    0x00,
    0x6D,
    0x6F,
    0x76,
    0x69,
};

// global variable used by these pieces

char str[20];
uint16_t n;
uint8_t buf[BUFFSIZE];

static int i = 0;
uint8_t temp = 0, temp_last = 0;
unsigned long fileposition = 0;
uint16_t frame_cnt = 0;
uint16_t remnant = 0;
uint32_t length = 0;
uint32_t startms;
uint32_t elapsedms;
uint32_t uVideoLen = 0;
bool is_header = false;
long bigdelta = 0;

camera_fb_t *fb = NULL;

FILE *avifile = NULL;
FILE *idxfile = NULL;
/* 扩展变量 ---------------------------------------------------------------------*/
/* 私有函数声明 -----------------------------------------------------------------*/
static void inline print_quartet(unsigned long i, FILE *fd);
static esp_err_t start_avi();
static esp_err_t another_pic_avi();
static esp_err_t end_avi();
/* 函数声明 ---------------------------------------------------------------------*/
/* 函数实现 ---------------------------------------------------------------------*/
esp_err_t do_fb()
{

  camera_fb_t *fb = esp_camera_fb_get();

  Serial.print("Pic, len=");
  Serial.println(fb->len);

  esp_camera_fb_return(fb);
}

void make_avi()
{

  // we are recording, but no file is open

  if (newfile == 0 && recording == 1)
  { // open the file

    //save_photo_dated();

    start_avi();

    digitalWrite(33, HIGH);
    newfile = 1;
    totalp = 0;
    totalw = 0;
    frame_cnt = 0;
    frames_so_far = 0;
  }
  else
  {

    // we have a file open, but not recording

    if (newfile == 1 && recording == 0)
    { // got command to close file
      digitalWrite(33, LOW);

      end_avi();

      Serial.println("Done capture due to command");

      frames_so_far = total_frames;

      newfile = 0;   // file is closed
      recording = 0; // DO NOT start another recording
    }
    else
    {

      if (newfile == 1 && recording == 1)
      { // regular recording

        if (frames_so_far == total_frames)
        { // we are done the recording

          Serial.println("Done capture for total frames!");

          digitalWrite(33, LOW); // close the file
          end_avi();

          frames_so_far = 0;
          newfile = 0; // file is closed

          if (repeat > 0)
          {
            recording = 1; // start another recording
            repeat = repeat - 1;
          }
          else
          {
            recording = 0;
          }
        }
        else if ((millis() - startms) > (total_frames * capture_interval))
        {

          Serial.println(" ");
          Serial.println("Done capture for time");
          Serial.print("Time Elapsed: ");
          Serial.print(millis() - startms);
          Serial.print(" Frames: ");
          Serial.println(frame_cnt);
          Serial.print("Config:       ");
          Serial.print(total_frames * capture_interval);
          Serial.print(" (");
          Serial.print(total_frames);
          Serial.print(" x ");
          Serial.print(capture_interval);
          Serial.println(")");

          digitalWrite(33, LOW); // close the file

          end_avi();

          frames_so_far = 0;
          newfile = 0; // file is closed
          if (repeat > 0)
          {
            recording = 1; // start another recording
            repeat = repeat - 1;
          }
          else
          {
            recording = 0;
          }
        }
        else
        { // regular

          current_millis = millis();

          if (current_millis - last_capture_millis > capture_interval)
          { // Take another picture - fixed interval

            //if ((current_millis - startms) > (frame_cnt * capture_interval)) {                   // Take another picture - with catch up

            last_capture_millis = millis();

            frames_so_far = frames_so_far + 1;
            frame_cnt++;

            another_pic_avi();
          }
        }
      }
    }
  }
}

char fname[100];
static esp_err_t start_avi()
{

  //Serial.println("Starting an avi ");

  do_fb(); // start the camera ... warm it up
  delay(1000);
  do_fb();
  delay(1000);
  do_fb();

  time(&now);
  localtime_r(&now, &timeinfo);

  strftime(strftime_buf, sizeof(strftime_buf), "%F__%H-%M-%S", &timeinfo);

  if (framesize == 6)
  {
    sprintf(fname, "/sdcard/%s_vga_Q%d_I%d_L%d_S%d.avi", strftime_buf, quality, capture_interval, xlength, xspeed);
  }
  else if (framesize == 7)
  {
    sprintf(fname, "/sdcard/%s_svga_Q%d_I%d_L%d_S%d.avi", strftime_buf, quality, capture_interval, xlength, xspeed);
  }
  else if (framesize == 10)
  {
    sprintf(fname, "/sdcard/%s_uxga_Q%d_I%d_L%d_S%d.avi", strftime_buf, quality, capture_interval, xlength, xspeed);
  }
  else if (framesize == 5)
  {
    sprintf(fname, "/sdcard/%s_cif_Q%d_I%d_L%d_S%d.avi", strftime_buf, quality, capture_interval, xlength, xspeed);
  }
  else
  {
    Serial.println("Wrong framesize");
    sprintf(fname, "/sdcard/%s_xxx_Q%d_I%d_L%d_S%d.avi", strftime_buf, quality, capture_interval, xlength, xspeed);
  }

  Serial.print("\nFile name will be >");
  Serial.print(fname);
  Serial.println("<");

  avifile = fopen(fname, "w");
  idxfile = fopen("/sdcard/idx.tmp", "w");

  if (avifile != NULL)
  {

    //Serial.printf("File open: %s\n", fname);
  }
  else
  {
    Serial.println("Could not open file");
    major_fail();
  }

  if (idxfile != NULL)
  {

    //Serial.printf("File open: %s\n", "/sdcard/idx.tmp");
  }
  else
  {
    Serial.println("Could not open file");
    major_fail();
  }

  // -- open large file and initialize  -- doesn't help much

  //fseek(avifile, 1 * 1024 * 1024, SEEK_SET);              // start with 1 MB file
  //size_t ze_err = fwrite(zero_buf, 1, 4, avifile);
  //fseek(avifile, 0, SEEK_SET);

  for (i = 0; i < AVIOFFSET; i++)
  {
    char ch = pgm_read_byte(&avi_header[i]);
    buf[i] = ch;
  }

  size_t err = fwrite(buf, 1, AVIOFFSET, avifile);

  if (framesize == 6)
  {

    fseek(avifile, 0x40, SEEK_SET);
    err = fwrite(vga_w, 1, 2, avifile);
    fseek(avifile, 0xA8, SEEK_SET);
    err = fwrite(vga_w, 1, 2, avifile);
    fseek(avifile, 0x44, SEEK_SET);
    err = fwrite(vga_h, 1, 2, avifile);
    fseek(avifile, 0xAC, SEEK_SET);
    err = fwrite(vga_h, 1, 2, avifile);
  }
  else if (framesize == 10)
  {

    fseek(avifile, 0x40, SEEK_SET);
    err = fwrite(uxga_w, 1, 2, avifile);
    fseek(avifile, 0xA8, SEEK_SET);
    err = fwrite(uxga_w, 1, 2, avifile);
    fseek(avifile, 0x44, SEEK_SET);
    err = fwrite(uxga_h, 1, 2, avifile);
    fseek(avifile, 0xAC, SEEK_SET);
    err = fwrite(uxga_h, 1, 2, avifile);
  }
  else if (framesize == 7)
  {

    fseek(avifile, 0x40, SEEK_SET);
    err = fwrite(svga_w, 1, 2, avifile);
    fseek(avifile, 0xA8, SEEK_SET);
    err = fwrite(svga_w, 1, 2, avifile);
    fseek(avifile, 0x44, SEEK_SET);
    err = fwrite(svga_h, 1, 2, avifile);
    fseek(avifile, 0xAC, SEEK_SET);
    err = fwrite(svga_h, 1, 2, avifile);
  }
  else if (framesize == 5)
  {

    fseek(avifile, 0x40, SEEK_SET);
    err = fwrite(cif_w, 1, 2, avifile);
    fseek(avifile, 0xA8, SEEK_SET);
    err = fwrite(cif_w, 1, 2, avifile);
    fseek(avifile, 0x44, SEEK_SET);
    err = fwrite(cif_h, 1, 2, avifile);
    fseek(avifile, 0xAC, SEEK_SET);
    err = fwrite(cif_h, 1, 2, avifile);
  }

  fseek(avifile, AVIOFFSET, SEEK_SET);

  Serial.print(F("\nRecording "));
  Serial.print(total_frames);
  Serial.println(F(" video frames ...\n"));

  startms = millis();
  bigdelta = millis();
  totalp = 0;
  totalw = 0;
  overtime_count = 0;
  jpeg_size = 0;
  movi_size = 0;
  uVideoLen = 0;
  idx_offset = 4;

} // end of start avi

static esp_err_t another_pic_avi()
{

  digitalWrite(33, LOW);

  bp = millis();
  fb = esp_camera_fb_get();
  totalp = totalp + millis() - bp;

  jpeg_size = fb->len;
  movi_size += jpeg_size; // Update totals
  uVideoLen += jpeg_size;

  size_t dc_err = fwrite(dc_buf, 1, 4, avifile);
  size_t ze_err = fwrite(zero_buf, 1, 4, avifile);

  bw = millis();
  size_t err = fwrite(fb->buf, 1, fb->len, avifile);

  totalw = totalw + millis() - bw;

  digitalWrite(33, HIGH); // red led is on durng the photo and the write

  if (millis() - bigdelta > capture_interval * 1.25)
  { // how many are 25 % overtime
    overtime_count = overtime_count + 1;
    //Serial.print(millis()-bigdelta);Serial.print(",");
  }

  if (millis() - bigdelta > capture_interval * 2)
  { // if we are 100% overtime

    //if (frame_cnt % 10 == 1) {

    Serial.print("Frame: ");
    Serial.print(frame_cnt);
    Serial.print(" size ");
    Serial.print(err);
    Serial.print(" delta ");
    Serial.print(millis() - bigdelta);
    Serial.print(" > ");
    Serial.print(capture_interval);

    Serial.print(" avg pic ");
    Serial.print(totalp / frame_cnt);
    Serial.print(" avg wrt ");
    Serial.print(totalw / frame_cnt);

    Serial.print(" overtime ");
    Serial.print(overtime_count);
    Serial.print(" ");
    Serial.print(100.0 * overtime_count / frame_cnt, 1);
    Serial.println(" %");
  }
  bigdelta = millis();

  remnant = (4 - (jpeg_size & 0x00000003)) & 0x00000003;

  print_quartet(idx_offset, idxfile);
  print_quartet(jpeg_size, idxfile);

  idx_offset = idx_offset + jpeg_size + remnant + 8;

  jpeg_size = jpeg_size + remnant;
  movi_size = movi_size + remnant;
  if (remnant > 0)
  {
    size_t rem_err = fwrite(zero_buf, 1, remnant, avifile);
  }
  esp_camera_fb_return(fb);

  fileposition = ftell(avifile);                          // Here, we are at end of chunk (after padding)
  fseek(avifile, fileposition - jpeg_size - 4, SEEK_SET); // Here we are the the 4-bytes blank placeholder

  print_quartet(jpeg_size, avifile); // Overwrite placeholder with actual frame size (without padding)

  fileposition = ftell(avifile);

  fseek(avifile, fileposition + 6, SEEK_SET); // Here is the FOURCC "JFIF" (JPEG header)
  // Overwrite "JFIF" (still images) with more appropriate "AVI1"

  size_t av_err = fwrite(avi1_buf, 1, 4, avifile);

  fileposition = ftell(avifile);
  fseek(avifile, fileposition + jpeg_size - 10, SEEK_SET);

} // end of another_pic_avi

static esp_err_t end_avi()
{

  unsigned long current_end = 0;

  current_end = ftell(avifile);

  Serial.println("End of avi - closing the files");

  elapsedms = millis() - startms;
  float fRealFPS = (1000.0f * (float)frame_cnt) / ((float)elapsedms) * xspeed;
  float fmicroseconds_per_frame = 1000000.0f / fRealFPS;
  uint8_t iAttainedFPS = round(fRealFPS);
  uint32_t us_per_frame = round(fmicroseconds_per_frame);

  //Modify the MJPEG header from the beginning of the file, overwriting various placeholders

  fseek(avifile, 4, SEEK_SET);
  print_quartet(movi_size + 240 + 16 * frame_cnt + 8 * frame_cnt, avifile);

  fseek(avifile, 0x20, SEEK_SET);
  print_quartet(us_per_frame, avifile);

  unsigned long max_bytes_per_sec = movi_size * iAttainedFPS / frame_cnt;

  fseek(avifile, 0x24, SEEK_SET);
  print_quartet(max_bytes_per_sec, avifile);

  fseek(avifile, 0x30, SEEK_SET);
  print_quartet(frame_cnt, avifile);

  fseek(avifile, 0x84, SEEK_SET);
  print_quartet((int)iAttainedFPS, avifile);

  fseek(avifile, 0xe8, SEEK_SET);

  print_quartet(movi_size + frame_cnt * 8 + 4, avifile);

  Serial.println(F("\n*** Video recorded and saved ***\n"));
  Serial.print(F("Recorded "));
  Serial.print(elapsedms / 1000);
  Serial.print(F("s in "));
  Serial.print(frame_cnt);
  Serial.print(F(" frames\nFile size is "));
  Serial.print(movi_size + 12 * frame_cnt + 4);
  Serial.print(F(" bytes\nActual FPS is "));
  Serial.print(fRealFPS, 2);
  Serial.print(F("\nMax data rate is "));
  Serial.print(max_bytes_per_sec);
  Serial.print(F(" byte/s\nFrame duration is "));
  Serial.print(us_per_frame);
  Serial.println(F(" us"));
  Serial.print(F("Average frame length is "));
  Serial.print(uVideoLen / frame_cnt);
  Serial.println(F(" bytes"));
  Serial.print("Average picture time (ms) ");
  Serial.println(totalp / frame_cnt);
  Serial.print("Average write time (ms) ");
  Serial.println(totalw / frame_cnt);
  Serial.print("Frames 25%+ late % ");
  Serial.println(100.0 * overtime_count / frame_cnt, 1);

  Serial.println("Writing the index");

  fseek(avifile, current_end, SEEK_SET);

  fclose(idxfile);

  size_t i1_err = fwrite(idx1_buf, 1, 4, avifile);

  print_quartet(frame_cnt * 16, avifile);

  idxfile = fopen("/sdcard/idx.tmp", "r");

  if (idxfile != NULL)
  {

    //Serial.printf("File open: %s\n", "/sdcard/idx.tmp");
  }
  else
  {
    Serial.println("Could not open file");
    //major_fail();
  }

  char *AteBytes;
  AteBytes = (char *)malloc(8);

  for (int i = 0; i < frame_cnt; i++)
  {

    size_t res = fread(AteBytes, 1, 8, idxfile);
    size_t i1_err = fwrite(dc_buf, 1, 4, avifile);
    size_t i2_err = fwrite(zero_buf, 1, 4, avifile);
    size_t i3_err = fwrite(AteBytes, 1, 8, avifile);
  }

  free(AteBytes);

  fclose(idxfile);
  fclose(avifile);

  Serial.println("---");
  WiFi.printDiag(Serial);
  Serial.println("---");

  videoFilename_a.push_back(String(fname));
  videoAllowRecord_f = false;
  //do_time();
}

//
// Writes an uint32_t in Big Endian at current file position
//
static void inline print_quartet(unsigned long i, FILE *fd)
{
  uint8_t x[1];

  x[0] = i % 0x100;
  size_t i1_err = fwrite(x, 1, 1, fd);
  i = i >> 8;
  x[0] = i % 0x100;
  size_t i2_err = fwrite(x, 1, 1, fd);
  i = i >> 8;
  x[0] = i % 0x100;
  size_t i3_err = fwrite(x, 1, 1, fd);
  i = i >> 8;
  x[0] = i % 0x100;
  size_t i4_err = fwrite(x, 1, 1, fd);
}

//camera.cpp
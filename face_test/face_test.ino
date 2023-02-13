

#include "esp_camera.h"
#include "camera_pins.h"
#include "img_converters.h"
#include <ESP_Mail_Client.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include "fb_gfx.h"
#include "fd_forward.h"
#include "fr_forward.h"

//WiFi名称
#define WIFI_SSID "testWiFi"
//WiFi密码
#define WIFI_PASSWORD "31415926"

//网易的SMTP服务器
#define SMTP_HOST "smtp.163.com"
//端口号
#define SMTP_PORT 465
//发送的邮箱
#define AUTHOR_EMAIL "puoughell@163.com"
//授权码
#define AUTHOR_PASSWORD "KUBMWLOFJOGGAQSO"

//接收的邮箱
#define RECIPIENT_EMAIL "2064617582@qq.com"

Servo myservo;
int cur_pos = 0;
int servoPin = 12;
int update_flag = 0;

void smtpCallback(SMTP_Status status);
void Update_Image(camera_fb_t * fb_data);
int Control_pos(int x, int w);

SMTPSession smtp;
ESP_Mail_Session session;


static mtmn_config_t mtmn_config = {0};
// static int run_face_recognition(dl_matrix3du_t *image_matrix, box_array_t *net_boxes);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  ESP32PWM::allocateTimer(1);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
	myservo.attach(servoPin, 1000, 2000); 

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
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

    // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

  Serial.print("Connecting to AP");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
      Serial.print(".");
      delay(200);
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  MailClient.networkReconnect(true);
  smtp.debug(1);
  smtp.callback(smtpCallback);

  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  // session.time.ntp_server = "ntp1.aliyun.com,ntp2.aliyun.com";
  // session.time.gmt_offset = 8*3600;
  // session.time.day_light_offset = 0;

  mtmn_config.type = FAST;
  mtmn_config.min_face = 80;
  mtmn_config.pyramid = 0.707;
  mtmn_config.pyramid_times = 4;
  mtmn_config.p_threshold.score = 0.6;
  mtmn_config.p_threshold.nms = 0.7;
  mtmn_config.p_threshold.candidate_number = 20;
  mtmn_config.r_threshold.score = 0.7;
  mtmn_config.r_threshold.nms = 0.7;
  mtmn_config.r_threshold.candidate_number = 10;
  mtmn_config.o_threshold.score = 0.7;
  mtmn_config.o_threshold.nms = 0.7;
  mtmn_config.o_threshold.candidate_number = 1;

}

void loop() {
  // put your main code here, to run repeatedly:
  camera_fb_t* fb = NULL;
  bool s;

  fb = esp_camera_fb_get();
  if (!fb) {
      Serial.println("Camera capture failed");
      esp_camera_fb_return(fb);
  }else
  {
    size_t out_len, out_width, out_height;
    uint8_t * out_buf;
    dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
    if (!image_matrix) {
        esp_camera_fb_return(fb);
        Serial.println("dl_matrix3du_alloc failed");
    }else
    {
      out_buf = image_matrix->item;
      out_len = fb->width * fb->height * 3;
      out_width = fb->width;
      out_height = fb->height;

      s = fmt2rgb888(fb->buf, fb->len, fb->format, out_buf);
      if(!s){
          dl_matrix3du_free(image_matrix);
          Serial.println("to rgb888 failed");
      }else
      {
        box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);
        if (net_boxes){
          int x, y, w, h, i;
          for(i = 0; i < net_boxes->len; i++)
          {
            x = (int)net_boxes->box[i].box_p[0];
            w = (int)net_boxes->box[i].box_p[2] - x + 1;
          }
          update_flag = Control_pos(x, w);
          if(update_flag)
          {
            Serial.println("I am Update photos");
            Update_Image(fb);
            update_flag = 0;
          }
        }
        esp_camera_fb_return(fb);
        dl_matrix3du_free(image_matrix);
      }
    }
  }
}

void Update_Image(camera_fb_t * fb_data)
{
  SMTP_Message message;
  SMTP_Attachment att;

  message.enable.chunking = true;
  message.sender.name = F("ESP");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = F("ESP Test Email");
  message.addRecipient(F("Sara"), RECIPIENT_EMAIL);
  message.html.content = F("<span style=\"color:#ff0000;\">The camera image.</span><br/><br/><img src=\"cid:image-001\" alt=\"esp32 cam image\"  width=\"2048\" height=\"1536\">");
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  message.html.charSet = F("utf-8");

  att.descr.filename = F("camera.jpg");
  att.descr.mime = F("image/jpg");

  att.blob.data = fb_data->buf;
  att.blob.size = fb_data->len;

  att.descr.content_id = F("image-001");
  att.descr.transfer_encoding = Content_Transfer_Encoding::enc_base64;
  message.addInlineImage(att);

  if (!smtp.connect(&session)){
    Serial.println("smtp会话连接失败\n");    
    return;
  }
    

  if (!MailClient.sendMail(&smtp, &message, true))
    Serial.println("Error sending Email, " + smtp.errorReason());
    
  ESP_MAIL_PRINTF("Free Heap: %d\n", MailClient.getFreeHeap());

}


void smtpCallback(SMTP_Status status)
{
    /* Print the current status */
    Serial.println(status.info());

    /* Print the sending result */
    if (status.success())
    {
        // ESP_MAIL_PRINTF used in the examples is for format printing via debug Serial port
        // that works for all supported Arduino platform SDKs e.g. AVR, SAMD, ESP32 and ESP8266.
        // In ESP8266 and ESP32, you can use Serial.printf directly.

        Serial.println("----------------");
        ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
        ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
        Serial.println("----------------\n");

        for (size_t i = 0; i < smtp.sendingResult.size(); i++)
        {
            /* Get the result item */
            SMTP_Result result = smtp.sendingResult.getItem(i);

            // In case, ESP32, ESP8266 and SAMD device, the timestamp get from result.timestamp should be valid if
            // your device time was synched with NTP server.
            // Other devices may show invalid timestamp as the device time was not set i.e. it will show Jan 1, 1970.
            // You can call smtp.setSystemTime(xxx) to set device time manually. Where xxx is timestamp (seconds since Jan 1, 1970)
            time_t ts = (time_t)result.timestamp;

            ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
            ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
            ESP_MAIL_PRINTF("Date/Time: %s\n", asctime(localtime(&ts)));
            ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
            ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
        }
        Serial.println("----------------\n");

        // You need to clear sending result as the memory usage will grow up.
        smtp.sendingResult.clear();
    }
}

int Control_pos(int x, int w)
{
  int correct_pos = 0;
  int diff = 0;
  x = x + w/2;
  diff = x -160;
  if(abs(diff) <= 40)
  {
    correct_pos = 1;
  }else
  {
    if(diff > 0)
    {
      cur_pos += 10;
      if(cur_pos == 180)
      {
        cur_pos = 170;
      }
      myservo.write(cur_pos);
    }else
    {
      cur_pos -= 10;
      if(cur_pos <= 0)
      {
        cur_pos = 10;
      }
      myservo.write(cur_pos);
    }
    delay(20);
  }
  return correct_pos;
}







#undef EPS
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#define EPS 192

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <esp_freertos_hooks.h>

#include "app_lcd.h"
#include "esp_camera.h"
#include <string.h>
#include "iot_lvgl.h"
#include "app_screen.h"


using namespace cv;
using namespace std;

int New_box[ROW][4];
int curr_num = 0;
static const char *TAG = "app_lcd";

static QueueHandle_t xQueueFrameI = NULL;
static QueueHandle_t xQueueFrameO = NULL;
static bool gReturnFB = true;
static lv_obj_t *lvCameraImage; // Camera image object



esp_err_t updateCameraImage(const cv::Mat &img) {
    // static variables because they must still be available when lv_task_handler() is called
    static Mat imgCopy;
    static lv_img_dsc_t my_img_dsc;

    if(img.empty()) {
        ESP_LOGW(TAG, "Can't display empty image");
        return ESP_ERR_INVALID_ARG;
    }
        // convert image to bgr565 if needed
    if(img.type() == CV_8UC1) {         // grayscale image
        cvtColor(img, imgCopy, COLOR_GRAY2BGR565, 1);
    }
    else if(img.type() == CV_8UC3) {    // BGR888 image
        cvtColor(img, imgCopy, COLOR_BGR2BGR565, 1);
    }
    else if(img.type() == CV_8UC2) {    // BGR565 image
        img.Copyto(imgCopy);
    }
    
    my_img_dsc.header.always_zero = 0;
    my_img_dsc.header.w = imgCopy.cols;
    my_img_dsc.header.h = imgCopy.rows;
    my_img_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
    my_img_dsc.data_size = imgCopy.size().width * imgCopy.size().height;
    my_img_dsc.data = imgCopy.ptr<uchar>(0);
    // TFT_jpg_image(CENTER,CENTER,0,-1, NULL,(uint8_t*)my_img_dsc.data,img.size().width * img.size().height);

    lv_img_set_src(lvCameraImage, &my_img_dsc);  /* Set the created file as image */
    lv_obj_set_pos(lvCameraImage, 0, 0);

    return ESP_OK;
}


static void task_process_handler(void *arg)
{
    camera_fb_t *frame = NULL;
    int i = 0;
    lvCameraImage = lv_img_create(lv_disp_get_scr_act(nullptr), nullptr);
    lv_obj_move_foreground(lvCameraImage);
    while(1)
    {
        if (xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY))
        {
            Mat src(frame->height, frame->width, CV_8UC2, frame->buf);
            Scalar color = Scalar(0,0,255);
            if(curr_num != 0)
            {
                for(i = 0; i < curr_num; i++)
                {
                    Rect temp_rect = Rect(New_box[i][0],New_box[i][1],New_box[i][2],New_box[i][3]);
                    rectangle(src, temp_rect, color);
                }
            }
            updateCameraImage(src);
            for(int j = 0; j < curr_num; j++)
            {
                for(int m = 0; m < 4; m++)
                {
                    New_box[j][m] = 0;
                }
            }
            curr_num = 0;
            if (xQueueFrameO)
            {
                xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
            }
            else if (gReturnFB)
            {
                esp_camera_fb_return(frame);
            }
            else
            {
                free(frame);
            }
        }
        
    }
}

void gui_init() {
    // Create screen
    lv_obj_t *scr = lv_obj_create(nullptr, nullptr);
    lv_scr_load(scr);
}


void app_lcd(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const bool return_fb)
{
    lvgl_init();
    gui_init();
    xQueueFrameI = frame_i;
    xQueueFrameO = frame_o;
    gReturnFB = return_fb;
    xTaskCreatePinnedToCore(task_process_handler, TAG, 4 * 1024, NULL, 5, NULL, 0);
}

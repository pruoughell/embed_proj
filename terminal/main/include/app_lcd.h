#pragma once

#define ROW   10

#include <stdint.h>
#include "esp_log.h"

#ifdef __cplusplus
extern "C"
{
#endif

void app_lcd(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const bool return_fb);



#ifdef __cplusplus
}
#endif
#ifndef __ESP32_DRIVER_MCP320X_LOG_H__
#define __ESP32_DRIVER_MCP320X_LOG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_log.h"

    static const char *TAG = "mcp320x";

    #define CMP_LOGD(format, ...) ESP_LOGD(TAG, format, ##__VA_ARGS__)
    #define CMP_LOGV(format, ...) ESP_LOGV(TAG, format, ##__VA_ARGS__)
    #define CMP_LOGI(format, ...) ESP_LOGI(TAG, format, ##__VA_ARGS__)
    #define CMP_LOGW(format, ...) ESP_LOGW(TAG, format, ##__VA_ARGS__)
    #define CMP_LOGE(format, ...) ESP_LOGE(TAG, format, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif



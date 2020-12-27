/* When building using VSCode tooling, the necessary
 variables are not injected in build time.
*/
#include "../build/config/sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "mcp320x.h"

#define GPIO_CS GPIO_NUM_5
#define GPIO_SCLK GPIO_NUM_18
#define GPIO_MISO GPIO_NUM_19
#define GPIO_MOSI GPIO_NUM_23

void app_main(void)
{
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,
        .flags = SPICOMMON_BUSFLAG_MASTER};

    mcp320x_config_t mcp320x_cfg = {
        .host = SPI3_HOST,
        .device_model = MCP3204_MODEL,
        .clock_speed_hz = 1 * 1000 * 1000, // 1 Mhz.
        .reference_voltage = 5000,         // 5V
        .cs_io_num = GPIO_CS};

    ESP_ERROR_CHECK(spi_bus_initialize(mcp320x_cfg.host, &bus_cfg, 0));

    mcp320x_handle_t mcp320x_handle;

    ESP_ERROR_CHECK(mcp320x_initialize(&mcp320x_cfg, &mcp320x_handle));

    while (true)
    {
        short raw;
        short voltage;

        ESP_ERROR_CHECK(mcp320x_read_raw(mcp320x_handle, MCP320X_CHANNEL_0, MCP320X_READ_MODE_SINGLE, &raw));
        ESP_ERROR_CHECK(mcp320x_read_voltage(mcp320x_handle, MCP320X_CHANNEL_0, MCP320X_READ_MODE_SINGLE, &voltage));

        ESP_LOGI("mcp320x", "Raw: %d", raw);
        ESP_LOGI("mcp320x", "Voltage: %d mV", voltage);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

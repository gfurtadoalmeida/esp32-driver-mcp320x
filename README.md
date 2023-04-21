# ESP32 MCP3204/3208 12-bit ADC Driver

![GitHub Build Status](https://github.com/gfurtadoalmeida/esp32-driver-mcp320x/actions/workflows/build.yml/badge.svg) [![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=esp32_driver_mcp320x&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=esp32_driver_mcp320x) [![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=esp32_driver_mcp320x&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=esp32_driver_mcp320x)  
ESP32 driver for Microchip [MCP3204](https://www.microchip.com/en-us/product/MCP3204) and [MCP3208](https://www.microchip.com/en-us/product/MCP3208) 12-bit ADCs.

## Characteristics

* ESP-IDF: [v4.4.4](https://docs.espressif.com/projects/esp-idf/en/v4.4.4/esp32/index.html)
* Written in **C** using just the [ESP-IDF](https://github.com/espressif/esp-idf) framework.
* Uses the SPI driver instead of raw GPIO manipulation.

## Documentation

Everything is on the [wiki](https://github.com/gfurtadoalmeida/esp32-driver-mcp320x/wiki).

## Example

```cpp
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp32_driver_mcp320x/mcp320x.h"

void app_main(void)
{
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = GPIO_NUM_23,
        .miso_io_num = GPIO_NUM_19,
        .sclk_io_num = GPIO_NUM_18,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,
        .flags = SPICOMMON_BUSFLAG_MASTER};

    mcp320x_config_t mcp320x_cfg = {
        .host = SPI3_HOST,
        .device_model = MCP3204_MODEL,
        .clock_speed_hz = 1 * 1000 * 1000, // 1 Mhz.
        .reference_voltage = 5000,         // 5V
        .cs_io_num = GPIO_NUM_5};

    mcp320x_t *mcp320x_handle;
    uint16_t voltage;

    ESP_ERROR_CHECK(spi_bus_initialize(mcp320x_cfg.host, &bus_cfg, 0));
    
    ESP_ERROR_CHECK(mcp320x_initialize(&mcp320x_cfg, &mcp320x_handle));
    
    ESP_ERROR_CHECK(mcp320x_acquire(mcp320x_handle, portMAX_DELAY));
    
    ESP_ERROR_CHECK(mcp320x_read_voltage(mcp320x_handle, 
                                         MCP320X_CHANNEL_0, 
                                         MCP320X_READ_MODE_SINGLE,
                                         1000, 
                                         &voltage));
    
    ESP_ERROR_CHECK(mcp320x_release(mcp320x_handle));
    
    ESP_ERROR_CHECK(mcp320x_free(mcp320x_handle));

    ESP_LOGI("mcp320x", "Voltage: %d mV", voltage);
}
```
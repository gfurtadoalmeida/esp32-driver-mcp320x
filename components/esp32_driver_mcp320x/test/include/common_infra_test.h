#ifndef __COMMON_INFRA_TEST_H__
#define __COMMON_INFRA_TEST_H__

#include <stdint.h>
#include "unity.h"
#include "unity_test_runner.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp32_driver_mcp320x/esp32_driver_mcp320x.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define EXECUTE_WITH_HANDLE(code)               \
    mcp320x_handle_t handle;                    \
    mcp320x_initialize(&VALID_CONFIG, &handle); \
    code;                                       \
    mcp320x_free(handle);

    // Test hardware setup:
    // VSS = VDD = 5V
    // Channel 3 = 2.5V
    // Chip Select pin = GPIO 5

    static mcp320x_config_t VALID_CONFIG = {
        .host = SPI3_HOST,
        .device_model = MCP3204_MODEL,
        .clock_speed_hz = 1 * 1000 * 1000, // 1 Mhz.
        .reference_voltage = 5000,         // 5V
        .cs_io_num = GPIO_NUM_5};

#ifdef __cplusplus
}
#endif

#endif
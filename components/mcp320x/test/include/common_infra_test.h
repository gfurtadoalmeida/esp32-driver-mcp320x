#ifndef __COMMON_INFRA_TEST_H__
#define __COMMON_INFRA_TEST_H__

#include "unity.h"
#include "unity_test_runner.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "mcp320x.h"

#ifdef __cplusplus
extern "C"
{
#endif

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

    // It's horrible, but it's only being used
    // on parameter validation tests.

    static mcp320x_handle_t DUMMY_HANDLE = (mcp320x_t *)&VALID_CONFIG;

#ifdef __cplusplus
}
#endif

#endif //__COMMON_INFRA_TEST_H__
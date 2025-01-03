# ESP32 - MCP3204/3208 12-bit ADC Driver

[![GitHub Release Status][git-bagdge-release]][git-release] [![Bugs][sonar-badge-bugs]][sonar-home] [![Code Smells][sonar-badge-smells]][sonar-home] [![Security Rating][sonar-badge-security]][sonar-home] [![Quality Gate Status][sonar-badge-quality]][sonar-home]  

ESP32 driver for Microchip [MCP3204](https://www.microchip.com/en-us/product/MCP3204) and [MCP3208](https://www.microchip.com/en-us/product/MCP3208) 12-bit ADCs.

## Characteristics

* ESP-IDF: [v5.3](https://docs.espressif.com/projects/esp-idf/en/v5.3/esp32/index.html)
* Written in **C** using just the [ESP-IDF Framework](https://github.com/espressif/esp-idf).
* Testable: 30+ tests.

## Documentation

Everything is at the [docs](/docs) folder.

## Code Size

Build options:

* Compile optimized for size (`CONFIG_COMPILER_OPTIMIZATION_SIZE=y`).
* Error logging (`CONFIG_LOG_DEFAULT_LEVEL_ERROR=y`).

| DRAM (bss,data) | Flash (code,rodata) |
|:-:|:-:|
| 0 B | 1.39 KB |

## Example: Sampling Channel `0`

```cpp
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp32_driver_mcp320x/mcp320x.h"

void app_main(void)
{
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = GPIO_NUM_23,
        .miso_io_num = GPIO_NUM_19,
        .sclk_io_num = GPIO_NUM_18,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .data4_io_num = GPIO_NUM_NC,
        .data5_io_num = GPIO_NUM_NC,
        .data6_io_num = GPIO_NUM_NC,
        .data7_io_num = GPIO_NUM_NC,
        .max_transfer_sz = 3, // 24 bits.
        .flags = SPICOMMON_BUSFLAG_MASTER,
        .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,
        .intr_flags = ESP_INTR_FLAG_LEVEL3};

    mcp320x_config_t mcp320x_cfg = {
        .host = SPI3_HOST,
        .device_model = MCP3204_MODEL,
        .clock_speed_hz = 1 * 1000 * 1000, // 1 Mhz.
        .reference_voltage = 5000,         // 5V
        .cs_io_num = GPIO_NUM_22};

    // Bus initialization is up to the developer.
    spi_bus_initialize(mcp320x_cfg.host, &bus_cfg, 0);

    // Add the device to the SPI bus.
    mcp320x_t *mcp320x_handle = mcp320x_install(&mcp320x_cfg);

    // Occupy the SPI bus for multiple transactions.
    mcp320x_acquire(mcp320x_handle, portMAX_DELAY);

    uint16_t voltage = 0;

    for (size_t i = 0; i < 10; i++)
    {
        // Read voltage, sampling 1000 times.
        mcp320x_sample_voltage(mcp320x_handle,
                               MCP320X_CHANNEL_0,
                               MCP320X_READ_MODE_SINGLE,
                               1000,
                               &voltage);

        ESP_LOGI("mcp320x", "Voltage: %d mV", voltage);
    }

    // Unoccupy the SPI bus.
    mcp320x_release(mcp320x_handle);

    // Free resources.
    mcp320x_delete(mcp320x_handle);
}
```

## Contributing

To contribute to this project make sure to read our [CONTRIBUTING.md](/docs/CONTRIBUTING.md) file.

[git-bagdge-release]: https://github.com/gfurtadoalmeida/esp32-driver-mcp320x/actions/workflows/release.yml/badge.svg
[git-release]: https://github.com/gfurtadoalmeida/esp32-driver-mcp320x/releases
[sonar-badge-bugs]: https://sonarcloud.io/api/project_badges/measure?project=esp32_driver_mcp320x&metric=bugs
[sonar-badge-quality]: https://sonarcloud.io/api/project_badges/measure?project=esp32_driver_mcp320x&metric=alert_status
[sonar-badge-security]: https://sonarcloud.io/api/project_badges/measure?project=esp32_driver_mcp320x&metric=security_rating
[sonar-badge-smells]: https://sonarcloud.io/api/project_badges/measure?project=esp32_driver_mcp320x&metric=code_smells
[sonar-home]: https://sonarcloud.io/project/overview?id=esp32_driver_mcp320x

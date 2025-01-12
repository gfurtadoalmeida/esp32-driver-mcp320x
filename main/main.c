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
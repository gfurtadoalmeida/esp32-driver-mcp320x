#include <stdio.h>
#include "unity.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define GPIO_SCLK GPIO_NUM_18
#define GPIO_MISO GPIO_NUM_19
#define GPIO_MOSI GPIO_NUM_23

static void print_banner(const char *text);

void app_main(void)
{
    /* This is just the test runner.
     * The real tests are on "components/#/test".
     */

    // Necessary to initialize the SPI bus before running the tests.
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCLK,
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

    spi_bus_initialize(SPI3_HOST, &bus_cfg, 0);

    UNITY_BEGIN();

    unity_run_all_tests();

    UNITY_END();

    print_banner("Starting interactive test menu");

    /* This function will not return, and will be busy waiting for UART input.
     * That's why, in "test/sdkconfig the following configuration are disabled:
     * - CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0
     * - CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU1
     */
    unity_run_menu();
}

static void print_banner(const char *text)
{
    printf("\n#### %s #####\n\n", text);
}
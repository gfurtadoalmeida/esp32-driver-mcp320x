#include "esp32_driver_mcp320x/mcp320x.h"
#include "assertion.h"
#include "log.h"

/**
 * @struct mcp320x_t
 * @brief Holds control data for a context.
 */
struct mcp320x_t
{
    spi_device_handle_t spi_handle;       /** @brief SPI device handle. */
    mcp320x_model_t mcp_model;            /** @brief Device model. */
    float millivolts_per_resolution_step; /** @brief Millivolts per resolution step (Vref / MCP320X_RESOLUTION). */
};

mcp320x_t *mcp320x_install(mcp320x_config_t const *config)
{
    CMP_CHECK((config != NULL), "config error(NULL)", NULL)
    CMP_CHECK((config->reference_voltage >= MCP320X_REF_VOLTAGE_MIN), "reference voltage error(<MCP320X_REF_VOLTAGE_MIN)", NULL)
    CMP_CHECK((config->reference_voltage <= MCP320X_REF_VOLTAGE_MAX), "reference voltage error(>MCP320X_REF_VOLTAGE_MAX)", NULL)
    CMP_CHECK((config->clock_speed_hz >= MCP320X_CLOCK_MIN_HZ), "clock speed error(<MCP320X_CLOCK_MIN_HZ)", NULL)
    CMP_CHECK((config->clock_speed_hz <= MCP320X_CLOCK_MAX_HZ), "clock speed error(>MCP320X_CLOCK_MAX_HZ)", NULL)

    spi_device_interface_config_t dev_cfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0, // Clock idle: low, clock phase: leading, data write: CS on and CLK fall, data read: CS on and CLK rise.
        .clock_source = SPI_CLK_SRC_DEFAULT,
        .duty_cycle_pos = 128,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = (int)config->clock_speed_hz,
        .input_delay_ns = 0,
        .spics_io_num = config->cs_io_num,
        .flags = SPI_DEVICE_NO_DUMMY,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL};

    spi_device_handle_t spi_device_handle;

    CMP_CHECK(spi_bus_add_device(config->host, &dev_cfg, &spi_device_handle) == ESP_OK, "bus error(spi_bus_add_device)", NULL)

    mcp320x_t *dev = (mcp320x_t *)malloc(sizeof(mcp320x_t));
    dev->spi_handle = spi_device_handle;
    dev->mcp_model = config->device_model;
    dev->millivolts_per_resolution_step = (float)config->reference_voltage / (float)MCP320X_RESOLUTION;

    return dev;
}

mcp320x_err_t mcp320x_delete(mcp320x_t *handle)
{
    CMP_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE)
    CMP_CHECK(spi_bus_remove_device(handle->spi_handle) == ESP_OK, "bus error(spi_bus_remove_device)", MCP320X_ERR_SPI_BUS)

    free(handle);

    return MCP320X_OK;
}

mcp320x_err_t mcp320x_acquire(mcp320x_t *handle, TickType_t timeout)
{
    CMP_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE)
    CMP_CHECK((spi_device_acquire_bus(handle->spi_handle, timeout) == ESP_OK), "device error(spi_device_acquire_bus)", MCP320X_ERR_SPI_BUS_ACQUIRE)

    return MCP320X_OK;
}

mcp320x_err_t mcp320x_release(mcp320x_t *handle)
{
    CMP_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE)

    spi_device_release_bus(handle->spi_handle);

    return MCP320X_OK;
}

mcp320x_err_t mcp320x_get_actual_freq(mcp320x_t *handle,
                                      uint32_t *frequency_hz)
{
    CMP_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE)
    CMP_CHECK((frequency_hz != NULL), "frequency_hz error(NULL)", MCP320X_ERR_INVALID_VALUE_HANDLE)

    int calculated_freq_khz;

    CMP_CHECK((spi_device_get_actual_freq(handle->spi_handle, &calculated_freq_khz) == ESP_OK), "device error(spi_device_get_actual_freq)", MCP320X_ERR_FAIL)

    *frequency_hz = (uint32_t)calculated_freq_khz * 1000;

    return MCP320X_OK;
}

mcp320x_err_t mcp320x_read(mcp320x_t *handle,
                           mcp320x_channel_t channel,
                           mcp320x_read_mode_t read_mode,
                           uint16_t *value)
{
    CMP_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE)
    CMP_CHECK(((int)channel < (int)handle->mcp_model), "channel error(invalid)", MCP320X_ERR_INVALID_CHANNEL)
    CMP_CHECK((value != NULL), "value error(NULL)", MCP320X_ERR_INVALID_VALUE_HANDLE)

    spi_transaction_t transaction = {
        .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA,
        .cmd = 0,
        .addr = 0,
        .length = 24};

    // Request format (tx_data) is eight bits aligned.
    //
    // 0 0 0 0 0 1 MODE C2 _ C1 C0 S N D D D D _ D D D D D D D D
    // |-----------------|   |---------------|   |-------------|
    //
    // Where:
    //   * 0: filler bits, must be zero.
    //   * 1: start bit.
    //   * MODE:
    //     - 0: differential conversion.
    //     - 1: single conversion.
    //   * C [0 1 2]:
    //     -  0 0 0: channel 0
    //     -  0 0 1: channel 1
    //     -  0 1 0: channel 2
    //     -  0 1 1: channel 3
    //     -  1 0 0: channel 4
    //     -  1 0 1: channel 5
    //     -  1 1 0: channel 6
    //     -  1 1 1: channel 7
    //   * S: sample bit because one more clock is required to complete the sample and hold period.
    //   * N: low null bit.
    //   * D: data output bits.

    transaction.tx_data[0] = (uint8_t)(0b00000100 | (read_mode << 1) | (channel >> 2));
    transaction.tx_data[1] = (uint8_t)(channel << 6);
    transaction.tx_data[2] = 0;

    CMP_CHECK(spi_device_polling_transmit(handle->spi_handle, &transaction) == ESP_OK, "device error(spi_device_polling_transmit)", MCP320X_ERR_SPI_BUS)

    // Response format (rx_data):
    //
    // X X X X X X X X _ X X X 0 B11 B10 B9 B8 _ B7 B6 B5 B4 B3 B2 B1 B0
    // |-------------|   |-------------------|   |---------------------|
    //
    // Where:
    //   * X: dummy bits; any value.
    //   * 0: start bit.
    //   * B [0 1 2 3 4 5 6 7 8 9 10 11]: digital output code, uint16_t bits, big-endian.
    //     - B11: most significant bit.
    //     - B0: least significant bit.
    //
    // More information on section "6.1 Using the MCP3204/3208 with Microcontroller (MCU) SPI Ports"
    // of the MCP320X datasheet.
    //
    // Result logic, taking the following sequence as example:
    //
    // 1270 = X X X X X X X X _ X X X X 0 1 0 0 _ 1 1 1 1 0 1 1 0
    //        |--- rx[0] ---|   |--- rx[1] ---|   |--- rx[2] ---|
    //             dummy          first part        second part
    //
    // 1) Move first_part 8 bits to the left to open space for second_part.
    //    > first_part  = X X X X X 1 0 0 0 0 0 0 0 0 0 0
    //
    // 2) Concat first_part with second_part.
    //    > first_part  = X X X X X 1 0 0 0 0 0 0 0 0 0 0
    //    > second_part = 0 0 0 0 0 0 0 0 1 1 1 1 0 1 1 0
    //    > result      = X X X X X 1 0 0 1 1 1 1 0 1 1 0
    //
    // 3) Clear dummy bits.
    //    > result      = X X X X X 1 0 0 1 1 1 1 0 1 1 0
    //    > mask        = 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1
    //    > result      = 0 0 0 0 0 1 0 0 1 1 1 1 0 1 1 0

    const uint16_t first_part = transaction.rx_data[1];
    const uint16_t second_part = transaction.rx_data[2];

    *value = ((first_part << 8) | second_part) & 0b0000111111111111;

    return MCP320X_OK;
}

mcp320x_err_t mcp320x_read_voltage(mcp320x_t *handle,
                                   mcp320x_channel_t channel,
                                   mcp320x_read_mode_t read_mode,
                                   uint16_t *voltage)
{
    uint16_t value = 0;

    mcp320x_err_t result = mcp320x_read(handle, channel, read_mode, &value);

    *voltage = (uint16_t)(value * handle->millivolts_per_resolution_step);

    return result;
}

mcp320x_err_t mcp320x_sample(mcp320x_t *handle,
                             mcp320x_channel_t channel,
                             mcp320x_read_mode_t read_mode,
                             uint16_t sample_count,
                             uint16_t *value)
{
    CMP_CHECK((sample_count > 0), "sample_count error(0)", MCP320X_ERR_INVALID_SAMPLE_COUNT)

    uint32_t sum = 0;
    uint16_t sample = 0;

    for (uint16_t i = 0; i < sample_count; i++)
    {
        mcp320x_err_t result = mcp320x_read(handle, channel, read_mode, &sample);

        if (result != MCP320X_OK)
        {
            return result;
        }

        sum += sample;
    }

    *value = (uint16_t)(sum / sample_count);

    return MCP320X_OK;
}

mcp320x_err_t mcp320x_sample_voltage(mcp320x_t *handle,
                                     mcp320x_channel_t channel,
                                     mcp320x_read_mode_t read_mode,
                                     uint16_t sample_count,
                                     uint16_t *voltage)
{
    uint16_t sample = 0;

    mcp320x_err_t result = mcp320x_sample(handle, channel, read_mode, sample_count, &sample);

    *voltage = (uint16_t)(sample * handle->millivolts_per_resolution_step);

    return result;
}

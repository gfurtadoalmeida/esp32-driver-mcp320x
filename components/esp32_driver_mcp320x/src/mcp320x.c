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
    CMP_CHECK((config->reference_voltage <= MCP320X_REF_VOLTAGE_MAX), "reference voltage error(>MCP320X_REF_VOLTAGE_MAX)", NULL)
    CMP_CHECK((config->reference_voltage >= MCP320X_REF_VOLTAGE_MIN), "reference voltage error(<MCP320X_REF_VOLTAGE_MIN)", NULL)
    CMP_CHECK((config->clock_speed_hz <= MCP320X_CLOCK_MAX_HZ), "clock speed error(>MCP320X_CLOCK_MAX_HZ)", NULL)
    CMP_CHECK((config->clock_speed_hz >= MCP320X_CLOCK_MIN_HZ), "clock speed error(<MCP320X_CLOCK_MIN_HZ)", NULL)

    spi_device_interface_config_t dev_cfg = {
        .command_bits = 0,
        .address_bits = 0,
        .clock_speed_hz = (int)config->clock_speed_hz,
        .mode = 0,
        .queue_size = 1,
        .spics_io_num = config->cs_io_num,
        .input_delay_ns = 0,
        .pre_cb = NULL,
        .post_cb = NULL,
        .flags = SPI_DEVICE_NO_DUMMY};

    spi_device_handle_t spi_device_handle;

    CMP_CHECK(spi_bus_add_device(config->host, &dev_cfg, &spi_device_handle) == ESP_OK, "failed to add device to SPI bus", NULL)

    mcp320x_t *dev = (mcp320x_t *)malloc(sizeof(mcp320x_t));
    dev->spi_handle = spi_device_handle;
    dev->mcp_model = config->device_model;
    dev->millivolts_per_resolution_step = (float)config->reference_voltage / (float)MCP320X_RESOLUTION;

    return dev;
}

mcp320x_err_t mcp320x_delete(mcp320x_t *handle)
{
    CMP_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE)
    CMP_CHECK(spi_bus_remove_device(handle->spi_handle) == ESP_OK, "failed to remove device from bus", MCP320X_ERR_SPI_BUS)

    free(handle);

    return MCP320X_OK;
}

mcp320x_err_t mcp320x_acquire(mcp320x_t *handle, TickType_t timeout)
{
    CMP_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE)
    CMP_CHECK((spi_device_acquire_bus(handle->spi_handle, timeout) == ESP_OK), "bus error(acquire)", MCP320X_ERR_SPI_BUS_ACQUIRE)

    return MCP320X_OK;
}

mcp320x_err_t mcp320x_release(mcp320x_t *handle)
{
    CMP_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE)

    spi_device_release_bus(handle->spi_handle);

    return MCP320X_OK;
}

mcp320x_err_t mcp320x_read(mcp320x_t *handle,
                           mcp320x_channel_t channel,
                           mcp320x_read_mode_t read_mode,
                           uint16_t sample_count,
                           uint16_t *value)
{
    // Request format (tx_data) is eight bits aligned.
    //
    // 0 0 0 0 0 1 SG/DIFF D2 _ D1 D0 X X X X X X _ X X X X X X X X
    // |--------------------|   |---------------|   |-------------|
    //
    // Where:
    //   * 0: dummy bits, must be zero.
    //   * 1: start bit.
    //   * SG/DIFF:
    //     - 0: differential conversion.
    //     - 1: single conversion.
    //   * D [0 1 2]:
    //     -  0 0 0: channel 0
    //     -  0 0 1: channel 1
    //     -  0 1 0: channel 2
    //     -  0 1 1: channel 3
    //     -  1 0 0: channel 4
    //     -  1 0 1: channel 5
    //     -  1 1 0: channel 6
    //     -  1 1 1: channel 7
    //   * X: dummy bits, any value.
    //
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
    // Digital output code = (Vin x MCP320X_RESOLUTION) / Vref
    //
    // More information on section "6.1 Using the MCP3204/3208 with Microcontroller (MCU) SPI Ports"
    // of the MCP320X datasheet.

    CMP_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE)
    CMP_CHECK((sample_count > 0), "sample_count error(0)", MCP320X_ERR_INVALID_SAMPLE_COUNT)
    CMP_CHECK((value != NULL), "value error(NULL)", MCP320X_ERR_INVALID_VALUE_HANDLE)

    uint32_t sum = 0;
    spi_transaction_t transaction = {
        .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA,
        .length = 24};

    transaction.tx_data[0] = (uint8_t)((1 << 2) | (read_mode << 1) | ((channel & 4) >> 2));
    transaction.tx_data[1] = (uint8_t)(channel << 6);
    transaction.tx_data[2] = 0;

    for (uint16_t i = 0; i < sample_count; i++)
    {
        CMP_CHECK(spi_device_transmit(handle->spi_handle, &transaction) == ESP_OK, "communication error(transmit)", MCP320X_ERR_SPI_BUS)

        // Result logic, taking the following sequence as example:
        //
        // 0 1 0 0 0 1 0 0 _ 1 0 1 0 0 1 0 0 _ 1 1 1 1 0 1 1 0 = 1270
        // |--- rx[0] ---|   |--- rx[1] ---|   |--- rx[2] ---|
        //
        // 1) As bits 5-7 from rx_data[1] can be any value (bit 4 is always zero),
        //    AND it with 15 (00001111) to zero them.
        //    > first_part  = 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0
        //    > second_part = 0 0 0 0 0 0 0 0 1 1 1 1 0 1 1 0
        //
        // 2) Move rx_data[1] value 8 bits to the left to open space for second_part.
        //    > first_part  = 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0
        //    > second_part = 0 0 0 0 0 0 0 0 1 1 1 1 0 1 1 0
        //
        // 3) Concat (ORing) first_part with second_part.
        //    > first_part  = 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0
        //    > second_part = 0 0 0 0 0 0 0 0 1 1 1 1 0 1 1 0
        //    > result      = 0 0 0 0 0 1 0 0 1 1 1 1 0 1 1 0

        const uint16_t first_part = transaction.rx_data[1];
        const uint16_t second_part = transaction.rx_data[2];

        sum += ((first_part & 15) << 8) | second_part;
    }

    *value = (uint16_t)(sum / sample_count);

    return MCP320X_OK;
}

mcp320x_err_t mcp320x_read_voltage(mcp320x_t *handle,
                                   mcp320x_channel_t channel,
                                   mcp320x_read_mode_t read_mode,
                                   uint16_t sample_count,
                                   uint16_t *voltage)
{
    uint16_t value_read = 0;

    mcp320x_err_t result = mcp320x_read(handle, channel, read_mode, sample_count, &value_read);

    if (result != MCP320X_OK)
    {
        return result;
    }

    *voltage = (uint16_t)(value_read * handle->millivolts_per_resolution_step);

    return MCP320X_OK;
}
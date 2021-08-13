#include "check.h"
#include "mcp320x.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Core sampling function.
     *
     * @param handle MCP320X context handle.
     * @param channel Channel to read from.
     * @param read_mode Read mode.
     * @param sample_count How many samples to take.
     * @param value Pointer to where the value will be stored.
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    static mcp320x_err_t _mcp320x_sample_raw(mcp320x_handle_t handle,
                                             mcp320x_channel_t channel,
                                             mcp320x_read_mode_t read_mode,
                                             unsigned short sample_count,
                                             unsigned short *value);

    /**
     * @struct mcp320x_t
     * @brief Holds control data for a context.
     */
    struct mcp320x_t
    {
        spi_device_handle_t spi_handle;       /*!< SPI device handle. */
        mcp320x_model_t mcp_model;            /*!< Device model. */
        float millivolts_per_resolution_step; /*!< Millivolts per resolution step (reference voltage / ADC resolution). */
    };

    mcp320x_err_t mcp320x_initialize(mcp320x_config_t *config, mcp320x_handle_t *handle)
    {
        MCP320X_CHECK((config != NULL), "config error(NULL)", MCP320X_ERR_INVALID_CONFIG_HANDLE);
        MCP320X_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE);
        MCP320X_CHECK((config->reference_voltage <= MCP320X_REF_VOLTAGE_MAX), "reference voltage error(>MCP320X_REF_VOLTAGE_MAX)", MCP320X_ERR_INVALID_REFERENCE_VOLTAGE);
        MCP320X_CHECK((config->reference_voltage >= MCP320X_REF_VOLTAGE_MIN), "reference voltage error(<MCP320X_REF_VOLTAGE_MIN)", MCP320X_ERR_INVALID_REFERENCE_VOLTAGE);
        MCP320X_CHECK((config->clock_speed_hz <= MCP320X_CLOCK_MAX_HZ), "clock speed error(>MCP320X_CLOCK_MAX_HZ)", MCP320X_ERR_INVALID_CLOCK_SPEED);
        MCP320X_CHECK((config->clock_speed_hz >= MCP320X_CLOCK_MIN_HZ), "clock speed error(<MCP320X_CLOCK_MIN_HZ)", MCP320X_ERR_INVALID_CLOCK_SPEED);

        spi_device_interface_config_t dev_cfg = {
            .command_bits = 0,
            .address_bits = 0,
            .clock_speed_hz = config->clock_speed_hz,
            .mode = 0,
            .queue_size = 1,
            .spics_io_num = config->cs_io_num,
            .input_delay_ns = 0,
            .pre_cb = NULL,
            .post_cb = NULL,
            .flags = SPI_DEVICE_NO_DUMMY};

        spi_device_handle_t spi_device_handle;

        MCP320X_CHECK(spi_bus_add_device(config->host, &dev_cfg, &spi_device_handle) == ESP_OK, "failed to add device to SPI bus", MCP320X_ERR_SPI_BUS);

        mcp320x_handle_t dev = (mcp320x_t *)malloc(sizeof(mcp320x_t));
        dev->spi_handle = spi_device_handle;
        dev->mcp_model = config->device_model;
        dev->millivolts_per_resolution_step = (float)config->reference_voltage / MCP320X_RESOLUTION;

        *handle = dev;

        return MCP320X_OK;
    }

    mcp320x_err_t mcp320x_free(mcp320x_handle_t handle)
    {
        MCP320X_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE);
        MCP320X_CHECK(spi_bus_remove_device(handle->spi_handle) == ESP_OK, "failed to remove device from bus", MCP320X_ERR_SPI_BUS);

        free(handle);

        return MCP320X_OK;
    }

    mcp320x_err_t mcp320x_acquire(mcp320x_handle_t handle, TickType_t wait)
    {
        MCP320X_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE);
        MCP320X_CHECK((spi_device_acquire_bus(handle->spi_handle, wait) == ESP_OK), "bus error(acquire)", MCP320X_ERR_SPI_BUS_ACQUIRE);

        return MCP320X_OK;
    }

    mcp320x_err_t mcp320x_release(mcp320x_handle_t handle)
    {
        MCP320X_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE);

        spi_device_release_bus(handle->spi_handle);

        return MCP320X_OK;
    }

    mcp320x_err_t mcp320x_read_raw(mcp320x_handle_t handle,
                                   mcp320x_channel_t channel,
                                   mcp320x_read_mode_t read_mode,
                                   unsigned short *value)
    {
        MCP320X_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE);
        MCP320X_CHECK((int)channel < (int)handle->mcp_model, "channel error(not supported)", MCP320X_ERR_INVALID_CHANNEL);
        MCP320X_CHECK((value != NULL), "value error(NULL)", MCP320X_ERR_INVALID_VALUE_HANDLE);

        return _mcp320x_sample_raw(handle, channel, read_mode, 1, value);
    }

    mcp320x_err_t mcp320x_sample_raw(mcp320x_handle_t handle,
                                     mcp320x_channel_t channel,
                                     mcp320x_read_mode_t read_mode,
                                     unsigned short sample_count,
                                     unsigned short *value)
    {
        MCP320X_CHECK((handle != NULL), "handle error(NULL)", MCP320X_ERR_INVALID_HANDLE);
        MCP320X_CHECK((int)channel < (int)handle->mcp_model, "channel error(not supported)", MCP320X_ERR_INVALID_CHANNEL);
        MCP320X_CHECK((value != NULL), "value error(NULL)", MCP320X_ERR_INVALID_VALUE_HANDLE);

        return _mcp320x_sample_raw(handle, channel, read_mode, sample_count, value);
    }

    mcp320x_err_t mcp320x_read_voltage(mcp320x_handle_t handle,
                                       mcp320x_channel_t channel,
                                       mcp320x_read_mode_t read_mode,
                                       unsigned short *value)
    {
        MCP320X_CHECK((value != NULL), "value error(NULL)", MCP320X_ERR_INVALID_VALUE_HANDLE);

        unsigned short raw_value;

        mcp320x_err_t err = mcp320x_read_raw(handle, channel, read_mode, &raw_value);

        if (err != MCP320X_OK)
        {
            return err;
        }

        *value = (unsigned short)(raw_value * handle->millivolts_per_resolution_step);

        return MCP320X_OK;
    }

    mcp320x_err_t mcp320x_sample_voltage(mcp320x_handle_t handle,
                                         mcp320x_channel_t channel,
                                         mcp320x_read_mode_t read_mode,
                                         unsigned short sample_count,
                                         unsigned short *value)
    {
        MCP320X_CHECK((value != NULL), "value error(NULL)", MCP320X_ERR_INVALID_VALUE_HANDLE);

        unsigned short raw_value;

        mcp320x_err_t err = mcp320x_sample_raw(handle, channel, read_mode, sample_count, &raw_value);

        if (err != MCP320X_OK)
        {
            return err;
        }

        *value = (unsigned short)(raw_value * handle->millivolts_per_resolution_step);

        return MCP320X_OK;
    }

    static mcp320x_err_t _mcp320x_sample_raw(mcp320x_handle_t handle,
                                             mcp320x_channel_t channel,
                                             mcp320x_read_mode_t read_mode,
                                             unsigned short sample_count,
                                             unsigned short *value)
    {
        // Request format (tx_data), eight bits aligned.
        //
        // 0 0 0 0 0 1 SG/DIFF D2 _ D1 D0 X X X X X X  _ X X X X X X X X
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
        //   * X: dummy bits, whatever value.
        //
        // Response format (rx_data):
        //
        // X X X X X X X X _ X X X 0 B11 B10 B9 B8 _ B7 B6 B5 B4 B3 B2 B1 B0
        //
        // Where:
        //   * X: dummy bits, whatever value.
        //   * 0: start bit.
        //   * B [0 1 2 3 4 5 6 7 8 9 10 11]: unsigned short bits, big-endian.
        //     - B11: most significant bit.
        //     - B0: least significant bit.
        //
        // More information on section "6.1 Using the MCP3204/3208 with Microcontroller (MCU) SPI Ports"
        // of the MCP320X datasheet.

        uint32_t sum = 0;
        spi_transaction_t transaction = {
            .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA,
            .length = 24 // 24 bits.
        };

        transaction.tx_data[0] = (1 << 2) | (read_mode << 1) | ((channel & 4) >> 2);
        transaction.tx_data[1] = channel << 6;
        transaction.tx_data[2] = 0;

        for (uint16_t i = 0; i < sample_count; i++)
        {
            MCP320X_CHECK(spi_device_transmit(handle->spi_handle, &transaction) == ESP_OK, "communication error(transmit)", MCP320X_ERR_SPI_BUS);

            // As bits 5-7, from rx_data[1], can be any value (bit 4 is always zero),
            // AND it with 15 (1111) to zero them.
            sum += ((transaction.rx_data[1] & 15) << 8) | transaction.rx_data[2];
        }

        *value = sum / sample_count;

        return MCP320X_OK;
    }
#ifdef __cplusplus
}
#endif

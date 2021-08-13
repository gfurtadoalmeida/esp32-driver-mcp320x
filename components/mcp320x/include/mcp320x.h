#ifndef __MCP320X_H__
#define __MCP320X_H__

#include "driver/gpio.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // Constants

#define MCP320X_RESOLUTION 4095      /*!< 12 bits = (2^12)-1 = 4095. */
#define MCP320X_CLOCK_MIN_HZ 10000   /*!< Min recommended clock speed for a reliable reading = 10Khz. */
#define MCP320X_CLOCK_MAX_HZ 2000000 /*!< Max clock speed supported = 2Mhz at 5V. */
#define MCP320X_REF_VOLTAGE_MIN 250  /*!< Min reference voltage, in mV = 250mV. */
#define MCP320X_REF_VOLTAGE_MAX 7000 /*!< Max reference voltage, in mV = 7000mV. The max safe voltage is 5000mV. */

    // Result codes

#define MCP320X_OK ESP_OK                        /*!< Success. */
#define MCP320X_ERR_FAIL ESP_FAIL                /*!< Failure: generic. */
#define MCP320X_ERR_INVALID_HANDLE 10            /*!< Failure: invalid handle. */
#define MCP320X_ERR_INVALID_CONFIG_HANDLE 11     /*!< Failure: invalid configuration handle. */
#define MCP320X_ERR_INVALID_VALUE_HANDLE 12      /*!< Failure: invalid value handle. */
#define MCP320X_ERR_INVALID_CLOCK_SPEED 20       /*!< Failure: invalid clock speed. */
#define MCP320X_ERR_INVALID_CHANNEL 21           /*!< Failure: invalid channel. */
#define MCP320X_ERR_INVALID_REFERENCE_VOLTAGE 22 /*!< Failure: invalid reference voltage. */
#define MCP320X_ERR_SPI_BUS 30                   /*!< Failure: error communicating with SPI bus. */
#define MCP320X_ERR_SPI_BUS_ACQUIRE 31           /*!< Failure: error communicating with SPI bus to acquire it. */

    /**
     * @typedef mcp320x_err_t
     * @brief Response code.
     */
    typedef int mcp320x_err_t;

    /**
     * @typedef mcp320x_t
     * @brief MCP320X context.
     */
    typedef struct mcp320x_t mcp320x_t;

    /**
     * @typedef nextion_handle_t
     * @brief Pointer to a MCP320X context.
     */
    typedef mcp320x_t *mcp320x_handle_t;

    /**
     * @typedef mcp320x_model_t
     * @brief MCP320X model.
     */
    typedef enum
    {
        MCP3204_MODEL = 4, /*!< 4 channels model. */
        MCP3208_MODEL = 8  /*!< 8 channels model. */
    } mcp320x_model_t;

    /**
     * @typedef mcp320x_channel_t
     * @brief MPC320X channel.
     */
    typedef enum
    {
        MCP320X_CHANNEL_0 = 0,
        MCP320X_CHANNEL_1 = 1,
        MCP320X_CHANNEL_2 = 2,
        MCP320X_CHANNEL_3 = 3,
        MCP320X_CHANNEL_4 = 4,
        MCP320X_CHANNEL_5 = 5,
        MCP320X_CHANNEL_6 = 6,
        MCP320X_CHANNEL_7 = 7
    } mcp320x_channel_t;

    /**
     * @typedef mcp320x_read_mode_t
     * @brief MCP320X read mode.
     */
    typedef enum
    {
        MCP320X_READ_MODE_DIFFERENTIAL = 0,
        MCP320X_READ_MODE_SINGLE = 1
    } mcp320x_read_mode_t;

    /**
     * @typedef mcp320x_config_t
     * @brief Configuration for a MCP320X IC.
     */
    typedef struct
    {
        spi_host_device_t host;       /*!< SPI peripheral used to communicate with the device. */
        mcp320x_model_t device_model; /*!< MCP320X model used with this configuration. */
        int clock_speed_hz;           /*!< Clock speed, in Hz. Recommended the use of divisors of 80MHz. */
        int reference_voltage;        /*!< Reference voltage, in millivolts. */
        gpio_num_t cs_io_num;         /*!< GPIO pin used for Chip Select (CS). */
    } mcp320x_config_t;

    /**
     * @brief Adds a MCP320X device to a SPI bus.
     * @param config Pointer to a mcp320x_config_t struct specifying how the device should be initialized.
     * @param handle Pointer to where a MCP320X handle will be stored.
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_initialize(mcp320x_config_t *config, mcp320x_handle_t *handle);

    /**
     * @brief Removes a MCP320X device from a SPI bus.
     * @param handle MCP320X context handle.
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_free(mcp320x_handle_t handle);

    /**
     * @brief Occupies the SPI bus for continuous readings.
     *
     * @note This function is not thread safe when multiple tasks access the same device.
     *
     * @param handle MCP320X context handle.
     * @param wait Time to wait before the the bus is occupied by the device. Currently MUST set to portMAX_DELAY.
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_acquire(mcp320x_handle_t handle, TickType_t wait);

    /**
     * @brief Releases the SPI bus occupied by the ADC. All other devices on the bus can start sending transactions.
     *
     * @note This function is not thread safe when multiple tasks access the same device.
     *
     * @param handle MCP320X context handle.
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_release(mcp320x_handle_t handle);

    /**
     * @brief Reads the raw value from a channel; reads a value from 0 to 4095 (MCP320X_RESOLUTION).
     *
     * @note This function is not thread safe when multiple tasks access the same device.
     *
     * @param handle MCP320X context handle.
     * @param channel Channel to read from.
     * @param read_mode Read mode.
     * @param value Pointer to where the value will be stored.
     *
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_read_raw(mcp320x_handle_t handle,
                                   mcp320x_channel_t channel,
                                   mcp320x_read_mode_t read_mode,
                                   unsigned short *value);

    /**
     * @brief Samples a channel; reads a value from 0 to 4095 (MCP320X_RESOLUTION).
     *
     * @note This function is not thread safe when multiple tasks access the same device.
     *
     * @param handle MCP320X context handle.
     * @param channel Channel to read from.
     * @param read_mode Read mode.
     * @param sample_count How many samples to take.
     * @param value Pointer to where the value will be stored.
     *
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_sample_raw(mcp320x_handle_t handle,
                                     mcp320x_channel_t channel,
                                     mcp320x_read_mode_t read_mode,
                                     unsigned short sample_count,
                                     unsigned short *value);
    /**
     * @brief Reads the converted voltage, in millivolts, from a channel.
     *
     * @note This function is not thread safe when multiple tasks access the same device.
     *
     * @param handle MCP320X context handle.
     * @param channel Channel to read from.
     * @param read_mode Read mode.
     * @param value Pointer to where the value will be stored.
     *
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_read_voltage(mcp320x_handle_t handle,
                                       mcp320x_channel_t channel,
                                       mcp320x_read_mode_t read_mode,
                                       unsigned short *value);

    /**
     * @brief Samples and converts a raw value to voltage, in millivolts, from a channel.
     *
     * @note This function is not thread safe when multiple tasks access the same device.
     *
     * @param handle MCP320X context handle.
     * @param channel Channel to read from.
     * @param read_mode Read mode.
     * @param sample_count How many samples to take.
     * @param value Pointer to where the value will be stored.
     *
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_sample_voltage(mcp320x_handle_t handle,
                                         mcp320x_channel_t channel,
                                         mcp320x_read_mode_t read_mode,
                                         unsigned short sample_count,
                                         unsigned short *value);

#ifdef __cplusplus
}
#endif

#endif //__MCP320X_H__

#ifndef __ESP32_DRIVER_MCP320X_MCP320X_H__
#define __ESP32_DRIVER_MCP320X_MCP320X_H__

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // Constants

#define MCP320X_RESOLUTION 4096                /** @brief ADC resolution = 12 bits = 2^12 = 4096 steps */
#define MCP320X_CLOCK_MIN_HZ (10 * 1000)       /** @brief Minimum recommended clock speed for a reliable reading = 10Khz. */
#define MCP320X_CLOCK_MAX_HZ (2 * 1000 * 1000) /** @brief Maximum clock speed supported = 2Mhz at 5V. */
#define MCP320X_REF_VOLTAGE_MIN 250            /** @brief Minimum reference voltage, in mV = 250mV. */
#define MCP320X_REF_VOLTAGE_MAX 7000           /** @brief Maximum reference voltage, in mV = 7000mV. The max safe voltage is 5000mV. */

    // Result codes

#define MCP320X_OK ESP_OK                   /** @brief Success. */
#define MCP320X_ERR_FAIL ESP_FAIL           /** @brief Failure: generic. */
#define MCP320X_ERR_INVALID_HANDLE 10       /** @brief Failure: invalid handle. */
#define MCP320X_ERR_INVALID_VALUE_HANDLE 11 /** @brief Failure: invalid value handle. */
#define MCP320X_ERR_INVALID_SAMPLE_COUNT 12 /** @brief Failure: invalid sample count. */
#define MCP320X_ERR_INVALID_CHANNEL 20      /** @brief Failure: invalid channel. */
#define MCP320X_ERR_SPI_BUS 30              /** @brief Failure: error communicating with SPI bus. */
#define MCP320X_ERR_SPI_BUS_ACQUIRE 31      /** @brief Failure: error communicating with SPI bus to acquire it. */

    /**
     * @typedef mcp320x_err_t
     * @brief Response code.
     */
    typedef esp_err_t mcp320x_err_t;

    /**
     * @typedef mcp320x_t
     * @brief MCP320X context.
     */
    typedef struct mcp320x_t mcp320x_t;

    /**
     * @typedef mcp320x_model_t
     * @brief MCP320X model.
     */
    typedef enum
    {
        MCP3204_MODEL = 4, /** @brief 4 channels model. */
        MCP3208_MODEL = 8  /** @brief 8 channels model. */
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
        spi_host_device_t host;       /** @brief SPI peripheral used to communicate with the device. */
        gpio_num_t cs_io_num;         /** @brief GPIO pin used for Chip Select (CS). */
        mcp320x_model_t device_model; /** @brief MCP320X model used with this configuration. */
        uint32_t clock_speed_hz;      /** @brief Clock speed, in Hz. Recommended the use of divisors of 80MHz. */
        uint16_t reference_voltage;   /** @brief Reference voltage, in millivolts. */
    } mcp320x_config_t;

    /**
     * @brief Add a MCP320X device to an already configured SPI bus.
     * @param[in] config Pointer to a @ref mcp320x_config_t struct specifying how the device should be initialized.
     * @return Valid pointer, otherwise NULL.
     */
    mcp320x_t *mcp320x_install(mcp320x_config_t const *config);

    /**
     * @brief Remove a MCP320X device from a SPI bus.
     * @param[in] handle MCP320X handle.
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_delete(mcp320x_t *handle);

    /**
     * @brief Occupy the SPI bus for continuous readings.
     * @note The bus must be released using the @ref mcp320x_release function.
     * @note This function is not thread safe when multiple tasks access the same SPI device.
     * @param[in] handle MCP320X handle.
     * @param[in] timeout Time to wait before the bus is occupied by the device. Currently MUST BE set to portMAX_DELAY.
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_acquire(mcp320x_t *handle, TickType_t timeout);

    /**
     * @brief Release the SPI bus occupied by the ADC. All other devices on the bus can start sending transactions.
     * @note The bus must be acquired using the @ref mcp320x_acquire function.
     * @note This function is not thread safe when multiple tasks access the same SPI device.
     * @param[in] handle MCP320X handle.
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_release(mcp320x_t *handle);

    /**
     * @brief Get the actual working frequency, in Hertz.
     * @param[in] handle MCP320X handle.
     * @param[out] frequency_hz Pointer to where the frequency in Hertz will be stored.
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_get_actual_freq(mcp320x_t *handle, uint32_t *frequency_hz);

    /**
     * @brief Read a digital code from 0 to 4096 (MCP320X_RESOLUTION).
     * @note This function is not thread safe when multiple tasks access the same SPI device.
     * @param[in] handle MCP320X handle.
     * @param[in] channel Channel to read from.
     * @param[in] read_mode Read mode.
     * @param[out] value Pointer to where the value will be stored.
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_read(mcp320x_t *handle,
                               mcp320x_channel_t channel,
                               mcp320x_read_mode_t read_mode,
                               uint16_t *value);

    /**
     * @brief Read a voltage, in millivolts.
     * @note This function is not thread safe when multiple tasks access the same SPI device.
     * @param[in] handle MCP320X handle.
     * @param[in] channel Channel to read from.
     * @param[in] read_mode Read mode.
     * @param[out] voltage Pointer to where the value will be stored.
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_read_voltage(mcp320x_t *handle,
                                       mcp320x_channel_t channel,
                                       mcp320x_read_mode_t read_mode,
                                       uint16_t *voltage);

    /**
     * @brief Sample a channel, returning a digital code from 0 to 4096 (MCP320X_RESOLUTION).
     * @note For high \p sample_count it's recommended to aquire the SPI bus using the @ref mcp320x_acquire function.
     * @note This function is not thread safe when multiple tasks access the same SPI device.
     * @param[in] handle MCP320X handle.
     * @param[in] channel Channel to read from.
     * @param[in] read_mode Read mode.
     * @param[in] sample_count How many samples to take.
     * @param[out] value Pointer to where the value will be stored.
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_sample(mcp320x_t *handle,
                                 mcp320x_channel_t channel,
                                 mcp320x_read_mode_t read_mode,
                                 uint16_t sample_count,
                                 uint16_t *value);

    /**
     * @brief Sample a channel, returning a voltage, in millivolts.
     * @note For high \p sample_count it's recommended to aquire the SPI bus using the @ref mcp320x_acquire function.
     * @note This function is not thread safe when multiple tasks access the same SPI device.
     * @param[in] handle MCP320X handle.
     * @param[in] channel Channel to read from.
     * @param[in] read_mode Read mode.
     * @param[in] sample_count How many samples to take.
     * @param[out] voltage Pointer to where the value will be stored.
     * @return MCP320X_OK when success, otherwise any MCP320X_ERR* code.
     */
    mcp320x_err_t mcp320x_sample_voltage(mcp320x_t *handle,
                                         mcp320x_channel_t channel,
                                         mcp320x_read_mode_t read_mode,
                                         uint16_t sample_count,
                                         uint16_t *voltage);

#ifdef __cplusplus
}
#endif
#endif

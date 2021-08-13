#include "common_infra_test.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // ======
    // READ
    // ======

    TEST_CASE("Cannot read raw with invalid handle", "[read][raw]")
    {
        unsigned short value;

        mcp320x_err_t result = mcp320x_read_raw(NULL, MCP320X_CHANNEL_0, MCP320X_READ_MODE_SINGLE, &value);

        TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_HANDLE, result);
    }

    TEST_CASE("Cannot read raw with invalid channel", "[read][raw]")
    {
        unsigned short value;

        mcp320x_err_t result = mcp320x_read_raw(DUMMY_HANDLE, MCP320X_CHANNEL_7, MCP320X_READ_MODE_SINGLE, &value);

        TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_CHANNEL, result);
    }

    TEST_CASE("Cannot read raw with null value", "[read][raw]")
    {
        mcp320x_err_t result = mcp320x_read_raw(DUMMY_HANDLE, MCP320X_CHANNEL_0, MCP320X_READ_MODE_SINGLE, NULL);

        TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_VALUE_HANDLE, result);
    }

    TEST_CASE("Can read raw", "[read][raw]")
    {
        mcp320x_handle_t handle;

        mcp320x_initialize(&VALID_CONFIG, &handle);

        unsigned short value;

        mcp320x_err_t result = mcp320x_read_raw(handle, MCP320X_CHANNEL_3, MCP320X_READ_MODE_SINGLE, &value);

        mcp320x_free(handle);

        TEST_ASSERT_EQUAL(MCP320X_OK, result);

        // Will accept 2.5V +- 61mV.
        TEST_ASSERT_INT16_WITHIN(50, 2047, value);
    }

    // ======
    // SAMPLE
    // ======

    TEST_CASE("Cannot sample raw with invalid handle", "[read][raw]")
    {
        unsigned short value;

        mcp320x_err_t result = mcp320x_sample_raw(NULL, MCP320X_CHANNEL_0, MCP320X_READ_MODE_SINGLE, 5, &value);

        TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_HANDLE, result);
    }

    TEST_CASE("Cannot sample raw with invalid channel", "[read][raw]")
    {
        unsigned short value;

        mcp320x_err_t result = mcp320x_sample_raw(DUMMY_HANDLE, MCP320X_CHANNEL_7, MCP320X_READ_MODE_SINGLE, 5, &value);

        TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_CHANNEL, result);
    }

    TEST_CASE("Cannot sample raw with null value", "[read][raw]")
    {
        mcp320x_err_t result = mcp320x_sample_raw(DUMMY_HANDLE, MCP320X_CHANNEL_0, MCP320X_READ_MODE_SINGLE, 5, NULL);

        TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_VALUE_HANDLE, result);
    }

    TEST_CASE("Can sample raw", "[read][raw]")
    {
        mcp320x_handle_t handle;

        mcp320x_initialize(&VALID_CONFIG, &handle);

        unsigned short value;

        mcp320x_err_t result = mcp320x_sample_raw(handle, MCP320X_CHANNEL_3, MCP320X_READ_MODE_SINGLE, 5, &value);

        mcp320x_free(handle);

        TEST_ASSERT_EQUAL(MCP320X_OK, result);

        // Will accept 2.5V +- 61mV.
        TEST_ASSERT_INT16_WITHIN(50, 2047, value);
    }

#ifdef __cplusplus
}
#endif

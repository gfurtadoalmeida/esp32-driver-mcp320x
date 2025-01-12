#include "common_infra_test.h"

TEST_CASE("Cannot sample with invalid handle", "[sample]")
{
    uint16_t value;

    mcp320x_err_t result = mcp320x_sample(NULL, MCP320X_CHANNEL_0, MCP320X_READ_MODE_SINGLE, 5, &value);

    TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_HANDLE, result);
}

TEST_CASE("Cannot sample with invalid channel", "[sample]")
{
    uint16_t value;

    EXECUTE_WITH_HANDLE(mcp320x_err_t result = mcp320x_sample(handle, MCP320X_CHANNEL_7, MCP320X_READ_MODE_SINGLE, 5, &value))

    TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_CHANNEL, result);
}

TEST_CASE("Cannot sample with null value", "[sample]")
{
    EXECUTE_WITH_HANDLE(mcp320x_err_t result = mcp320x_sample(handle, MCP320X_CHANNEL_0, MCP320X_READ_MODE_SINGLE, 5, NULL))

    TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_VALUE_HANDLE, result);
}

TEST_CASE("Can sample", "[sample]")
{
    uint16_t value;

    EXECUTE_WITH_HANDLE(mcp320x_err_t result = mcp320x_sample(handle, MCP320X_CHANNEL_3, MCP320X_READ_MODE_SINGLE, 5, &value))

    TEST_ASSERT_EQUAL(MCP320X_OK, result);
    TEST_ASSERT_INT16_WITHIN(50, 2048, value); // Will accept 2.5V +- 50mV.
}

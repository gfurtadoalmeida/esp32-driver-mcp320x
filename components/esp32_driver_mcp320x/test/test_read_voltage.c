#include "common_infra_test.h"

TEST_CASE("Cannot read voltage with invalid handle", "[read][voltage]")
{
    uint16_t value;

    mcp320x_err_t result = mcp320x_read_voltage(NULL, MCP320X_CHANNEL_0, MCP320X_READ_MODE_SINGLE, &value);

    TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_HANDLE, result);
}

TEST_CASE("Cannot read voltage with invalid channel", "[read][voltage]")
{
    uint16_t value;

    EXECUTE_WITH_HANDLE(mcp320x_err_t result = mcp320x_read_voltage(handle, MCP320X_CHANNEL_7, MCP320X_READ_MODE_SINGLE, &value))

    TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_CHANNEL, result);
}

TEST_CASE("Cannot read voltage with null value", "[read][voltage]")
{
    EXECUTE_WITH_HANDLE(mcp320x_err_t result = mcp320x_read_voltage(handle, MCP320X_CHANNEL_0, MCP320X_READ_MODE_SINGLE, NULL));

    TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_VALUE_HANDLE, result);
}

TEST_CASE("Can read voltage", "[read][voltage]")
{
    uint16_t value;

    EXECUTE_WITH_HANDLE(mcp320x_err_t result = mcp320x_read_voltage(handle, MCP320X_CHANNEL_3, MCP320X_READ_MODE_SINGLE, &value))

    TEST_ASSERT_EQUAL(MCP320X_OK, result);
    TEST_ASSERT_INT16_WITHIN(50, 2500, value); // Will accept 2.5V +- 50mV.
}

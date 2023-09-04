#include "common_infra_test.h"

TEST_CASE("Cannot get frequency with invalid handle", "[freq]")
{
    uint32_t frequency;

    mcp320x_err_t result = mcp320x_get_actual_freq(NULL, &frequency);

    TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_HANDLE, result);
}

TEST_CASE("Cannot get frequency with null frequency", "[freq]")
{
    uint32_t frequency;

    EXECUTE_WITH_HANDLE(mcp320x_err_t result = mcp320x_get_actual_freq(handle, &frequency))

    TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_VALUE_HANDLE, result);
}

TEST_CASE("Can get frequency", "[freq]")
{
    uint32_t frequency;

    EXECUTE_WITH_HANDLE(mcp320x_err_t result = mcp320x_get_actual_freq(handle, &frequency))

    TEST_ASSERT_EQUAL(MCP320X_OK, result);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(0, frequency);
}

#include "common_infra_test.h"

// ======
// INIT
// ======

TEST_CASE("Cannot init with null configuration", "[init]")
{
    mcp320x_t *handle = mcp320x_install(NULL);

    TEST_ASSERT_NULL(handle);
}

TEST_CASE("Cannot init with null handle", "[init]")
{
    mcp320x_config_t cfg = {
        .clock_speed_hz = 1000000,
        .reference_voltage = 5000};

    mcp320x_t *handle = mcp320x_install(&cfg);

    TEST_ASSERT_NULL(handle);
}

TEST_CASE("Cannot init driver with low frequency", "[init]")
{
    mcp320x_config_t cfg = {
        .clock_speed_hz = MCP320X_CLOCK_MIN_HZ - 1,
        .reference_voltage = 5000};

    mcp320x_t *handle = mcp320x_install(&cfg);

    TEST_ASSERT_NULL(handle);
}

TEST_CASE("Cannot init with high frequency", "[init]")
{
    mcp320x_config_t cfg = {
        .clock_speed_hz = MCP320X_CLOCK_MAX_HZ + 1,
        .reference_voltage = 5000};

    mcp320x_t *handle = mcp320x_install(&cfg);

    TEST_ASSERT_NULL(handle);
}

TEST_CASE("Cannot init with low reference voltage", "[init]")
{
    mcp320x_config_t cfg = {
        .clock_speed_hz = 1000000,
        .reference_voltage = MCP320X_REF_VOLTAGE_MIN - 1};

    mcp320x_t *handle = mcp320x_install(&cfg);

    TEST_ASSERT_NULL(handle);
}

TEST_CASE("Cannot init with high reference voltage", "[init]")
{
    mcp320x_config_t cfg = {
        .clock_speed_hz = 1000000,
        .reference_voltage = MCP320X_REF_VOLTAGE_MAX + 1};

    mcp320x_t *handle = mcp320x_install(&cfg);

    TEST_ASSERT_NULL(handle);
}

TEST_CASE("Can init", "[init]")
{
    mcp320x_t *handle = mcp320x_install(&VALID_CONFIG);

    TEST_ASSERT_NOT_NULL(handle);

    mcp320x_delete(handle);
}

// ======
// FREE
// ======

TEST_CASE("Cannot free with null handle", "[free]")
{
    mcp320x_err_t result = mcp320x_delete(NULL);

    TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_HANDLE, result);
}

TEST_CASE("Can free", "[free]")
{
    mcp320x_t *handle = mcp320x_install(&VALID_CONFIG);
    mcp320x_err_t result = mcp320x_delete(handle);

    TEST_ASSERT_EQUAL(MCP320X_OK, result);
}

// =======
// ACQUIRE
// =======

TEST_CASE("Cannot acquire with null handle", "[acquire]")
{
    mcp320x_err_t result = mcp320x_acquire(NULL, portMAX_DELAY);

    TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_HANDLE, result);
}

TEST_CASE("Can acquire", "[acquire]")
{
    mcp320x_t *handle = mcp320x_install(&VALID_CONFIG);
    mcp320x_err_t result = mcp320x_acquire(handle, portMAX_DELAY);

    mcp320x_release(handle);
    mcp320x_delete(handle);

    TEST_ASSERT_EQUAL(MCP320X_OK, result);
}

// =======
// RELEASE
// =======

TEST_CASE("Cannot release with null handle", "[release]")
{
    mcp320x_err_t result = mcp320x_release(NULL);

    TEST_ASSERT_EQUAL(MCP320X_ERR_INVALID_HANDLE, result);
}

TEST_CASE("Can release", "[release]")
{
    mcp320x_t *handle = mcp320x_install(&VALID_CONFIG);
    mcp320x_acquire(handle, portMAX_DELAY);

    mcp320x_err_t result = mcp320x_release(handle);

    mcp320x_delete(handle);

    TEST_ASSERT_EQUAL(MCP320X_OK, result);
}

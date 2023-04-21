#ifndef __ESP32_DRIVER_MCP320X_ASSERTION_H__
#define __ESP32_DRIVER_MCP320X_ASSERTION_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "log.h"

/**
 * @brief Returns a given value if the condition passed is false.
 *
 * @param condition Condition to evaluate.
 * @param message Message to be logged if the condition is false.
 * @param return_value Value to be returned if the condition is false.
 *
 * @return If the condition is false the method will return what was
 * passed on @ref return_value parameter, otherwise the method will
 * continue.
 */
#define CMP_CHECK(condition, message, return_value)              \
    if (!(condition))                                            \
    {                                                            \
        CMP_LOGE("%s(%d): %s", __FUNCTION__, __LINE__, message); \
        return (return_value);                                   \
    }

#ifdef __cplusplus
}
#endif
#endif
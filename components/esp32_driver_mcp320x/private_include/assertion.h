#ifndef __ESP32_DRIVER_MCP320X_ASSERTION_H__
#define __ESP32_DRIVER_MCP320X_ASSERTION_H__

#include "log.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Return a given value if the condition passed is false.
 * @param condition Condition to evaluate.
 * @param message Message to be logged if the condition is false.
 * @param return_value Value to be returned if the condition is false.
 * @return \p return_value if the condition is false, otherwise the
 * method will continue.
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

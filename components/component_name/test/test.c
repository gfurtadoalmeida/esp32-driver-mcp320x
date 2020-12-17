#include "unity.h"
#include "unity_test_runner.h"

#ifdef __cplusplus
extern "C"
{
#endif

    TEST_CASE("Can pass this test", "[categoryA][categoryB]")
    {
        TEST_ASSERT_TRUE(true);
    }

#ifdef __cplusplus
}
#endif

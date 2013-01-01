#include "string_utils.h"
#include <gtest/gtest.h>

TEST(TestStringUtils, VectorStringEqual) {
    EXPECT_FALSE((vector<string>{"a"} == vector<string>{"a", "b"}));
    EXPECT_FALSE((vector<string>{"a", "b", "c"} == vector<string>{"a", "b", "d"}));
    EXPECT_TRUE((vector<string>{"a", "b", "c"} == vector<string>{"a", "b", "c"}));
}
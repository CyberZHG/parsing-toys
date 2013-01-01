#include "string_utils.h"
#include <gtest/gtest.h>

using namespace std;

TEST(TestStringUtils, VectorStringEqual) {
    EXPECT_FALSE((vector<string>{"a"} == vector<string>{"a", "b"}));
    EXPECT_FALSE((vector<string>{"a", "b", "c"} == vector<string>{"a", "b", "d"}));
    EXPECT_TRUE((vector<string>{"a", "b", "c"} == vector<string>{"a", "b", "c"}));
}

TEST(TestStringUtils, ToSubscripts) {
    EXPECT_EQ(string("₀"), toSubscript(0));
    EXPECT_EQ(string("₄₂"), toSubscript(42));
    EXPECT_EQ(string("₁₃₅₆₇₈₉"), toSubscript(1356789));
}
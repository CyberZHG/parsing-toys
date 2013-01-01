#include "re.h"
#include <gtest/gtest.h>

using namespace std;

TEST(TestRegexToNFA, Empty) {
    const RegularExpression re("ε");
    const auto nfa = re.toNFA();
    ASSERT_NE(nfa, nullptr);
    EXPECT_EQ(nfa->id, 0);
    EXPECT_EQ(nfa->type, "start");
    ASSERT_EQ(nfa->edges.size(), 1);
    EXPECT_EQ(nfa->edges[0].first, RegularExpression::EPSILON);
    EXPECT_EQ(nfa->edges[0].second->id, 1);
    EXPECT_EQ(nfa->edges[0].second->type, "accept");
}

TEST(TestRegexToNFA, Text) {
    const RegularExpression re("a");
    const auto nfa = re.toNFA();
    ASSERT_NE(nfa, nullptr);
    EXPECT_EQ(nfa->id, 0);
    EXPECT_EQ(nfa->type, "start");
    ASSERT_EQ(nfa->edges.size(), 1);
    EXPECT_EQ(nfa->edges[0].first, "a");
    EXPECT_EQ(nfa->edges[0].second->id, 1);
    EXPECT_EQ(nfa->edges[0].second->type, "accept");
}

TEST(TestRegexToNFA, Cat2) {
    const RegularExpression re("ab");
    const auto nfa = re.toNFA();
    ASSERT_NE(nfa, nullptr);
    EXPECT_EQ(nfa->id, 0);
    EXPECT_EQ(nfa->type, "start");
    ASSERT_EQ(nfa->edges.size(), 1);
    EXPECT_EQ(nfa->edges[0].first, "a");

    const auto node1 = nfa->edges[0].second;
    EXPECT_EQ(node1->id, 1);
    ASSERT_EQ(node1->edges.size(), 1);
    EXPECT_EQ(node1->edges[0].first, "b");
    EXPECT_EQ(node1->edges[0].second->id, 2);
    EXPECT_EQ(node1->edges[0].second->type, "accept");
}

TEST(TestRegexToNFA, Cat3) {
    const RegularExpression re("abc");
    const auto nfa = re.toNFA();
    ASSERT_NE(nfa, nullptr);
    EXPECT_EQ(nfa->edges[0].first, "a");

    const auto node1 = nfa->edges[0].second;
    EXPECT_EQ(node1->edges[0].first, "b");

    const auto node2 = node1->edges[0].second;
    EXPECT_EQ(node2->edges[0].first, "c");
    EXPECT_EQ(node2->edges[0].second->type, "accept");
}

TEST(TestRegexToNFA, Or) {
    RegularExpression re("a|b");
    auto nfa = re.toNFA();
    ASSERT_NE(nfa, nullptr);
    EXPECT_EQ(nfa->id, 0);
    EXPECT_EQ(nfa->type, "start");
    ASSERT_EQ(nfa->edges.size(), 2);
    EXPECT_EQ(nfa->edges[0].first, RegularExpression::EPSILON);
    EXPECT_EQ(nfa->edges[1].first, RegularExpression::EPSILON);

    auto branch1Start = nfa->edges[0].second;
    ASSERT_EQ(branch1Start->edges.size(), 1);
    EXPECT_EQ(branch1Start->edges[0].first, "a");
    auto branch1End = branch1Start->edges[0].second;
    ASSERT_EQ(branch1End->edges.size(), 1);
    EXPECT_EQ(branch1End->edges[0].first, RegularExpression::EPSILON);
    EXPECT_EQ(branch1End->edges[0].second->type, "accept");

    auto branch2Start = nfa->edges[1].second;
    ASSERT_EQ(branch2Start->edges.size(), 1);
    EXPECT_EQ(branch2Start->edges[0].first, "b");
}

TEST(TestRegexToNFA, Star) {
    RegularExpression re("a*");
    auto nfa = re.toNFA();
    ASSERT_NE(nfa, nullptr);
    EXPECT_EQ(nfa->id, 0);
    EXPECT_EQ(nfa->type, "start");
    ASSERT_EQ(nfa->edges.size(), 2);
    EXPECT_EQ(nfa->edges[0].first, RegularExpression::EPSILON);
    EXPECT_EQ(nfa->edges[1].first, RegularExpression::EPSILON);
    EXPECT_EQ(nfa->edges[1].second->type, "accept");

    auto loopStart = nfa->edges[0].second;
    ASSERT_EQ(loopStart->edges.size(), 1);
    EXPECT_EQ(loopStart->edges[0].first, "a");

    auto loopEnd = loopStart->edges[0].second;
    ASSERT_EQ(loopEnd->edges.size(), 2);
    EXPECT_EQ(loopEnd->edges[0].first, RegularExpression::EPSILON);
    EXPECT_EQ(loopEnd->edges[0].second, loopStart);
    EXPECT_EQ(loopEnd->edges[1].first, RegularExpression::EPSILON);
    EXPECT_EQ(loopEnd->edges[1].second->type, "accept");
}

TEST(TestRegexToNFA, Example373a) {
    const RegularExpression re("(a|b)*");
    const auto nfa = re.toNFA();
    ASSERT_NE(nfa, nullptr);
    EXPECT_EQ(nfa->type, "start");
    EXPECT_EQ(nfa->edges.size(), 2);
    EXPECT_EQ(nfa->edges[0].first, RegularExpression::EPSILON);
    EXPECT_EQ(nfa->edges[1].first, RegularExpression::EPSILON);
    EXPECT_EQ(nfa->edges[1].second->type, "accept");
}

TEST(TestRegexToNFA, Example373b) {
    const RegularExpression re("(a*|b*)*");
    const auto nfa = re.toNFA();
    ASSERT_NE(nfa, nullptr);
    EXPECT_EQ(nfa->type, "start");
}

TEST(TestRegexToNFA, Example373c) {
    const RegularExpression re("((ε|a)b*)*");
    const auto nfa = re.toNFA();
    ASSERT_NE(nfa, nullptr);
    EXPECT_EQ(nfa->type, "start");
}

TEST(TestRegexToNFA, Example373d) {
    const RegularExpression re("(a|b)*abb(a|b)*");
    const auto nfa = re.toNFA();
    ASSERT_NE(nfa, nullptr);
    EXPECT_EQ(nfa->type, "start");
}

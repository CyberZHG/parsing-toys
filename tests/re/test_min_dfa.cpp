#include "re.h"
#include <gtest/gtest.h>

using namespace std;

TEST(TestMinDFA, Epsilon) {
    const RegularExpression re("ε");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->key, "A");
    EXPECT_EQ(minDfa->edges.size(), 0);
}

TEST(TestMinDFA, StarAB) {
    const RegularExpression re("(a|b)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->key, "A,B,C");
    EXPECT_EQ(minDfa->edges.size(), 1);
    ASSERT_NE(minDfa->trans.find("a,b"), minDfa->trans.end());
    EXPECT_EQ(minDfa->trans.at("a,b")->key, "A,B,C");
}

TEST(TestMinDFA, PlusAB) {
    const RegularExpression re("(a|b)+");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->key, "A");
    EXPECT_EQ(minDfa->edges.size(), 1);
    ASSERT_NE(minDfa->trans.find("a,b"), minDfa->trans.end());
    const auto next = minDfa->trans.at("a,b");
    EXPECT_EQ(next->key, "B,C,D,E");
    EXPECT_EQ(next->edges.size(), 1);
    ASSERT_NE(next->trans.find("a,b"), next->trans.end());
    EXPECT_EQ(next->trans.at("a,b")->key, "B,C,D,E");
}

TEST(TestMinDFA, ABcStar) {
    const RegularExpression re("a(b|c)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->key, "A");
    EXPECT_EQ(minDfa->edges.size(), 1);
    ASSERT_NE(minDfa->trans.find("a"), minDfa->trans.end());
    const auto next = minDfa->trans.at("a");
    EXPECT_EQ(next->key, "B,C,D");
    EXPECT_EQ(next->edges.size(), 1);
    ASSERT_NE(next->trans.find("b,c"), next->trans.end());
    EXPECT_EQ(next->trans.at("b,c")->key, "B,C,D");
}

TEST(TestMinDFA, StarABStarCD) {
    const RegularExpression re("(a|b)*(c|d)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->key, "A,B,C");
    EXPECT_EQ(minDfa->edges.size(), 2);
    ASSERT_NE(minDfa->trans.find("a,b"), minDfa->trans.end());
    EXPECT_EQ(minDfa->trans.at("a,b")->key, "A,B,C");
    ASSERT_NE(minDfa->trans.find("c,d"), minDfa->trans.end());
    const auto cd = minDfa->trans.at("c,d");
    EXPECT_EQ(cd->key, "D,E");
    EXPECT_EQ(cd->edges.size(), 1);
    ASSERT_NE(cd->trans.find("c,d"), cd->trans.end());
    EXPECT_EQ(cd->trans.at("c,d")->key, "D,E");
}

TEST(TestMinDFA, ABOrB) {
    const RegularExpression re("ab|b");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->key, "A");
    EXPECT_EQ(minDfa->edges.size(), 2);
    ASSERT_NE(minDfa->trans.find("a"), minDfa->trans.end());
    EXPECT_EQ(minDfa->trans.at("a")->key, "B");
    ASSERT_NE(minDfa->trans.find("b"), minDfa->trans.end());
    EXPECT_EQ(minDfa->trans.at("b")->key, "C,D");

    const auto nodeA = minDfa->trans.at("a");
    EXPECT_EQ(nodeA->edges.size(), 1);
    ASSERT_NE(nodeA->trans.find("b"), nodeA->trans.end());
    EXPECT_EQ(nodeA->trans.at("b")->key, "C,D");
}

TEST(TestMinDFA, DFA) {
    const RegularExpression re("a");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    ASSERT_NE(dfa, nullptr);
    EXPECT_EQ(dfa->id, "A");
    EXPECT_EQ(dfa->type, "");
    EXPECT_EQ(dfa->symbols.size(), 1);
    EXPECT_EQ(dfa->symbols[0], "a");
    ASSERT_NE(dfa->trans.find("a"), dfa->trans.end());
    EXPECT_EQ(dfa->trans.at("a")->type, "accept");
}

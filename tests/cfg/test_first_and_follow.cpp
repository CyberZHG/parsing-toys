#include "cfg.h"
#include <gtest/gtest.h>
#include <ranges>
#include <algorithm>

using namespace std;

TEST(TestContextFreeGrammarFirstAndFollow, Empty) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(""));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(0, result.size());
}

TEST(TestContextFreeGrammarFirstAndFollow, SingleProductionSingleSymbol) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S->a"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(1, result.size());
    EXPECT_EQ("S", result.symbolAt(0));
    EXPECT_EQ(false, result.getNullable("S"));
    EXPECT_EQ(vector<string>({"a"}), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SingleProductionEmptySymbol) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S->ε"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(1, result.size());
    EXPECT_EQ(true, result.getNullable("S"));
    EXPECT_EQ(vector<string>({"ε"}), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SingleProductionSingleSymbolOrEmpty) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S->a|ε"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(1, result.size());
    EXPECT_EQ(true, result.getNullable("S"));
    EXPECT_EQ(vector<string>({"a", "ε"}), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SelfLoop) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S->S"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(1, result.size());
    EXPECT_EQ(false, result.getNullable("S"));
    EXPECT_EQ(vector<string>(), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
}

TEST(TestContextFreeGrammarFirstAndFollow, Loop) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S->A A->B B->A"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(3, result.size());
    EXPECT_EQ(false, result.getNullable("S"));
    EXPECT_EQ(vector<string>(), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>(), result.getFirstSet("A"));
    EXPECT_EQ(vector<string>(), result.getFirstSet("B"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("A"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("B"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SpecialCase1) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S->A A->a S b"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(2, result.size());
    EXPECT_EQ(vector<string>({"a"}), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({"b", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
    EXPECT_EQ(vector<string>({"a"}), result.getFirstSet("A"));
    EXPECT_EQ(vector<string>({"b", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("A"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SpecialCase2) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(S -> A B C D
                                      A -> b | ε
                                      B -> c
                                      C -> d
                                      D -> e)"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(5, result.size());
    EXPECT_EQ(vector<string>({"b", "c"}), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({"b", "ε"}), result.getFirstSet("A"));
    EXPECT_EQ(vector<string>({"c"}), result.getFirstSet("B"));
    EXPECT_EQ(vector<string>({"d"}), result.getFirstSet("C"));
    EXPECT_EQ(vector<string>({"e"}), result.getFirstSet("D"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
    EXPECT_EQ(vector<string>({"c"}), result.getFollowSet("A"));
    EXPECT_EQ(vector<string>({"d"}), result.getFollowSet("B"));
    EXPECT_EQ(vector<string>({"e"}), result.getFollowSet("C"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("D"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SpecialCase3) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(E -> T E'
                                      T -> F T'
                                      E' -> + T E' | ε
                                      T' -> * F T' | ε
                                      F -> ( E ) | id)"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(5, result.size());
    EXPECT_EQ(vector<string>({"(", "id"}), result.getFirstSet("E"));
    EXPECT_EQ(vector<string>({"(", "id"}), result.getFirstSet("T"));
    EXPECT_EQ(vector<string>({"(", "id"}), result.getFirstSet("F"));
    EXPECT_EQ(vector<string>({"+", "ε"}), result.getFirstSet("E'"));
    EXPECT_EQ(vector<string>({"*", "ε"}), result.getFirstSet("T'"));
    EXPECT_EQ(vector<string>({")", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("E"));
    EXPECT_EQ(vector<string>({")", "+", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("T"));
    EXPECT_EQ(vector<string>({")", "*", "+", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("F"));
    EXPECT_EQ(vector<string>({")", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("E'"));
    EXPECT_EQ(vector<string>({")", "+", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("T'"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SpecialCase4) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(E -> E + T | T
                                      T -> a)"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(2, result.size());
    EXPECT_EQ(vector<string>({"a"}), result.getFirstSet("E"));
    EXPECT_EQ(vector<string>({"a"}), result.getFirstSet("T"));
    EXPECT_EQ(vector<string>({"+", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("E"));
    EXPECT_EQ(vector<string>({"+", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("T"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SpecialCase5) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(S -> A B
                                      A -> a
                                      B -> ε)"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(3, result.size());
    EXPECT_EQ(vector<string>({"a"}), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({"a"}), result.getFirstSet("A"));
    EXPECT_EQ(vector<string>({"ε"}), result.getFirstSet("B"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("A"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("B"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SpecialCase6) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(A -> A a | ε)"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(1, result.size());
    EXPECT_EQ(vector<string>({"a", "ε"}), result.getFirstSet("A"));
    EXPECT_EQ(vector<string>({"a", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("A"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SpecialCase7) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(S -> A B C
                                      A -> a | ε
                                      B -> b | ε
                                      C -> c)"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(4, result.size());
    EXPECT_EQ(vector<string>({"a", "b", "c"}), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({"a", "ε"}), result.getFirstSet("A"));
    EXPECT_EQ(vector<string>({"b", "ε"}), result.getFirstSet("B"));
    EXPECT_EQ(vector<string>({"c"}), result.getFirstSet("C"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
    EXPECT_EQ(vector<string>({"b", "c"}), result.getFollowSet("A"));
    EXPECT_EQ(vector<string>({"c"}), result.getFollowSet("B"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("C"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SpecialCase8) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(S -> A B
                                      A -> a
                                      B -> ε)"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(3, result.size());
    EXPECT_EQ(vector<string>({"a"}), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({"a"}), result.getFirstSet("A"));
    EXPECT_EQ(vector<string>({"ε"}), result.getFirstSet("B"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("A"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("B"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SpecialCase9) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(S -> A b
                                      A -> ε | a)"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(2, result.size());
    EXPECT_EQ(vector<string>({"a", "b"}), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({"a", "ε"}), result.getFirstSet("A"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
    EXPECT_EQ(vector<string>({"b"}), result.getFollowSet("A"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SpecialCase10) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(S -> A A
                                      A -> a | ε)"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(2, result.size());
    EXPECT_EQ(vector<string>({"a", "ε"}), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({"a", "ε"}), result.getFirstSet("A"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
    EXPECT_EQ(vector<string>({"a", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("A"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SpecialCase11) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(S -> A B C
                                      A -> a | ε
                                      B -> b | ε
                                      C -> c | ε)"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(4, result.size());
    EXPECT_EQ(vector<string>({"a", "b", "c", "ε"}), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({"a", "ε"}), result.getFirstSet("A"));
    EXPECT_EQ(vector<string>({"b", "ε"}), result.getFirstSet("B"));
    EXPECT_EQ(vector<string>({"c", "ε"}), result.getFirstSet("C"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
    EXPECT_EQ(vector<string>({"b", "c", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("A"));
    EXPECT_EQ(vector<string>({"c", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("B"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("C"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SpecialCase12) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(S -> A d | e
                                      A -> B C
                                      B -> b | ε
                                      C -> A | c)"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(4, result.size());
    EXPECT_EQ(vector<string>({"b", "c", "e"}), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({"b", "c"}), result.getFirstSet("A"));
    EXPECT_EQ(vector<string>({"b", "ε"}), result.getFirstSet("B"));
    EXPECT_EQ(vector<string>({"b", "c"}), result.getFirstSet("C"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
    EXPECT_EQ(vector<string>({"d"}), result.getFollowSet("A"));
    EXPECT_EQ(vector<string>({"b", "c"}), result.getFollowSet("B"));
    EXPECT_EQ(vector<string>({"d"}), result.getFollowSet("C"));
}

TEST(TestContextFreeGrammarFirstAndFollow, SpecialCase13) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(S -> A B A
                                      A -> a | ε
                                      B -> b | ε)"));
    const auto result = grammar.computeFirstAndFollowSet();
    EXPECT_EQ(3, result.size());
    EXPECT_EQ(vector<string>({"a", "b", "ε"}), result.getFirstSet("S"));
    EXPECT_EQ(vector<string>({"a", "ε"}), result.getFirstSet("A"));
    EXPECT_EQ(vector<string>({"b", "ε"}), result.getFirstSet("B"));
    EXPECT_EQ(vector<string>({ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("S"));
    EXPECT_EQ(vector<string>({"a", "b", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("A"));
    EXPECT_EQ(vector<string>({"a", ContextFreeGrammar::EOF_SYMBOL}), result.getFollowSet("B"));
}

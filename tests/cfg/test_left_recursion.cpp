#include "cfg.h"
#include <gtest/gtest.h>
#include <ranges>
#include <algorithm>

TEST(TestContextFreeGrammarLeftRecursionElimination, Empty) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(""));
    EXPECT_TRUE(grammar.leftRecursionElimination());
    EXPECT_EQ("", grammar.toString());
}

TEST(TestContextFreeGrammarLeftRecursionElimination, SingleProductionSingleSymbol) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S->a"));
    EXPECT_TRUE(grammar.leftRecursionElimination());
    EXPECT_EQ("S -> a\n", grammar.toString());
}

TEST(TestContextFreeGrammarLeftRecursionElimination, SpecialCase1) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A a | b  A -> A c | S d | ϵ"));
    EXPECT_TRUE(grammar.leftRecursionElimination());
    const auto expected = R"( S -> A a
    | b
 A -> b d A'
    | A'
A' -> c A'
    | a d A'
    | ε
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftRecursionElimination, SpecialCase2) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> S S + | S S * | a"));
    EXPECT_TRUE(grammar.leftRecursionElimination());
    const auto expected = R"( S -> a S'
S' -> S + S'
    | S * S'
    | ε
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftRecursionElimination, SpecialCase3) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> 0 S 1 | 0 1"));
    EXPECT_TRUE(grammar.leftRecursionElimination());
    const auto expected = R"(S -> 0 S 1
   | 0 1
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftRecursionElimination, SpecialCase4) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> + S S | * S S | a"));
    EXPECT_TRUE(grammar.leftRecursionElimination());
    const auto expected = R"(S -> + S S
   | * S S
   | a
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftRecursionElimination, SpecialCase5) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> S ( S ) S | ε"));
    EXPECT_TRUE(grammar.leftRecursionElimination());
    const auto expected = R"( S -> S'
S' -> ( S ) S S'
    | ε
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftRecursionElimination, SpecialCase6) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> S + S | S S | ( S ) | S * | a"));
    EXPECT_TRUE(grammar.leftRecursionElimination());
    const auto expected = R"( S -> ( S ) S'
    | a S'
S' -> + S S'
    | S S'
    | * S'
    | ε
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftRecursionElimination, SpecialCase7) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> ( L ) | a L -> L , S | S"));
    EXPECT_TRUE(grammar.leftRecursionElimination());
    const auto expected = R"( S -> ( L )
    | a
 L -> ( L ) L'
    | a L'
L' -> , S L'
    | ε
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftRecursionElimination, SpecialCase8) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a S b S | b S a S | ε"));
    EXPECT_TRUE(grammar.leftRecursionElimination());
    const auto expected = R"(S -> a S b S
   | b S a S
   | ε
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftRecursionElimination, SpecialCase9) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(bexpr -> bexpr or bterm | bterm
bterm -> bterm and bfactor | bfactor
bfactor -> not bfactor | ( bexpr ) | true | false)"));
    EXPECT_TRUE(grammar.leftRecursionElimination());
    const auto expected = R"(  bexpr -> bterm bexpr'
 bexpr' -> or bterm bexpr'
         | ε
  bterm -> bfactor bterm'
 bterm' -> and bfactor bterm'
         | ε
bfactor -> not bfactor
         | ( bexpr )
         | true
         | false
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftRecursionElimination, SpecialCase10) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("A -> A a | A b"));
    EXPECT_FALSE(grammar.leftRecursionElimination());
}

TEST(TestContextFreeGrammarLeftRecursionElimination, SpecialCase11) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("A -> B B -> A"));
    EXPECT_FALSE(grammar.leftRecursionElimination());
}

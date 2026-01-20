#include "cfg.h"
#include <gtest/gtest.h>
#include <ranges>
#include <algorithm>

using namespace std;

TEST(TestContextFreeGrammarLeftFactoring, Empty) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(""));
    grammar.leftFactoring(true);
    EXPECT_EQ("", grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SingleProductionSingleSymbol) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S->a"));
    grammar.leftFactoring(true);
    EXPECT_EQ("S -> a\n", grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase1) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> i E t S | i E t S e S | a  E -> b"));
    grammar.leftFactoring(true);
    const auto expected = R"( S -> a
    | i E t S S'
S' -> e S
    | ε
 E -> b
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase2) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> S S + | S S * | a"));
    grammar.leftFactoring(true);
    const auto expected = R"( S -> a
    | S S S'
S' -> *
    | +
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase3) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> 0 S 1 | 0 1"));
    grammar.leftFactoring(true);
    const auto expected = R"( S -> 0 S'
S' -> 1
    | S 1
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase4) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> + S S | * S S | a"));
    grammar.leftFactoring(true);
    const auto expected = R"(S -> + S S
   | * S S
   | a
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase5) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> S ( S ) S | ϵ"));
    grammar.leftFactoring(true);
    const auto expected = R"(S -> S ( S ) S
   | ε
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase6) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> S + S | S S | ( S ) | S * | a"));
    grammar.leftFactoring(true);
    const auto expected = R"( S -> ( S )
    | a
    | S S'
S' -> *
    | + S
    | S
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase7) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> ( L ) | a L -> L , S | S"));
    grammar.leftFactoring(true);
    const auto expected = R"(S -> ( L )
   | a
L -> L , S
   | S
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase8) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a S b S | b S a S | ϵ"));
    grammar.leftFactoring(true);
    const auto expected = R"(S -> a S b S
   | b S a S
   | ε
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase9) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(bexpr -> bexpr or bterm | bterm
bterm -> bterm and bfactor | bfactor
bfactor -> not bfactor | ( bexpr ) | true | false)"));
    grammar.leftFactoring(true);
    const auto expected = R"(  bexpr -> bexpr or bterm
         | bterm
  bterm -> bterm and bfactor
         | bfactor
bfactor -> not bfactor
         | ( bexpr )
         | true
         | false
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase10) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("A -> id | B | a B -> id b"));
    grammar.leftFactoring(true);
    const auto expected = R"( A -> a
    | id A'
A' -> b
    | ε
 B -> id b
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase11) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a b | A A -> a b"));
    grammar.leftFactoring(true);
    const auto expected = R"(S -> a b
A -> a b
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase12) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a b c | A c A -> a b"));
    grammar.leftFactoring(true);
    const auto expected = R"(S -> a b c
A -> a b
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase13) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("A->id|B|a B->C C->D D->id b"));
    grammar.leftFactoring(true);
    const auto expected = R"( A -> a
    | id A'
A' -> b
    | ε
 B -> C
 C -> D
 D -> id b
)";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarLeftFactoring, SpecialCase14) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(S -> A c | A d
A -> a b)"));
    grammar.leftFactoring(true);
    const auto expected = R"( S -> A S'
S' -> c
    | d
 A -> a b
)";
    EXPECT_EQ(expected, grammar.toString());
}

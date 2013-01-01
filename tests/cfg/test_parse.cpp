#include "cfg.h"
#include <gtest/gtest.h>

TEST(TestContextFreeGrammarParse, Empty) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(""));
    EXPECT_EQ("", grammar.toString());
}

TEST(TestContextFreeGrammarParse, SingleProductionSingleSymbol) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S->a"));
    EXPECT_EQ("S -> a\n", grammar.toString());
}

TEST(TestContextFreeGrammarParse, SingleProductionTwoSymbols) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S->a b"));
    EXPECT_EQ("S -> a b\n", grammar.toString());
}

TEST(TestContextFreeGrammarParse, SingleHeadTwoProductions) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S->a|b"));
    EXPECT_EQ("S -> a\n   | b\n", grammar.toString());
}

TEST(TestContextFreeGrammarParse, NoHead1) {
    ContextFreeGrammar grammar;
    EXPECT_FALSE(grammar.parse("->"));
    EXPECT_EQ("Line 1 Column 1: Can not find the head of the production.", grammar.errorMessage());
}

TEST(TestContextFreeGrammarParse, NoHead2) {
    ContextFreeGrammar grammar;
    EXPECT_FALSE(grammar.parse("S->->a"));
    EXPECT_EQ("Line 1 Column 4: Can not find the head of the production.", grammar.errorMessage());
}

TEST(TestContextFreeGrammarParse, NoHead3) {
    ContextFreeGrammar grammar;
    EXPECT_FALSE(grammar.parse("S->a|->b"));
    EXPECT_EQ("Line 1 Column 6: Can not find the head of the production.", grammar.errorMessage());
}

TEST(TestContextFreeGrammarParse, NoHead4) {
    ContextFreeGrammar grammar;
    EXPECT_FALSE(grammar.parse("S"));
    EXPECT_EQ("Line 1 Column 1: Can not find the head of the production.", grammar.errorMessage());
}

TEST(TestContextFreeGrammarParse, NoHead5) {
    ContextFreeGrammar grammar;
    EXPECT_FALSE(grammar.parse("|"));
    EXPECT_EQ("Line 1 Column 1: Can not find the head of the production.", grammar.errorMessage());
}

TEST(TestContextFreeGrammarParse, EmptyProduction1) {
    ContextFreeGrammar grammar;
    EXPECT_FALSE(grammar.parse("S->"));
    EXPECT_EQ("Line 1 Column 4: Found empty production for 'S'.", grammar.errorMessage());
}

TEST(TestContextFreeGrammarParse, EmptyProduction2) {
    ContextFreeGrammar grammar;
    EXPECT_FALSE(grammar.parse("S->|b"));
    EXPECT_EQ("Line 1 Column 4: Found empty production for 'S'.", grammar.errorMessage());
}

TEST(TestContextFreeGrammarParse, EmptyProduction3) {
    ContextFreeGrammar grammar;
    EXPECT_FALSE(grammar.parse("S->a||b"));
    EXPECT_EQ("Line 1 Column 6: Found empty production for 'S'.", grammar.errorMessage());
}

TEST(TestContextFreeGrammarParse, EmptyProduction4) {
    ContextFreeGrammar grammar;
    EXPECT_FALSE(grammar.parse("S->a|B->c"));
    EXPECT_EQ("Line 1 Column 6: Found empty production for 'S'.", grammar.errorMessage());
}

TEST(TestContextFreeGrammarParse, SpecialCase1) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> S S + | S S * | a"));
    EXPECT_EQ("S -> S S +\n   | S S *\n   | a\n", grammar.toString());
}

TEST(TestContextFreeGrammarParse, SpecialCase2) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(R"(bexpr -> bexpr or bterm | bterm
bterm -> bterm and bfactor | bfactor
bfactor -> not bfactor | ( bexpr ) | true | false)"));
    EXPECT_EQ(R"(  bexpr -> bexpr or bterm
         | bterm
  bterm -> bterm and bfactor
         | bfactor
bfactor -> not bfactor
         | ( bexpr )
         | true
         | false
)", grammar.toString());
}

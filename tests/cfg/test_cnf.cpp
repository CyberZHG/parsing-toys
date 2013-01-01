#include "cfg.h"
#include <gtest/gtest.h>

using namespace std;

TEST(TestContextFreeGrammarCNF, EmptyGrammarIsCNF) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(""));
    EXPECT_TRUE(grammar.isChomskyNormalForm());
}

TEST(TestContextFreeGrammarCNF, SingleTerminalIsCNF) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a"));
}

TEST(TestContextFreeGrammarCNF, TwoNonTerminalsIsCNF) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A B\nA -> a\nB -> b"));
    EXPECT_TRUE(grammar.isChomskyNormalForm());
}

TEST(TestContextFreeGrammarCNF, StartEpsilonIsCNF) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> ε"));
    EXPECT_TRUE(grammar.isChomskyNormalForm());
}

TEST(TestContextFreeGrammarCNF, TerminalInLongProductionNotCNF) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a B\nB -> b"));
    EXPECT_FALSE(grammar.isChomskyNormalForm());
}

TEST(TestContextFreeGrammarCNF, ThreeSymbolsNotCNF) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A B C\nA -> a\nB -> b\nC -> c"));
    EXPECT_FALSE(grammar.isChomskyNormalForm());
}

TEST(TestContextFreeGrammarCNF, UnitProductionNotCNF) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A\nA -> a"));
    EXPECT_FALSE(grammar.isChomskyNormalForm());
}

TEST(TestContextFreeGrammarCNF, NonStartEpsilonNotCNF) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A B\nA -> a | ε\nB -> b"));
    EXPECT_FALSE(grammar.isChomskyNormalForm());
}

TEST(TestContextFreeGrammarCNF, ConvertEmpty) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(""));
    grammar.toChomskyNormalForm();
    EXPECT_EQ("", grammar.toString());
    EXPECT_TRUE(grammar.isChomskyNormalForm());
    const auto expected = R"()";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarCNF, ConvertSingleTerminal) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a"));
    grammar.toChomskyNormalForm();
    EXPECT_TRUE(grammar.isChomskyNormalForm());
    const auto expected = R"()";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarCNF, ConvertUnitProductions) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A\nA -> B\nB -> a"));
    grammar.toChomskyNormalForm();
    EXPECT_TRUE(grammar.isChomskyNormalForm());
    const auto expected = R"()";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarCNF, ConvertLongProduction) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A B C\nA -> a\nB -> b\nC -> c"));
    grammar.toChomskyNormalForm();
    EXPECT_TRUE(grammar.isChomskyNormalForm());
    const auto expected = R"()";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarCNF, ConvertTerminalInLongProduction) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a b"));
    grammar.toChomskyNormalForm();
    EXPECT_TRUE(grammar.isChomskyNormalForm());
    const auto expected = R"()";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarCNF, ConvertEpsilonProduction) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A B\nA -> a | ε\nB -> b | ε"));
    grammar.toChomskyNormalForm();
    EXPECT_TRUE(grammar.isChomskyNormalForm());
    const auto expected = R"()";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarCNF, ConvertStartOnRHS) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a S b | a b"));
    grammar.toChomskyNormalForm();
    EXPECT_TRUE(grammar.isChomskyNormalForm());
    const auto expected = R"()";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarCNF, ConvertArithmeticExpression) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("E -> E + T | T\nT -> T * F | F\nF -> ( E ) | id"));
    grammar.toChomskyNormalForm();
    EXPECT_TRUE(grammar.isChomskyNormalForm());
    const auto expected = R"()";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarCNF, ConvertStartCanDeriveEpsilon) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A B | ε\nA -> a\nB -> b"));
    grammar.toChomskyNormalForm();
    EXPECT_TRUE(grammar.isChomskyNormalForm());
    const auto expected = R"()";
    EXPECT_EQ(expected, grammar.toString());
}

TEST(TestContextFreeGrammarCNF, ConvertComplexGrammar) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a S b S | b S a S | ε"));
    grammar.toChomskyNormalForm();
    EXPECT_TRUE(grammar.isChomskyNormalForm());
    const auto expected = R"()";
    EXPECT_EQ(expected, grammar.toString());
}

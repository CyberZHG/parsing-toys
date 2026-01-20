#include "cfg.h"
#include <gtest/gtest.h>

using namespace std;

TEST(TestCYK, EmptyString) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> ε"));
    const auto result = grammar.cykParse("");
    EXPECT_TRUE(result.accepted);
    EXPECT_EQ(0, result.size());
}

TEST(TestCYK, EmptyStringNotAccepted) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a"));
    const auto result = grammar.cykParse("");
    EXPECT_FALSE(result.accepted);
}

TEST(TestCYK, SingleTerminal) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a"));
    const auto result = grammar.cykParse("a");
    EXPECT_TRUE(result.accepted);
    EXPECT_EQ(1, result.size());
}

TEST(TestCYK, SingleTerminalRejected) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a"));
    const auto result = grammar.cykParse("b");
    EXPECT_FALSE(result.accepted);
}

TEST(TestCYK, TwoNonTerminals) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A B\nA -> a\nB -> b"));
    const auto result = grammar.cykParse("a b");
    EXPECT_TRUE(result.accepted);
    EXPECT_EQ(2, result.size());
}

TEST(TestCYK, CNFGrammarPalindrome) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a | b | A S A | B S B\nA -> a\nB -> b"));
    grammar.toChomskyNormalForm();
    const auto result = grammar.cykParse("a b a");
    EXPECT_TRUE(result.accepted);
}

TEST(TestCYK, CNFGrammarPalindromeRejected) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a | b | A S A | B S B\nA -> a\nB -> b"));
    grammar.toChomskyNormalForm();
    const auto result = grammar.cykParse("a b b");
    EXPECT_FALSE(result.accepted);
}

TEST(TestCYK, ArithmeticExpression) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("E -> E + T | T\nT -> T * F | F\nF -> ( E ) | id"));
    grammar.toChomskyNormalForm();
    const auto result = grammar.cykParse("id + id * id");
    EXPECT_TRUE(result.accepted);
}

TEST(TestCYK, ArithmeticExpressionRejected) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("E -> E + T | T\nT -> T * F | F\nF -> ( E ) | id"));
    grammar.toChomskyNormalForm();
    const auto result = grammar.cykParse("id + + id");
    EXPECT_FALSE(result.accepted);
}

TEST(TestCYK, EpsilonProductions) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A B\nA -> a | ε\nB -> b | ε"));
    grammar.toChomskyNormalForm();
    const auto resultAB = grammar.cykParse("a b");
    EXPECT_TRUE(resultAB.accepted);
    const auto resultA = grammar.cykParse("a");
    EXPECT_TRUE(resultA.accepted);
    const auto resultB = grammar.cykParse("b");
    EXPECT_TRUE(resultB.accepted);
    const auto resultEmpty = grammar.cykParse("");
    EXPECT_TRUE(resultEmpty.accepted);
}

TEST(TestCYK, MatchedParentheses) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> ( S ) | S S | ε"));
    grammar.toChomskyNormalForm();
    const auto result1 = grammar.cykParse("( )");
    EXPECT_TRUE(result1.accepted);
    const auto result2 = grammar.cykParse("( ( ) )");
    EXPECT_TRUE(result2.accepted);
    const auto result3 = grammar.cykParse("( ) ( )");
    EXPECT_TRUE(result3.accepted);
    const auto result4 = grammar.cykParse("( ( )");
    EXPECT_FALSE(result4.accepted);
}

TEST(TestCYK, ParseTreeSingleTerminal) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> a"));
    const auto result = grammar.cykParse("a");
    EXPECT_TRUE(result.accepted);
    EXPECT_NE(nullptr, result.parseTree);
    EXPECT_EQ("S", result.parseTree->label);
    EXPECT_EQ(1, result.parseTree->children.size());
    EXPECT_EQ("a", result.parseTree->children[0]->label);
}

TEST(TestCYK, ParseTreeTwoNonTerminals) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A B\nA -> a\nB -> b"));
    const auto result = grammar.cykParse("a b");
    EXPECT_TRUE(result.accepted);
    EXPECT_NE(nullptr, result.parseTree);
    EXPECT_EQ("S", result.parseTree->label);
    EXPECT_EQ(2, result.parseTree->children.size());
    EXPECT_EQ("A", result.parseTree->children[0]->label);
    EXPECT_EQ("B", result.parseTree->children[1]->label);
}

TEST(TestCYK, CellContents) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A B\nA -> a\nB -> b"));
    const auto result = grammar.cykParse("a b");
    EXPECT_EQ(2, result.size());
    const auto cell00 = result.getCell(0, 0);
    EXPECT_EQ(1, cell00.size());
    EXPECT_EQ("A", cell00[0]);
    const auto cell11 = result.getCell(1, 1);
    EXPECT_EQ(1, cell11.size());
    EXPECT_EQ("B", cell11[0]);
    const auto cell01 = result.getCell(0, 1);
    EXPECT_EQ(1, cell01.size());
    EXPECT_EQ("S", cell01[0]);
}

TEST(TestCYK, CellStringFormat) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A B | A C\nA -> a\nB -> b\nC -> b"));
    const auto result = grammar.cykParse("a b");
    EXPECT_EQ(2, result.size());
    const auto cellStr = result.getCellString(0, 1, ", ");
    EXPECT_TRUE(cellStr.find('S') != string::npos);
}

TEST(TestCYK, InvalidCellIndices) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse("S -> A B\nA -> a\nB -> b"));
    const auto result = grammar.cykParse("a b");
    EXPECT_EQ(2, result.size());

    auto cell1 = result.getCell(5, 0);
    EXPECT_TRUE(cell1.empty());
    auto cellStr1 = result.getCellString(5, 0, ", ");
    EXPECT_EQ("", cellStr1);

    auto cell2 = result.getCell(0, 5);
    EXPECT_TRUE(cell2.empty());
    auto cellStr2 = result.getCellString(0, 5, ", ");
    EXPECT_EQ("", cellStr2);

    auto cell3 = result.getCell(1, 0);
    EXPECT_TRUE(cell3.empty());
    auto cellStr3 = result.getCellString(1, 0, ", ");
    EXPECT_EQ("", cellStr3);
}

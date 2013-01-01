#include "cfg.h"
#include <gtest/gtest.h>

TEST(TestContextFreeGrammarTokenize, DifferentTokens) {
    EXPECT_NE(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::PRODUCE_SIGN, "->", 1, 1}),
              ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "->", 1, 1}));
    EXPECT_NE(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 1, 1}),
              ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "b", 1, 1}));
    EXPECT_NE(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 1, 1}),
              ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 2, 1}));
    EXPECT_NE(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 1, 1}),
              ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 1, 2}));
}

TEST(TestContextFreeGrammarTokenize, Empty) {
    const auto tokens = ContextFreeGrammar::tokenize("");
    EXPECT_EQ(0, tokens.size());
}

TEST(TestContextFreeGrammarTokenize, OnlyArrow) {
    const auto tokens = ContextFreeGrammar::tokenize("->");
    EXPECT_EQ(1, tokens.size());
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::PRODUCE_SIGN, "->", 1, 1}), tokens[0]);
}

TEST(TestContextFreeGrammarTokenize, OnlyOr) {
    const auto tokens = ContextFreeGrammar::tokenize("|");
    EXPECT_EQ(1, tokens.size());
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::OR_SIGN, "|", 1, 1}), tokens[0]);
}

TEST(TestContextFreeGrammarTokenize, OnlySymbol) {
    const auto tokens = ContextFreeGrammar::tokenize("SSS");
    EXPECT_EQ(1, tokens.size());
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "SSS", 1, 1}), tokens[0]);
}

TEST(TestContextFreeGrammarTokenize, PrintTo) {
    const auto tokens = ContextFreeGrammar::tokenize("S");
    EXPECT_EQ(1, tokens.size());
    stringstream ss;
    PrintTo(tokens[0], &ss);
    const auto output = ss.str();
    EXPECT_EQ("{0,S,1,1}", output);
}

TEST(TestContextFreeGrammarTokenize, OneHeadOneProduction) {
    const auto tokens = ContextFreeGrammar::tokenize("S -> a b");
    EXPECT_EQ(4, tokens.size());
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "S", 1, 1}), tokens[0]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::PRODUCE_SIGN, "->", 1, 3}), tokens[1]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 1, 6}), tokens[2]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "b", 1, 8}), tokens[3]);
}

TEST(TestContextFreeGrammarTokenize, OneHeadOneProductionCompact) {
    const auto tokens = ContextFreeGrammar::tokenize("S->a b");
    EXPECT_EQ(4, tokens.size());
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "S", 1, 1}), tokens[0]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::PRODUCE_SIGN, "->", 1, 2}), tokens[1]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 1, 4}), tokens[2]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "b", 1, 6}), tokens[3]);
}

TEST(TestContextFreeGrammarTokenize, OneHeadTwoProductions) {
    const auto tokens = ContextFreeGrammar::tokenize("S -> a b | a d");
    EXPECT_EQ(7, tokens.size());
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "S", 1, 1}), tokens[0]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::PRODUCE_SIGN, "->", 1, 3}), tokens[1]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 1, 6}), tokens[2]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "b", 1, 8}), tokens[3]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::OR_SIGN, "|", 1, 10}), tokens[4]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 1, 12}), tokens[5]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "d", 1, 14}), tokens[6]);
}

TEST(TestContextFreeGrammarTokenize, OneHeadTwoProductionsCompact) {
    const auto tokens = ContextFreeGrammar::tokenize("S->a b|a d");
    EXPECT_EQ(7, tokens.size());
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "S", 1, 1}), tokens[0]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::PRODUCE_SIGN, "->", 1, 2}), tokens[1]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 1, 4}), tokens[2]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "b", 1, 6}), tokens[3]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::OR_SIGN, "|", 1, 7}), tokens[4]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 1, 8}), tokens[5]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "d", 1, 10}), tokens[6]);
}

TEST(TestContextFreeGrammarTokenize, OneHeadTwoProductionsMultiline) {
    const auto tokens = ContextFreeGrammar::tokenize("S->a b\n  |a d");
    EXPECT_EQ(7, tokens.size());
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "S", 1, 1}), tokens[0]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::PRODUCE_SIGN, "->", 1, 2}), tokens[1]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 1, 4}), tokens[2]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "b", 1, 6}), tokens[3]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::OR_SIGN, "|", 2, 3}), tokens[4]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 2, 4}), tokens[5]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "d", 2, 6}), tokens[6]);
}

TEST(TestContextFreeGrammarTokenize, OneHeadTwoProductionsNewLines) {
    const auto tokens = ContextFreeGrammar::tokenize("S->a b\r\n\n\r  |a d");
    EXPECT_EQ(7, tokens.size());
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "S", 1, 1}), tokens[0]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::PRODUCE_SIGN, "->", 1, 2}), tokens[1]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 1, 4}), tokens[2]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "b", 1, 6}), tokens[3]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::OR_SIGN, "|", 4, 3}), tokens[4]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "a", 4, 4}), tokens[5]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "d", 4, 6}), tokens[6]);
}

TEST(TestContextFreeGrammarTokenize, MinusSign1) {
    const auto tokens = ContextFreeGrammar::tokenize("S -> + | -");
    EXPECT_EQ(5, tokens.size());
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "S", 1, 1}), tokens[0]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::PRODUCE_SIGN, "->", 1, 3}), tokens[1]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "+", 1, 6}), tokens[2]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::OR_SIGN, "|", 1, 8}), tokens[3]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "-", 1, 10}), tokens[4]);
}

TEST(TestContextFreeGrammarTokenize, MinusSign2) {
    const auto tokens = ContextFreeGrammar::tokenize("S -> - | +");
    EXPECT_EQ(5, tokens.size());
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "S", 1, 1}), tokens[0]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::PRODUCE_SIGN, "->", 1, 3}), tokens[1]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "-", 1, 6}), tokens[2]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::OR_SIGN, "|", 1, 8}), tokens[3]);
    EXPECT_EQ(ContextFreeGrammarToken({ContextFreeGrammarToken::Type::SYMBOL, "+", 1, 10}), tokens[4]);
}

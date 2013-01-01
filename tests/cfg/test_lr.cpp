#include "cfg.h"
#include "automaton.h"
#include <gtest/gtest.h>
#include <format>
#include <fstream>
using namespace std;

TEST(TestLRParsing, SimpleGrammar) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S → A A
A → a
)");
    const auto automaton = grammar.computeLR0Automaton();
    auto table = grammar.computeLR0ActionGotoTable(automaton);
    EXPECT_FALSE(table.hasConflict());

    const auto steps = table.parse("a a");
    const auto expected = R"(| Stack | Symbols | Inputs | Action |
|:-:|:-:|:-:|:-:|
| 0 | | a a ¥ | shift 3 |
| 0 3 | a | a ¥ | reduce A -> a |
| 0 2 | A | a ¥ | shift 3 |
| 0 2 3 | A a | ¥ | reduce A -> a |
| 0 2 4 | A A | ¥ | reduce S -> A A |
| 0 1 | S | ¥ | accept |
)";
    EXPECT_EQ(expected, steps.toString());
    const auto expectedParseTree = R"(S -> A A
  A -> a
    a
  A -> a
    a
)";
    EXPECT_EQ(expectedParseTree, table.parseTree->toString());

    const ::testing::TestInfo* info = ::testing::UnitTest::GetInstance()->current_test_info();
    auto svg = table.parseTree->toSVG();
    auto filePath = format("{}_{}.svg", info->test_suite_name(), info->name());
    ofstream file(filePath);
    file << svg;
    file.close();
}

TEST(TestLRParsing, ShiftReduceConflict) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S → a S
S → a
)");
    const auto automaton = grammar.computeLR0Automaton();
    auto table = grammar.computeLR0ActionGotoTable(automaton);
    EXPECT_TRUE(table.hasConflict());
    EXPECT_TRUE(table.hasConflict(2, "a"));

    const auto steps = table.parse("a a");
    const auto expected = R"(| Stack | Symbols | Inputs | Action |
|:-:|:-:|:-:|:-:|
| 0 | | a a ¥ | shift 2 |
| 0 2 | a | a ¥ | conflict: shift 2 / reduce S -> a |
)";
    EXPECT_EQ(expected, steps.toString());
}

TEST(TestLRParsing, ReduceReduceConflict) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S → A
S → B
A → a
B → a
)");
    const auto automaton = grammar.computeLR0Automaton();
    auto table = grammar.computeLR0ActionGotoTable(automaton);
    EXPECT_TRUE(table.hasConflict());

    const auto steps = table.parse("a");
    const auto expected = R"(| Stack | Symbols | Inputs | Action |
|:-:|:-:|:-:|:-:|
| 0 | | a ¥ | shift 4 |
| 0 4 | a | ¥ | conflict: reduce A -> a / reduce B -> a |
)";
    EXPECT_EQ(expected, steps.toString());
}

TEST(TestLRParsing, InvalidSymbol) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S → a
)");
    const auto automaton = grammar.computeLR0Automaton();
    auto table = grammar.computeLR0ActionGotoTable(automaton);

    const auto steps = table.parse("b");
    const auto expected = R"(| Stack | Symbols | Inputs | Action |
|:-:|:-:|:-:|:-:|
| 0 | | b ¥ | invalid symbol |
)";
    EXPECT_EQ(expected, steps.toString());
}

TEST(TestLRParsing, EpsilonProduction) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S → A
A → ε
)");
    const auto automaton = grammar.computeLR0Automaton();
    auto table = grammar.computeLR0ActionGotoTable(automaton);

    const auto steps = table.parse("");
    const auto expected = R"(| Stack | Symbols | Inputs | Action |
|:-:|:-:|:-:|:-:|
| 0 | | ¥ | reduce A -> ε |
| 0 2 | A | ¥ | reduce S -> A |
| 0 1 | S | ¥ | accept |
)";
    EXPECT_EQ(expected, steps.toString());
    const auto expectedParseTree = R"(S -> A
  A -> ε
)";
    EXPECT_EQ(expectedParseTree, table.parseTree->toString());

    const ::testing::TestInfo* info = ::testing::UnitTest::GetInstance()->current_test_info();
    auto svg = table.parseTree->toSVG();
    auto filePath = format("{}_{}.svg", info->test_suite_name(), info->name());
    ofstream file(filePath);
    file << svg;
    file.close();
}

TEST(TestLRParsing, ChainReduction) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S → A
A → B
B → c
)");
    const auto automaton = grammar.computeLR0Automaton();
    auto table = grammar.computeLR0ActionGotoTable(automaton);
    EXPECT_FALSE(table.hasConflict());

    const auto steps = table.parse("c");
    const auto expected = R"(| Stack | Symbols | Inputs | Action |
|:-:|:-:|:-:|:-:|
| 0 | | c ¥ | shift 4 |
| 0 4 | c | ¥ | reduce B -> c |
| 0 3 | B | ¥ | reduce A -> B |
| 0 2 | A | ¥ | reduce S -> A |
| 0 1 | S | ¥ | accept |
)";
    EXPECT_EQ(expected, steps.toString());
    const auto expectedParseTree = R"(S -> A
  A -> B
    B -> c
      c
)";
    EXPECT_EQ(expectedParseTree, table.parseTree->toString());

    const ::testing::TestInfo* info = ::testing::UnitTest::GetInstance()->current_test_info();
    auto svg = table.parseTree->toSVG();
    auto filePath = format("{}_{}.svg", info->test_suite_name(), info->name());
    ofstream file(filePath);
    file << svg;
    file.close();
}

TEST(TestLRParsing, SingleTerminal) {
    ContextFreeGrammar grammar;
    grammar.parse("S → a");
    const auto automaton = grammar.computeLR0Automaton();
    auto table = grammar.computeLR0ActionGotoTable(automaton);
    EXPECT_FALSE(table.hasConflict());

    const auto steps = table.parse("a");
    const auto expected = R"(| Stack | Symbols | Inputs | Action |
|:-:|:-:|:-:|:-:|
| 0 | | a ¥ | shift 2 |
| 0 2 | a | ¥ | reduce S -> a |
| 0 1 | S | ¥ | accept |
)";
    EXPECT_EQ(expected, steps.toString());
    const auto expectedParseTree = R"(S -> a
  a
)";
    EXPECT_EQ(expectedParseTree, table.parseTree->toString());

    const ::testing::TestInfo* info = ::testing::UnitTest::GetInstance()->current_test_info();
    auto svg = table.parseTree->toSVG();
    auto filePath = format("{}_{}.svg", info->test_suite_name(), info->name());
    ofstream file(filePath);
    file << svg;
    file.close();
}

TEST(TestLRParsing, MultipleTerminals) {
    ContextFreeGrammar grammar;
    grammar.parse("S → a b c");
    const auto automaton = grammar.computeLR0Automaton();
    auto table = grammar.computeLR0ActionGotoTable(automaton);
    EXPECT_FALSE(table.hasConflict());

    const auto steps = table.parse("a b c");
    const auto expected = R"(| Stack | Symbols | Inputs | Action |
|:-:|:-:|:-:|:-:|
| 0 | | a b c ¥ | shift 2 |
| 0 2 | a | b c ¥ | shift 3 |
| 0 2 3 | a b | c ¥ | shift 4 |
| 0 2 3 4 | a b c | ¥ | reduce S -> a b c |
| 0 1 | S | ¥ | accept |
)";
    EXPECT_EQ(expected, steps.toString());
    const auto expectedParseTree = R"(S -> a b c
  a
  b
  c
)";
    EXPECT_EQ(expectedParseTree, table.parseTree->toString());

    const ::testing::TestInfo* info = ::testing::UnitTest::GetInstance()->current_test_info();
    auto svg = table.parseTree->toSVG();
    auto filePath = format("{}_{}.svg", info->test_suite_name(), info->name());
    ofstream file(filePath);
    file << svg;
    file.close();
}

TEST(TestLRParsing, MissingNumPopSymbols) {
    ActionGotoTable table(2);
    table.actions[0]["a"].emplace_back("reduce S -> a");

    const auto steps = table.parse("a");
    const auto expected = R"(| Stack | Symbols | Inputs | Action |
|:-:|:-:|:-:|:-:|
| 0 | | a ¥ | reduce S -> a |
| 0 | | a ¥ | invalid action/goto table |
)";
    EXPECT_EQ(expected, steps.toString());
}

TEST(TestLRParsing, MissingReducedSymbols) {
    ActionGotoTable table(2);
    table.actions[0]["a"].emplace_back("shift 1");
    table.nextStates[0]["a"] = 1;

    table.actions[1]["¥"].emplace_back("reduce S -> a");
    table.reduceProductions[1]["¥"] = {"a"};

    const auto steps = table.parse("a");
    const auto expected = R"(| Stack | Symbols | Inputs | Action |
|:-:|:-:|:-:|:-:|
| 0 | | a ¥ | shift 1 |
| 0 1 | a | ¥ | reduce S -> a |
| 0 | | ¥ | invalid action/goto table |
)";
    EXPECT_EQ(expected, steps.toString());
}

TEST(TestLRParsing, MissingGotoEntry) {
    ActionGotoTable table(2);
    table.actions[0]["a"].emplace_back("shift 1");
    table.nextStates[0]["a"] = 1;

    table.actions[1]["¥"].emplace_back("reduce S -> a");
    table.reduceProductions[1]["¥"] = {"a"};
    table.reduceHeads[1]["¥"] = "S";

    const auto steps = table.parse("a");
    const auto expected = R"(| Stack | Symbols | Inputs | Action |
|:-:|:-:|:-:|:-:|
| 0 | | a ¥ | shift 1 |
| 0 1 | a | ¥ | reduce S -> a |
| 0 | | ¥ | invalid action/goto table |
)";
    EXPECT_EQ(expected, steps.toString());
}

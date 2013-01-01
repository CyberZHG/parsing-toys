#include "cfg.h"
#include "automaton.h"
#include <gtest/gtest.h>
#include <ranges>
#include <algorithm>
#include <fstream>
#include <format>

using namespace std;

TEST(TestContextFreeGrammarLL1Automaton, Empty) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(""));
    const auto table = grammar.computeLL1Table();
    const auto expected = R"(| | ¥ |
|:-:|:-:|
)";
    EXPECT_EQ(expected, table.toString());
}

TEST(TestContextFreeGrammarLL1Automaton, SpecialCase1) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
E -> E + T | T
T -> T * F | F
F -> ( E ) | id
)");
    grammar.leftRecursionElimination();
    grammar.leftFactoring();
    const auto table = grammar.computeLL1Table();
    const auto expectedTable = R"(| | ( | ) | * | + | id | ε | ¥ |
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| E | E -> T E' |  |  |  | E -> T E' |  |  |
| E' |  | E' -> ε |  | E' -> + T E' |  |  | E' -> ε |
| T | T -> F T' |  |  |  | T -> F T' |  |  |
| T' |  | T' -> ε | T' -> * F T' | T' -> ε |  |  | T' -> ε |
| F | F -> ( E ) |  |  |  | F -> id |  |  |
)";
    EXPECT_EQ(expectedTable, table.toString());

    const auto steps = table.parse("id + id * id");
    const auto expectedSteps = R"(| Stack | Input | Action |
|:-:|:-:|:-:|
| ¥ E | id + id * id ¥ | E -> T E' |
| ¥ E' T | id + id * id ¥ | T -> F T' |
| ¥ E' T' F | id + id * id ¥ | F -> id |
| ¥ E' T' id | id + id * id ¥ | match id |
| ¥ E' T' | + id * id ¥ | T' -> ε |
| ¥ E' | + id * id ¥ | E' -> + T E' |
| ¥ E' T + | + id * id ¥ | match + |
| ¥ E' T | id * id ¥ | T -> F T' |
| ¥ E' T' F | id * id ¥ | F -> id |
| ¥ E' T' id | id * id ¥ | match id |
| ¥ E' T' | * id ¥ | T' -> * F T' |
| ¥ E' T' F * | * id ¥ | match * |
| ¥ E' T' F | id ¥ | F -> id |
| ¥ E' T' id | id ¥ | match id |
| ¥ E' T' | ¥ | T' -> ε |
| ¥ E' | ¥ | E' -> ε |
| ¥ | ¥ | accept |
)";
    EXPECT_EQ(expectedSteps, steps.toString());

    const ::testing::TestInfo* info = ::testing::UnitTest::GetInstance()->current_test_info();
    auto svg = steps.parseTree->toSVG();
    auto filePath = format("{}_{}.svg", info->test_suite_name(), info->name());
    ofstream file(filePath);
    file << svg;
    file.close();
}

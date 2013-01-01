#include "cfg.h"
#include "automaton.h"
#include <gtest/gtest.h>
#include <ranges>
#include <fstream>

using namespace std;

TEST(TestContextFreeGrammarSLR1Automaton, Empty) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(""));
    EXPECT_THROW(grammar.computeSLR1Automaton(), runtime_error);
}

TEST(TestContextFreeGrammarSLR1Automaton, SpecialCase1) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
E -> E + T | T
T -> T * F | F
F -> ( E ) | id
)");
    const auto automaton = grammar.computeSLR1Automaton();

    const auto table = grammar.computeSLR1ActionGotoTable(automaton);
    const auto expectedTable = R"(| State | ( | ) | * | + | id | ¥ | E | T | F |
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| 0 | shift 4 |  |  |  | shift 5 |  | 1 | 2 | 3 |
| 1 |  |  |  | shift 6 |  | accept |  |  |  |
| 2 |  | reduce E -> T | shift 7 | reduce E -> T |  | reduce E -> T |  |  |  |
| 3 |  | reduce T -> F | reduce T -> F | reduce T -> F |  | reduce T -> F |  |  |  |
| 4 | shift 4 |  |  |  | shift 5 |  | 8 | 2 | 3 |
| 5 |  | reduce F -> id | reduce F -> id | reduce F -> id |  | reduce F -> id |  |  |  |
| 6 | shift 4 |  |  |  | shift 5 |  |  | 9 | 3 |
| 7 | shift 4 |  |  |  | shift 5 |  |  |  | 10 |
| 8 |  | shift 11 |  | shift 6 |  |  |  |  |  |
| 9 |  | reduce E -> E + T | shift 7 | reduce E -> E + T |  | reduce E -> E + T |  |  |  |
| 10 |  | reduce T -> T * F | reduce T -> T * F | reduce T -> T * F |  | reduce T -> T * F |  |  |  |
| 11 |  | reduce F -> ( E ) | reduce F -> ( E ) | reduce F -> ( E ) |  | reduce F -> ( E ) |  |  |  |
)";
    EXPECT_EQ(expectedTable, table.toString(grammar));
}
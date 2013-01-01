#include "cfg.h"
#include "automaton.h"
#include <gtest/gtest.h>
#include <ranges>
#include <algorithm>
#include <fstream>
#include <format>

using namespace std;

TEST(TestContextFreeGrammarLALR1Automaton, Empty) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(""));
    EXPECT_THROW(grammar.computeLALR1Automaton(), runtime_error);
}

TEST(TestContextFreeGrammarLALR1Automaton, SpecialCase1) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S -> C C
C -> c C | d
)");
    const auto automaton = grammar.computeLALR1Automaton();
    EXPECT_EQ(7, automaton->size());

    EXPECT_EQ(R"(I₀
===
S' -> · S ﹐ ¥
---
S -> · C C ﹐ ¥
C -> · c C ﹐ c
   | · c C ﹐ d
   | · d ﹐ c
   | · d ﹐ d
)", automaton->nodeAt(0).toString());

    EXPECT_EQ(R"(I₁
===
S' -> S · ﹐ ¥
---
)", automaton->nodeAt(1).toString());
    EXPECT_TRUE(automaton->nodeAt(1).accept);

    EXPECT_EQ(R"(I₂
===
S -> C · C ﹐ ¥
---
C -> · c C ﹐ ¥
   | · d ﹐ ¥
)", automaton->nodeAt(2).toString());

    EXPECT_EQ(R"(I₃
===
C -> c · C ﹐ c
   | c · C ﹐ d
   | c · C ﹐ ¥
---
C -> · c C ﹐ d
   | · d ﹐ d
   | · c C ﹐ c
   | · d ﹐ c
   | · c C ﹐ ¥
   | · d ﹐ ¥
)", automaton->nodeAt(3).toString());

    EXPECT_EQ(R"(I₄
===
C -> d · ﹐ c
   | d · ﹐ d
   | d · ﹐ ¥
---
)", automaton->nodeAt(4).toString());

    EXPECT_EQ(R"(I₅
===
S -> C C · ﹐ ¥
---
)", automaton->nodeAt(5).toString());

    EXPECT_EQ(R"(I₆
===
C -> c C · ﹐ c
   | c C · ﹐ d
   | c C · ﹐ ¥
---
)", automaton->nodeAt(6).toString());

    EXPECT_EQ(R"(0 -- S --> 1
0 -- C --> 2
0 -- c --> 3
0 -- d --> 4
2 -- C --> 5
2 -- c --> 3
2 -- d --> 4
3 -- C --> 6
3 -- c --> 3
3 -- d --> 4
)", automaton->edgesToString());

    const ::testing::TestInfo* info = ::testing::UnitTest::GetInstance()->current_test_info();
    auto svg = automaton->toSVG();
    auto filePath = format("{}_{}.svg", info->test_suite_name(), info->name());
    ofstream file(filePath);
    file << svg;
    file.close();

    auto table = grammar.computeLALR1ActionGotoTable(automaton);
    const auto expectedTable = R"(| State | c | d | ¥ | S | C |
|:-:|:-:|:-:|:-:|:-:|:-:|
| 0 | shift 3 | shift 4 |  | 1 | 2 |
| 1 |  |  | accept |  |  |
| 2 | shift 3 | shift 4 |  |  | 5 |
| 3 | shift 3 | shift 4 |  |  | 6 |
| 4 | reduce C -> d | reduce C -> d | reduce C -> d |  |  |
| 5 |  |  | reduce S -> C C |  |  |
| 6 | reduce C -> c C | reduce C -> c C | reduce C -> c C |  |  |
)";
    EXPECT_EQ(expectedTable, table.toString(grammar));
}

TEST(TestContextFreeGrammarLALR1Automaton, SpecialCase2) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S -> A a | B b
A -> a A b | a b
B -> a B b b | a b b
)");
    const auto automaton = grammar.computeLALR1Automaton();
    EXPECT_EQ(14, automaton->size());

    EXPECT_EQ(R"(I₈
===
B -> a b · b ﹐ b
A -> a b · ﹐ a
   | a b · ﹐ b
---
)", automaton->nodeAt(8).toString());

    const ::testing::TestInfo* info = ::testing::UnitTest::GetInstance()->current_test_info();
    auto svg = automaton->toSVG();
    auto filePath = format("{}_{}.svg", info->test_suite_name(), info->name());
    ofstream file(filePath);
    file << svg;
    file.close();

    auto table = grammar.computeLALR1ActionGotoTable(automaton);
    const auto expectedTable = R"(| State | a | b | ¥ | S | A | B |
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| 0 | shift 4 |  |  | 1 | 2 | 3 |
| 1 |  |  | accept |  |  |  |
| 2 | shift 5 |  |  |  |  |  |
| 3 |  | shift 6 |  |  |  |  |
| 4 | shift 4 | shift 8 |  |  | 9 | 7 |
| 5 |  |  | reduce S -> A a |  |  |  |
| 6 |  |  | reduce S -> B b |  |  |  |
| 7 |  | shift 10 |  |  |  |  |
| 8 | reduce A -> a b | shift 11 / reduce A -> a b |  |  |  |  |
| 9 |  | shift 12 |  |  |  |  |
| 10 |  | shift 13 |  |  |  |  |
| 11 |  | reduce B -> a b b |  |  |  |  |
| 12 | reduce A -> a A b | reduce A -> a A b |  |  |  |  |
| 13 |  | reduce B -> a B b b |  |  |  |  |
)";
    EXPECT_EQ(expectedTable, table.toString(grammar));
}
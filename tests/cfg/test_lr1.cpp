#include "cfg.h"
#include "automaton.h"
#include <gtest/gtest.h>
#include <ranges>
#include <algorithm>
#include <fstream>
#include <format>

using namespace std;

TEST(TestContextFreeGrammarLR1Automaton, Empty) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(""));
    EXPECT_THROW(grammar.computeLR1Automaton(), runtime_error);
}

TEST(TestContextFreeGrammarLR1Automaton, SpecialCase1) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S -> C C
C -> c C | d
)");
    const auto automaton = grammar.computeLR1Automaton();
    EXPECT_EQ(10, automaton->size());

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
---
C -> · c C ﹐ d
   | · d ﹐ d
   | · c C ﹐ c
   | · d ﹐ c
)", automaton->nodeAt(3).toString());

    EXPECT_EQ(R"(I₄
===
C -> d · ﹐ c
   | d · ﹐ d
---
)", automaton->nodeAt(4).toString());

    EXPECT_EQ(R"(I₅
===
S -> C C · ﹐ ¥
---
)", automaton->nodeAt(5).toString());

    EXPECT_EQ(R"(I₆
===
C -> c · C ﹐ ¥
---
C -> · c C ﹐ ¥
   | · d ﹐ ¥
)", automaton->nodeAt(6).toString());

    EXPECT_EQ(R"(I₇
===
C -> d · ﹐ ¥
---
)", automaton->nodeAt(7).toString());

    EXPECT_EQ(R"(I₈
===
C -> c C · ﹐ c
   | c C · ﹐ d
---
)", automaton->nodeAt(8).toString());

    EXPECT_EQ(R"(I₉
===
C -> c C · ﹐ ¥
---
)", automaton->nodeAt(9).toString());

    EXPECT_EQ(R"(0 -- S --> 1
0 -- C --> 2
0 -- c --> 3
0 -- d --> 4
2 -- C --> 5
2 -- c --> 6
2 -- d --> 7
3 -- C --> 8
3 -- c --> 3
3 -- d --> 4
6 -- C --> 9
6 -- c --> 6
6 -- d --> 7
)", automaton->edgesToString());

    const ::testing::TestInfo* info = ::testing::UnitTest::GetInstance()->current_test_info();
    auto svg = automaton->toSVG();
    auto filePath = format("{}_{}.svg", info->test_suite_name(), info->name());
    ofstream file(filePath);
    file << svg;
    file.close();

    auto table = grammar.computeLR1ActionGotoTable(automaton);
    const auto expectedTable = R"(| State | c | d | ¥ | S | C |
|:-:|:-:|:-:|:-:|:-:|:-:|
| 0 | shift 3 | shift 4 |  | 1 | 2 |
| 1 |  |  | accept |  |  |
| 2 | shift 6 | shift 7 |  |  | 5 |
| 3 | shift 3 | shift 4 |  |  | 8 |
| 4 | reduce C -> d | reduce C -> d |  |  |  |
| 5 |  |  | reduce S -> C C |  |  |
| 6 | shift 6 | shift 7 |  |  | 9 |
| 7 |  |  | reduce C -> d |  |  |
| 8 | reduce C -> c C | reduce C -> c C |  |  |  |
| 9 |  |  | reduce C -> c C |  |  |
)";
    EXPECT_EQ(expectedTable, table.toString(grammar));
}

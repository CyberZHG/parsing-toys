#include "cfg.h"
#include "automaton.h"
#include <gtest/gtest.h>
#include <ranges>
#include <algorithm>

TEST(TestContextFreeGrammarLR0Automaton, Empty) {
    ContextFreeGrammar grammar;
    EXPECT_TRUE(grammar.parse(""));
    EXPECT_THROW(grammar.computeLR0Automaton(), runtime_error);
}

TEST(TestContextFreeGrammarLR0Automaton, SpecialCase1) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
E -> E + T | T
T -> T * F | F
F -> ( E ) | id
)");
    const auto automaton = grammar.computeLR0Automaton();
    EXPECT_EQ(12, automaton->size());

    EXPECT_EQ(R"(I₀
===
E' -> · E
---
E -> · E + T
   | · T
T -> · T * F
   | · F
F -> · ( E )
   | · id
)", automaton->nodeAt(0).toString());

   EXPECT_EQ(R"(I₁
===
E' -> E ·
 E -> E · + T
---
)", automaton->nodeAt(1).toString());
    EXPECT_TRUE(automaton->nodeAt(1).accept);

    EXPECT_EQ(R"(I₂
===
E -> T ·
T -> T · * F
---
)", automaton->nodeAt(2).toString());

    EXPECT_EQ(R"(I₃
===
T -> F ·
---
)", automaton->nodeAt(3).toString());

    EXPECT_EQ(R"(I₄
===
F -> ( · E )
---
E -> · E + T
   | · T
T -> · T * F
   | · F
F -> · ( E )
   | · id
)", automaton->nodeAt(4).toString());

    EXPECT_EQ(R"(I₅
===
F -> id ·
---
)", automaton->nodeAt(5).toString());

    EXPECT_EQ(R"(I₆
===
E -> E + · T
---
T -> · T * F
   | · F
F -> · ( E )
   | · id
)", automaton->nodeAt(6).toString());

    EXPECT_EQ(R"(I₇
===
T -> T * · F
---
F -> · ( E )
   | · id
)", automaton->nodeAt(7).toString());

    EXPECT_EQ(R"(I₈
===
F -> ( E · )
E -> E · + T
---
)", automaton->nodeAt(8).toString());

    EXPECT_EQ(R"(I₉
===
E -> E + T ·
T -> T · * F
---
)", automaton->nodeAt(9).toString());

    EXPECT_EQ(R"(I₁₀
===
T -> T * F ·
---
)", automaton->nodeAt(10).toString());

    EXPECT_EQ(R"(I₁₁
===
F -> ( E ) ·
---
)", automaton->nodeAt(11).toString());

    EXPECT_EQ(R"(0 -- E --> 1
0 -- T --> 2
0 -- F --> 3
0 -- ( --> 4
0 -- id --> 5
1 -- + --> 6
2 -- * --> 7
4 -- E --> 8
4 -- ( --> 4
4 -- id --> 5
4 -- T --> 2
4 -- F --> 3
6 -- T --> 9
6 -- F --> 3
6 -- ( --> 4
6 -- id --> 5
7 -- F --> 10
7 -- ( --> 4
7 -- id --> 5
8 -- ) --> 11
8 -- + --> 6
9 -- * --> 7
)", automaton->edgesToString());
}

#include "cfg.h"
#include <gtest/gtest.h>
#include <ranges>
#include <algorithm>

using namespace std;

TEST(TestContextFreeGrammarClosure, Empty) {
    const ContextFreeGrammar grammar, kernel;
    const auto nonKernel = grammar.computeClosure(kernel);
    EXPECT_EQ("", nonKernel.toString());
}

TEST(TestContextFreeGrammarClosure, SpecialCase1) {
    ContextFreeGrammar grammar, kernel;
    grammar.parse(R"(E' -> E
                           E -> E + T | T
                           T -> T * F | F
                           F -> ( E ) | id)");
    kernel.parse("E' -> · E");
    const auto nonKernel = grammar.computeClosure(kernel);
    EXPECT_EQ(R"(E -> · E + T
   | · T
T -> · T * F
   | · F
F -> · ( E )
   | · id
)", nonKernel.toString());
}

TEST(TestContextFreeGrammarClosure, SpecialCase2) {
    ContextFreeGrammar grammar, kernel;
    grammar.parse(R"(S' → S
                          S  → A
                          A  → B
                          B  → c)");
    kernel.parse("S' → · S");
    const auto nonKernel = grammar.computeClosure(kernel);
   EXPECT_EQ(R"(S -> · A
A -> · B
B -> · c
)", nonKernel.toString());
}

TEST(TestContextFreeGrammarClosure, SpecialCase3) {
    ContextFreeGrammar grammar, kernel;
    grammar.parse(R"(S' → S
                          S  → a A
                          A  → b)");
    kernel.parse("S → a · A");
    const auto nonKernel = grammar.computeClosure(kernel);
    EXPECT_EQ(R"(A -> · b
)", nonKernel.toString());
}

TEST(TestContextFreeGrammarClosure, SpecialCase4) {
    ContextFreeGrammar grammar, kernel;
    grammar.parse(R"(E' → E
                          E  → E + T
                          E  → T
                          T  → id)");
    kernel.parse("E → · E + T");
    const auto nonKernel = grammar.computeClosure(kernel);
    EXPECT_EQ(R"(E -> · T
T -> · id
)", nonKernel.toString());
}

TEST(TestContextFreeGrammarClosure, SpecialCase5) {
    ContextFreeGrammar grammar, kernel;
    grammar.parse(R"(S' → S
                          S  → A B
                          A  → a
                          B  → b)");
    kernel.parse("S → A · B");
    const auto nonKernel = grammar.computeClosure(kernel);
    EXPECT_EQ(R"(B -> · b
)", nonKernel.toString());
}

TEST(TestContextFreeGrammarClosure, SpecialCase6) {
    ContextFreeGrammar grammar, kernel;
    grammar.parse(R"(S' → S
                          S  → A
                          A  → B | C
                          B  → A
                          C  → d)");
    kernel.parse("S → · A");
    const auto nonKernel = grammar.computeClosure(kernel);
    EXPECT_EQ(R"(A -> · B
   | · C
B -> · A
C -> · d
)", nonKernel.toString());
}

TEST(TestContextFreeGrammarClosure, SpecialCase7) {
    ContextFreeGrammar grammar, kernel;
    grammar.parse(R"(S' → S
                          S  → A B
                          A  → ε | a
                          B  → b)");
    kernel.parse("S → · A B");
    const auto nonKernel = grammar.computeClosure(kernel);
    EXPECT_EQ(R"(A -> ·
   | · a
)", nonKernel.toString());
}

TEST(TestContextFreeGrammarClosure, SpecialCase8) {
    ContextFreeGrammar grammar, kernel;
    grammar.parse(R"(S' → S
                          S  → A C
                          A  → ε
                          C  → D
                          D  → d)");
    kernel.parse("S → · A C");
    const auto nonKernel = grammar.computeClosure(kernel);
    EXPECT_EQ(R"(A -> ·
)", nonKernel.toString());
}

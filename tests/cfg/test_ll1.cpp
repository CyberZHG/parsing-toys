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
    auto table = grammar.computeLL1Table();
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
    auto svg = table.parseTree->toSVG();
    auto filePath = format("{}_{}.svg", info->test_suite_name(), info->name());
    ofstream file(filePath);
    file << svg;
    file.close();
}

TEST(TestContextFreeGrammarLL1Automaton, SpecialCase2) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S -> i E t S
   | i E t S e S
   | a
E -> b
)");
    grammar.leftRecursionElimination();
    grammar.leftFactoring();
    auto table = grammar.computeLL1Table();
    const auto expectedTable = R"(| | a | b | e | i | t | ε | ¥ |
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| S | S -> a |  |  | S -> i E t S S' |  |  |  |
| S' |  |  | S' -> e S / S' -> ε |  |  |  | S' -> ε |
| E |  | E -> b |  |  |  |  |  |
)";
    EXPECT_EQ(expectedTable, table.toString());

    const auto steps = table.parse("i b t a e a");
    const auto expectedSteps = R"(| Stack | Input | Action |
|:-:|:-:|:-:|
| ¥ S | i b t a e a ¥ | S -> i E t S S' |
| ¥ S' S t E i | i b t a e a ¥ | match i |
| ¥ S' S t E | b t a e a ¥ | E -> b |
| ¥ S' S t b | b t a e a ¥ | match b |
| ¥ S' S t | t a e a ¥ | match t |
| ¥ S' S | a e a ¥ | S -> a |
| ¥ S' a | a e a ¥ | match a |
| ¥ S' | e a ¥ | conflict: S' -> e S / S' -> ε |
)";
    EXPECT_EQ(expectedSteps, steps.toString());
}

TEST(TestContextFreeGrammarLL1Automaton, SpecialCase3) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S → A a | b
A → ε | b
)");
    grammar.leftRecursionElimination();
    grammar.leftFactoring();
    const auto table = grammar.computeLL1Table();
    const auto expectedTable = R"(| | a | b | ¥ |
|:-:|:-:|:-:|:-:|
| S | S -> A a | S -> A a / S -> b |  |
| A | A -> ε | A -> b |  |
)";
    EXPECT_EQ(expectedTable, table.toString());
}

TEST(TestContextFreeGrammarLL1Automaton, SpecialCase4) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S → A c | B d
A → a | ε
B → a
)");
    grammar.leftRecursionElimination();
    grammar.leftFactoring();
    const auto table = grammar.computeLL1Table();
    const auto expectedTable = R"(| | a | c | d | ¥ |
|:-:|:-:|:-:|:-:|:-:|
| S | S -> A c / S -> B d | S -> A c |  |  |
| A | A -> a | A -> ε |  |  |
| B | B -> a |  |  |  |
)";
    EXPECT_EQ(expectedTable, table.toString());
}

TEST(TestContextFreeGrammarLL1Automaton, SpecialCase5) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S → A B
A → ε | a
B → b | ε
)");
    grammar.leftRecursionElimination();
    grammar.leftFactoring();
    const auto table = grammar.computeLL1Table();
    const auto expectedTable = R"(| | a | b | ¥ |
|:-:|:-:|:-:|:-:|
| S | S -> A B | S -> A B | S -> A B |
| A | A -> a | A -> ε | A -> ε |
| B |  | B -> b | B -> ε |
)";
    EXPECT_EQ(expectedTable, table.toString());
}

TEST(TestContextFreeGrammarLL1Automaton, SpecialCase6) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S → a S b | a b
)");
    grammar.leftRecursionElimination();
    grammar.leftFactoring();
    const auto table = grammar.computeLL1Table();
    const auto expectedTable = R"(| | a | b | ¥ |
|:-:|:-:|:-:|:-:|
| S | S -> a S' |  |  |
| S' | S' -> S b | S' -> b |  |
)";
    EXPECT_EQ(expectedTable, table.toString());
}

TEST(TestContextFreeGrammarLL1Automaton, SpecialCase7) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S → if E then S
  | if E then S else S
  | other
)");
    grammar.leftRecursionElimination();
    grammar.leftFactoring();
    const auto table = grammar.computeLL1Table();
    const auto expectedTable = R"(| | E | else | if | other | then | ε | ¥ |
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| S |  |  | S -> if E then S S' | S -> other |  |  |  |
| S' |  | S' -> else S / S' -> ε |  |  |  |  | S' -> ε |
)";
    EXPECT_EQ(expectedTable, table.toString());
}

TEST(TestContextFreeGrammarLL1Automaton, SpecialCase8) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
E → T | T + E
T → id
)");
    grammar.leftRecursionElimination();
    grammar.leftFactoring();
    const auto table = grammar.computeLL1Table();
    const auto expectedTable = R"(| | + | id | ε | ¥ |
|:-:|:-:|:-:|:-:|:-:|
| E |  | E -> T E' |  |  |
| E' | E' -> + E |  |  | E' -> ε |
| T |  | T -> id |  |  |
)";
    EXPECT_EQ(expectedTable, table.toString());
}

TEST(TestContextFreeGrammarLL1Automaton, SpecialCase9) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S → A | b
A → ε | b
)");
    grammar.leftRecursionElimination();
    grammar.leftFactoring();
    const auto table = grammar.computeLL1Table();
    const auto expectedTable = R"(| | b | ¥ |
|:-:|:-:|:-:|
| S | S -> A / S -> b | S -> A |
| A | A -> b | A -> ε |
)";
    EXPECT_EQ(expectedTable, table.toString());
}

TEST(TestContextFreeGrammarLL1Table, MTableDirect) {
    MTable table;
    table.addEntry("S", "a", {"a"});
    table.addEntry("S", "b", {"b"});
    table.addEntry("A", "c", {"c"});
    EXPECT_EQ(2, table.numNonTerminals());
    EXPECT_EQ(3, table.numTerminals());
    EXPECT_EQ("S", table.getNonTerminal(0));
    EXPECT_EQ("A", table.getNonTerminal(1));
    EXPECT_EQ("a", table.getTerminal(0));
    EXPECT_EQ("b", table.getTerminal(1));
    EXPECT_EQ("c", table.getTerminal(2));
}

TEST(TestContextFreeGrammarLL1Table, HasConflict) {
    MTable table;
    table.addEntry("S", "a", {"a"});
    table.addEntry("S", "b", {"b"});
    EXPECT_FALSE(table.hasConflict());
    EXPECT_FALSE(table.hasConflict("S", "a"));
    table.addEntry("S", "a", {"A"});
    EXPECT_TRUE(table.hasConflict());
    EXPECT_TRUE(table.hasConflict("S", "a"));
    EXPECT_FALSE(table.hasConflict("X", "a"));
    EXPECT_FALSE(table.hasConflict("S", "x"));
    EXPECT_EQ("", table.getCell("X", "a"));
    EXPECT_EQ("", table.getCell("S", "x"));
}

TEST(TestContextFreeGrammarLL1Table, HasConflictBound) {
    MTable table;
    table.addEntry("S", "a", {"a"});
    table.addEntry("A", "z", {"z"});
    EXPECT_FALSE(table.hasConflict("A", "a"));
}

TEST(TestContextFreeGrammarLL1Table, GetCellBoundsCheck) {
    MTable table;
    table.addEntry("S", "a", {"a"});
    table.addEntry("A", "z", {"z"});
    EXPECT_EQ("", table.getCell("A", "a"));
}

TEST(TestContextFreeGrammarLL1Parsing, TerminalMismatch) {
    ContextFreeGrammar grammar;
    grammar.parse("S -> a b");
    auto table = grammar.computeLL1Table();
    const auto steps = table.parse("a c");
    const auto result = steps.toString();
    EXPECT_TRUE(result.find("error: expected b") != string::npos);
}

TEST(TestContextFreeGrammarLL1Parsing, UnexpectedSymbol) {
    MTable table;
    table.addEntry("S", "a", {"a"});
    table.nonTerminals = {"S"};
    table.nonTerminalIndex["S"] = 0;
    table.terminals = {"a", "¥"};
    table.terminalIndex["a"] = 0;
    table.terminalIndex["¥"] = 1;
    table.entries.resize(1);
    table.entries[0].resize(2);
    table.entries[0][0] = {{"a"}};
    const auto steps = table.parse("x");
    const auto result = steps.toString();
    EXPECT_TRUE(result.find("error: unexpected symbol") != string::npos);
}

TEST(TestContextFreeGrammarLL1Parsing, NoRuleError) {
    MTable table;
    table.nonTerminals = {"S"};
    table.nonTerminalIndex["S"] = 0;
    table.terminals = {"a", "b", "¥"};
    table.terminalIndex["a"] = 0;
    table.terminalIndex["b"] = 1;
    table.terminalIndex["¥"] = 2;
    table.entries.resize(1);
    table.entries[0].resize(3);
    table.entries[0][0] = {{"a"}};
    const auto steps = table.parse("b");
    const auto result = steps.toString();
    EXPECT_TRUE(result.find("error: no rule") != string::npos);
}

TEST(TestContextFreeGrammarLL1Parsing, EmptyStackAndInput) {
    ContextFreeGrammar grammar;
    grammar.parse("S -> ε");
    auto table = grammar.computeLL1Table();

    const auto steps = table.parse("");
    EXPECT_TRUE(steps.toString().find("accept") != string::npos);
    EXPECT_NE(nullptr, table.parseTree);
}

TEST(TestContextFreeGrammarLL1Parsing, ComputeFirstOfProductionTerminal) {
    ContextFreeGrammar grammar;
    grammar.parse(R"(
S -> a b
)");
    const auto table = grammar.computeLL1Table();
    EXPECT_TRUE(table.getCell("S", "a").find("S -> a b") != string::npos);
}

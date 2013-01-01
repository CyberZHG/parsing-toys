#include "cfg.h"
#include <gtest/gtest.h>
#include <ranges>
#include <algorithm>

TEST(TestPrimedSymbol, AddProduction1) {
    ContextFreeGrammar grammar;
    grammar.addProductions("B", {{"b"}});
    grammar.addProductions("A", {{"a"}});
    grammar.addProductions("B", {{"c"}});
    grammar.addProductions("A", {{"d"}});
    EXPECT_EQ("B -> b\n   | c\nA -> a\n   | d\n", grammar.toString());
}

TEST(TestPrimedSymbol, PrimedNotExist) {
    ContextFreeGrammar grammar;
    EXPECT_THROW(grammar.generatePrimedSymbol("A"), runtime_error);
}

TEST(TestPrimedSymbol, Primed1) {
    ContextFreeGrammar grammar;
    grammar.addProductions("A", {{"a"}});
    grammar.addProductions("B", {{"b"}});
    grammar.addProductions(grammar.generatePrimedSymbol("A"), {{"c"}});
    EXPECT_EQ(" A -> a\nA' -> c\n B -> b\n", grammar.toString());
    grammar.addProductions(grammar.generatePrimedSymbol("A"), {{"d"}});
    EXPECT_EQ("   A -> a\n  A' -> c\nA'_1 -> d\n   B -> b\n", grammar.toString());
    grammar.addProductions(grammar.generatePrimedSymbol("A"), {{"e"}});
    EXPECT_EQ("   A -> a\n  A' -> c\nA'_1 -> d\nA'_2 -> e\n   B -> b\n", grammar.toString());
}

TEST(TestPrimedSymbol, Primed2) {
    ContextFreeGrammar grammar;
    grammar.addProductions("A", {{"a"}});
    grammar.addProductions("A'", {{"c"}});
    grammar.addProductions("B", {{"b"}});
    grammar.addProductions(grammar.generatePrimedSymbol("A"), {{"d"}});
    EXPECT_EQ("   A -> a\n  A' -> c\nA'_1 -> d\n   B -> b\n", grammar.toString());
}

TEST(TestPrimedSymbol, Primed3) {
    ContextFreeGrammar grammar;
    grammar.addProductions("A", {{"a"}});
    grammar.addProductions("A'", {{"c"}});
    grammar.addProductions("A'_2", {{"e"}});
    grammar.addProductions("B", {{"b"}});
    grammar.addProductions(grammar.generatePrimedSymbol("A"), {{"d"}});
    EXPECT_EQ("   A -> a\n  A' -> c\nA'_1 -> d\nA'_2 -> e\n   B -> b\n", grammar.toString());
}

TEST(TestPrimedSymbol, PrimedExistingPrime1) {
    ContextFreeGrammar grammar;
    grammar.addProductions("A'", {{"c"}});
    grammar.addProductions("B", {{"b"}});
    grammar.addProductions(grammar.generatePrimedSymbol("A'"), {{"d"}});
    EXPECT_EQ("  A' -> c\nA'_1 -> d\n   B -> b\n", grammar.toString());
}

TEST(TestPrimedSymbol, PrimedExistingPrime2) {
    ContextFreeGrammar grammar;
    grammar.addProductions("A'", {{"a"}});
    grammar.addProductions("A'_1", {{"b"}});
    grammar.addProductions("A'_2", {{"c"}});
    grammar.addProductions("B", {{"d"}});
    grammar.addProductions(grammar.generatePrimedSymbol("A'_1"), {{"e"}});
    EXPECT_EQ("  A' -> a\nA'_1 -> b\nA'_2 -> c\nA'_3 -> e\n   B -> d\n", grammar.toString());
}

TEST(TestPrimedSymbol, PrimedNoPrime1) {
    ContextFreeGrammar grammar;
    grammar.addProductions("A'B", {{"a"}});
    grammar.addProductions(grammar.generatePrimedSymbol("A'B"), {{"b"}});
    EXPECT_EQ(" A'B -> a\nA'B' -> b\n", grammar.toString());
}

TEST(TestPrimedSymbol, PrimedNoPrime2) {
    ContextFreeGrammar grammar;
    grammar.addProductions("A'_", {{"a"}});
    grammar.addProductions(grammar.generatePrimedSymbol("A'_"), {{"b"}});
    EXPECT_EQ(" A'_ -> a\nA'_' -> b\n", grammar.toString());
}

TEST(TestPrimedSymbol, PrimedNoPrime3) {
    ContextFreeGrammar grammar;
    grammar.addProductions("A'_d", {{"a"}});
    grammar.addProductions(grammar.generatePrimedSymbol("A'_d"), {{"b"}});
    EXPECT_EQ(" A'_d -> a\nA'_d' -> b\n", grammar.toString());
}

TEST(TestPrimedSymbol, PrimedNoPrime4) {
    ContextFreeGrammar grammar;
    grammar.addProductions("A'_1d", {{"a"}});
    grammar.addProductions(grammar.generatePrimedSymbol("A'_1d"), {{"b"}});
    EXPECT_EQ(" A'_1d -> a\nA'_1d' -> b\n", grammar.toString());
}

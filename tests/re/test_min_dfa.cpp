#include "re.h"
#include <gtest/gtest.h>
#include <queue>
#include <ranges>
#include <unordered_set>

using namespace std;

TEST(TestMinDFA, Epsilon) {
    const RegularExpression re("ε");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->key, "A");
    EXPECT_EQ(minDfa->edges.size(), 0);
}

TEST(TestMinDFA, StarAB) {
    const RegularExpression re("(a|b)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->key, "A,B,C");
    EXPECT_EQ(minDfa->edges.size(), 1);
    ASSERT_NE(minDfa->trans.find("a,b"), minDfa->trans.end());
    EXPECT_EQ(minDfa->trans.at("a,b")->key, "A,B,C");
}

TEST(TestMinDFA, PlusAB) {
    const RegularExpression re("(a|b)+");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->key, "A");
    EXPECT_EQ(minDfa->edges.size(), 1);
    ASSERT_NE(minDfa->trans.find("a,b"), minDfa->trans.end());
    const auto next = minDfa->trans.at("a,b");
    EXPECT_EQ(next->key, "B,C,D,E");
    EXPECT_EQ(next->edges.size(), 1);
    ASSERT_NE(next->trans.find("a,b"), next->trans.end());
    EXPECT_EQ(next->trans.at("a,b")->key, "B,C,D,E");
}

TEST(TestMinDFA, ABcStar) {
    const RegularExpression re("a(b|c)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->key, "A");
    EXPECT_EQ(minDfa->edges.size(), 1);
    ASSERT_NE(minDfa->trans.find("a"), minDfa->trans.end());
    const auto next = minDfa->trans.at("a");
    EXPECT_EQ(next->key, "B,C,D");
    EXPECT_EQ(next->edges.size(), 1);
    ASSERT_NE(next->trans.find("b,c"), next->trans.end());
    EXPECT_EQ(next->trans.at("b,c")->key, "B,C,D");
}

TEST(TestMinDFA, StarABStarCD) {
    const RegularExpression re("(a|b)*(c|d)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->key, "A,B,C");
    EXPECT_EQ(minDfa->edges.size(), 2);
    ASSERT_NE(minDfa->trans.find("a,b"), minDfa->trans.end());
    EXPECT_EQ(minDfa->trans.at("a,b")->key, "A,B,C");
    ASSERT_NE(minDfa->trans.find("c,d"), minDfa->trans.end());
    const auto cd = minDfa->trans.at("c,d");
    EXPECT_EQ(cd->key, "D,E");
    EXPECT_EQ(cd->edges.size(), 1);
    ASSERT_NE(cd->trans.find("c,d"), cd->trans.end());
    EXPECT_EQ(cd->trans.at("c,d")->key, "D,E");
}

TEST(TestMinDFA, ABOrB) {
    const RegularExpression re("ab|b");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->key, "A");
    EXPECT_EQ(minDfa->edges.size(), 2);
    ASSERT_NE(minDfa->trans.find("a"), minDfa->trans.end());
    EXPECT_EQ(minDfa->trans.at("a")->key, "B");
    ASSERT_NE(minDfa->trans.find("b"), minDfa->trans.end());
    EXPECT_EQ(minDfa->trans.at("b")->key, "C,D");

    const auto nodeA = minDfa->trans.at("a");
    EXPECT_EQ(nodeA->edges.size(), 1);
    ASSERT_NE(nodeA->trans.find("b"), nodeA->trans.end());
    EXPECT_EQ(nodeA->trans.at("b")->key, "C,D");
}

TEST(TestMinDFA, DFA) {
    const RegularExpression re("a");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    ASSERT_NE(dfa, nullptr);
    EXPECT_EQ(dfa->id, "A");
    EXPECT_EQ(dfa->type, "");
    EXPECT_EQ(dfa->symbols.size(), 1);
    EXPECT_EQ(dfa->symbols[0], "a");
    ASSERT_NE(dfa->trans.find("a"), dfa->trans.end());
    EXPECT_EQ(dfa->trans.at("a")->type, "accept");
}

TEST(TestMinDFA, NullDFA) {
    const auto minDfa = RegularExpression::toMinDFA(nullptr);
    EXPECT_EQ(minDfa, nullptr);
}

TEST(TestMinDFA, NullNFA) {
    const auto dfa = RegularExpression::toDFA(nullptr);
    EXPECT_EQ(dfa, nullptr);
}

TEST(TestMinDFA, DFAAcceptStart) {
    const RegularExpression re("ε");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    ASSERT_NE(dfa, nullptr);
    EXPECT_EQ(dfa->type, "accept");
}

TEST(TestMinDFA, DFAMultipleSymbols) {
    const RegularExpression re("a|b");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    ASSERT_NE(dfa, nullptr);
    EXPECT_EQ(dfa->symbols.size(), 2);
}

TEST(TestMinDFA, ComplexDFA) {
    const RegularExpression re("(a|b)*abb");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    ASSERT_NE(dfa, nullptr);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
}

TEST(TestDFAGraph, BasicOperations) {
    const RegularExpression re("a|b");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto graph = RegularExpression::toDFAGraph(dfa);

    EXPECT_GT(graph.size(), 0u);
    EXPECT_GT(graph.numEdges(), 0u);

    for (size_t i = 0; i < graph.numEdges(); ++i) {
        EXPECT_LT(graph.edgeFrom(i), graph.size());
        EXPECT_LT(graph.edgeTo(i), graph.size());
        EXPECT_FALSE(graph.edgeLabel(i).empty());
    }
}

TEST(TestDFAGraph, NullDFA) {
    const auto graph = RegularExpression::toDFAGraph(nullptr);
    EXPECT_EQ(graph.size(), 0u);
    EXPECT_EQ(graph.numEdges(), 0u);
}

TEST(TestDFAGraph, StateAttributes) {
    const RegularExpression re("a");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto graph = RegularExpression::toDFAGraph(dfa);

    ASSERT_GT(graph.size(), 0u);
    EXPECT_FALSE(graph.stateIdAt(0).empty());
    EXPECT_FALSE(graph.stateKeyAt(0).empty());
}

TEST(TestDFAGraph, StateTypes) {
    const RegularExpression re("a");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto graph = RegularExpression::toDFAGraph(dfa);

    bool hasAccept = false;
    for (size_t i = 0; i < graph.size(); ++i) {
        if (graph.stateTypeAt(i) == "accept") {
            hasAccept = true;
            break;
        }
    }
    EXPECT_TRUE(hasAccept);
}

TEST(TestDFAGraph, ToSVG) {
    const RegularExpression re("a*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto graph = RegularExpression::toDFAGraph(dfa);

    const auto svg = graph.toSVG(false);
    EXPECT_FALSE(svg.empty());
    EXPECT_NE(svg.find("<svg"), string::npos);
}

TEST(TestDFAGraph, ToSVGDarkMode) {
    const RegularExpression re("a*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto graph = RegularExpression::toDFAGraph(dfa);

    const auto svg = graph.toSVG(true);
    EXPECT_FALSE(svg.empty());
    EXPECT_NE(svg.find("<svg"), string::npos);
}

TEST(TestMinDFAGraph, BasicOperations) {
    const RegularExpression re("(a|b)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    const auto graph = RegularExpression::toDFAGraph(minDfa);

    EXPECT_GT(graph.size(), 0u);
}

TEST(TestMinDFAGraph, ToSVG) {
    const RegularExpression re("(a|b)*abb");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    const auto graph = RegularExpression::toDFAGraph(minDfa);

    const auto svg = graph.toSVG(false);
    EXPECT_FALSE(svg.empty());
    EXPECT_NE(svg.find("<svg"), string::npos);
}

TEST(TestDFA, ManyStates) {
    const RegularExpression re("(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A)(a|b|c)");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    ASSERT_NE(dfa, nullptr);

    unordered_set<string> visited;
    queue<shared_ptr<DFAState>> q;
    q.push(dfa);
    visited.insert(dfa->id);
    while (!q.empty()) {
        const auto state = q.front();
        q.pop();
        for (const auto &next: state->edges | views::values) {
            if (!visited.contains(next->id)) {
                visited.insert(next->id);
                q.push(next);
            }
        }
    }
    EXPECT_GT(visited.size(), 26u);
}

TEST(TestDFA, DuplicateTargetInMove) {
    const RegularExpression re("(a|a)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    ASSERT_NE(dfa, nullptr);
}

TEST(TestMinDFA, RevisitedKeySplit) {
    const RegularExpression re("(a|b)*a(a|b)(a|b)");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
}

TEST(TestMinDFA, MultiElementPartitionSort) {
    const RegularExpression re("(a|b|c)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_FALSE(minDfa->key.empty());
}

TEST(TestMinDFA, StartNotFirstPartition) {
    const RegularExpression re("b*a");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->id, "1");
}

TEST(TestMinDFA, SwapPartitionsComplex) {
    const RegularExpression re("c(a|b)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->id, "1");
}

TEST(TestMinDFA, SinglePartOr) {
    const RegularExpression re("abc");
    const auto nfa = re.toNFA();
    ASSERT_NE(nfa, nullptr);
    const auto dfa = RegularExpression::toDFA(nfa);
    ASSERT_NE(dfa, nullptr);
}

TEST(TestMinDFA, MultiElementPartitionSortDetailed) {
    const RegularExpression re("(a|b|c|d)*e");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
}

TEST(TestMinDFA, PartitionRevisit) {
    const RegularExpression re("(a|b)*abb");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    ASSERT_NE(dfa, nullptr);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);

    unordered_set<string> visited;
    queue<shared_ptr<DFAState>> q;
    q.push(minDfa);
    visited.insert(minDfa->id);
    while (!q.empty()) {
        auto state = q.front();
        q.pop();
        for (const auto &next: state->edges | views::values) {
            if (!visited.contains(next->id)) {
                visited.insert(next->id);
                q.push(next);
            }
        }
    }
    EXPECT_EQ(visited.size(), 4u);
}

TEST(TestMinDFA, StartNotInFirstPartitionAfterSort) {
    const RegularExpression re("z(a|b)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->id, "1");
}

TEST(TestMinDFA, LargeAlphabetMinimization) {
    const RegularExpression re("(a|b|c|d|e|f|g|h)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_FALSE(minDfa->edges.empty());
}

TEST(TestMinDFA, HopcroftRevisitSplit) {
    const RegularExpression re("(a|b)*(aa|bb)");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
}

TEST(TestMinDFA, BuildMinDFASortMultiElement) {
    const RegularExpression re("(a|b)+");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);

    unordered_set<string> origStates;
    queue<shared_ptr<DFAState>> q1;
    q1.push(dfa);
    origStates.insert(dfa->id);
    while (!q1.empty()) {
        auto state = q1.front();
        q1.pop();
        for (const auto &next: state->edges | views::values) {
            if (!origStates.contains(next->id)) {
                origStates.insert(next->id);
                q1.push(next);
            }
        }
    }

    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);

    unordered_set<string> minStates;
    queue<shared_ptr<DFAState>> q2;
    q2.push(minDfa);
    minStates.insert(minDfa->id);
    while (!q2.empty()) {
        auto state = q2.front();
        q2.pop();
        for (const auto &next: state->edges | views::values) {
            if (!minStates.contains(next->id)) {
                minStates.insert(next->id);
                q2.push(next);
            }
        }
    }

    EXPECT_EQ(minStates.size(), 2u);
    EXPECT_LT(minStates.size(), origStates.size());
}

TEST(TestMinDFA, SwapStartPartition) {
    const RegularExpression re("a(b|c|d|e|f)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
    EXPECT_EQ(minDfa->id, "1");
}

TEST(TestMinDFA, MultiElementPartitionA) {
    const RegularExpression re("(a|b|c)*d");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
}

TEST(TestMinDFA, MultiElementPartitionB) {
    const RegularExpression re("a(b|c)*d(e|f)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
}

TEST(TestMinDFA, ComplexMinimization) {
    const RegularExpression re("(ab|ac|ad)*");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);
    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);
}

TEST(TestMinDFA, DragonBookExample) {
    const RegularExpression re("(a|b)*abb");
    const auto nfa = re.toNFA();
    const auto dfa = RegularExpression::toDFA(nfa);

    unordered_set<string> dfaStates;
    queue<shared_ptr<DFAState>> q;
    q.push(dfa);
    dfaStates.insert(dfa->id);
    while (!q.empty()) {
        auto s = q.front();
        q.pop();
        for (const auto &next: s->edges | views::values) {
            if (!dfaStates.contains(next->id)) {
                dfaStates.insert(next->id);
                q.push(next);
            }
        }
    }

    const auto minDfa = RegularExpression::toMinDFA(dfa);
    ASSERT_NE(minDfa, nullptr);

    unordered_set<string> minStates;
    queue<shared_ptr<DFAState>> q2;
    q2.push(minDfa);
    minStates.insert(minDfa->id);
    while (!q2.empty()) {
        auto s = q2.front();
        q2.pop();
        for (const auto &next: s->edges | views::values) {
            if (!minStates.contains(next->id)) {
                minStates.insert(next->id);
                q2.push(next);
            }
        }
    }

    EXPECT_LT(minStates.size(), dfaStates.size());
}

#ifndef PARSINGTOYS_AUTOMATON_H
#define PARSINGTOYS_AUTOMATON_H

#include "cfg.h"
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

struct FiniteAutomatonNode {
    bool accept = false;
    string label;
    ContextFreeGrammar kernel;
    ContextFreeGrammar nonKernel;

    /**
     * For unit tests only.
     * @return
     */
    string toString() const;
};

struct FiniteAutomatonEdge {
    int u, v;
    string label;
};

class FiniteAutomaton {
public:
    FiniteAutomaton() = default;
    ~FiniteAutomaton() = default;

    size_t size() const;
    FiniteAutomatonNode& nodeAt(size_t i);
    string newNodeLabel(const string& prefix = "I") const;
    size_t addNode(const FiniteAutomatonNode& node);
    size_t addEdge(const FiniteAutomatonEdge& edge);
    size_t addEdge(int u, int v, const string& label);

    /**
     * For unit tests only.
     * @return
     */
    string edgesToString() const;

private:
    vector<FiniteAutomatonNode> _nodes;
    vector<FiniteAutomatonEdge> _edges;
    unordered_map<string, size_t> _keyToNodeIndex;
};

#endif //PARSINGTOYS_AUTOMATON_H

#include "automaton.h"
#include "string_utils.h"

using namespace std;

string FiniteAutomatonNode::toString() const {
    string result = label + "\n===\n";
    result += kernel.toString() + "---\n";
    result += nonKernel.toString();
    return result;
}

size_t FiniteAutomaton::size() const {
    return _nodes.size();
}

FiniteAutomatonNode& FiniteAutomaton::nodeAt(const size_t i) {
    return _nodes[i];
}

string FiniteAutomaton::newNodeLabel(const string& prefix) const {
    return prefix + toSubscript(_nodes.size());
}

size_t FiniteAutomaton::addNode(const FiniteAutomatonNode& node) {
    const auto key = node.kernel.toString() + "---\n" + node.nonKernel.toString();
    if (const auto it = _keyToNodeIndex.find(key); it != _keyToNodeIndex.end()) {
        return it->second;
    }
    const size_t index = _nodes.size();
    _keyToNodeIndex[key] = index;
    _nodes.emplace_back(node);
    return index;
}

size_t FiniteAutomaton::addEdge(const FiniteAutomatonEdge& edge) {
    const size_t index = _edges.size();
    _edges.emplace_back(edge);
    return index;
}

size_t FiniteAutomaton::addEdge(const size_t u, const size_t v, const string& label) {
    FiniteAutomatonEdge edge;
    edge.u = u;
    edge.v = v;
    edge.label = label;
    return addEdge(edge);
}

string FiniteAutomaton::edgesToString() const {
    string result;
    for (const auto&[u, v, label] : _edges) {
        result += to_string(u) + " -- " + label + " --> " + to_string(v) + "\n";
    }
    return result;
}


#ifndef PARSING_TOYS_AUTOMATON_H
#define PARSING_TOYS_AUTOMATON_H

#include "cfg.h"
#include <string>
#include <vector>
#include <unordered_map>

struct FiniteAutomatonNode {
    bool accept = false;
    std::string label;
    ContextFreeGrammar kernel;
    ContextFreeGrammar nonKernel;

    /** For unit tests only. */
    [[nodiscard]] std::string toString() const;
};

struct FiniteAutomatonEdge {
    size_t u, v;
    std::string label;
};

class FiniteAutomaton {
public:
    FiniteAutomaton() = default;
    ~FiniteAutomaton() = default;

    [[nodiscard]] const std::vector<FiniteAutomatonEdge>& edges() const;

    [[nodiscard]] std::size_t size() const;
    FiniteAutomatonNode& nodeAt(std::size_t i);
    [[nodiscard]] std::string newNodeLabel(const std::string& prefix = "I") const;
    std::size_t addNode(const FiniteAutomatonNode& node);
    std::size_t addEdge(const FiniteAutomatonEdge& edge);
    std::size_t addEdge(std::size_t u, std::size_t v, const std::string& label);

    [[nodiscard]] std::string toSVG(bool darkMode = false) const;

    /**
     * For unit tests only.
     * @return
     */
    [[nodiscard]] std::string edgesToString() const;

private:
    std::vector<FiniteAutomatonNode> _nodes;
    std::vector<FiniteAutomatonEdge> _edges;
    std::unordered_map<std::string, std::size_t> _keyToNodeIndex;
};

#endif //PARSING_TOYS_AUTOMATON_H

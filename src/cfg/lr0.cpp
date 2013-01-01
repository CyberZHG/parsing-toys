#include "cfg.h"
#include "automaton.h"
#include <unordered_set>

using namespace std;

unique_ptr<FiniteAutomaton> ContextFreeGrammar::computeLR0Automaton() {
    if (_ordering.empty()) {
        throw runtime_error("Can not find a start symbol");
    }
    const auto& startSymbol = _ordering[0];
    const auto newStartSymbol = generatePrimedSymbol(startSymbol, false);
    auto automaton = make_unique<FiniteAutomaton>();
    FiniteAutomatonNode initialNode;
    initialNode.label = automaton->newNodeLabel();
    initialNode.kernel.addProduction(newStartSymbol, {DOT_SYMBOL, startSymbol});
    initialNode.nonKernel = computeClosure(initialNode.kernel);
    automaton->addNode(initialNode);
    for (size_t u = 0; u < automaton->size(); ++u) {
        auto& node = automaton->nodeAt(u);
        auto grammar = node.kernel | node.nonKernel;
        unordered_set<Symbol> transitionSymbolSet;
        vector<Symbol> transitionSymbols;
        // Find all available transitions.
        for (const auto& head : grammar._ordering) {
            const auto& productions = grammar._productions.at(head);
            for (const auto& production : productions) {
                for (size_t i = 0; i < production.size(); ++i) {
                    if (production[i] == DOT_SYMBOL) {
                        if (i + 1 == production.size()) {
                            if (head == newStartSymbol) {
                                node.accept = true;
                            }
                        } else {
                            const auto& symbol = production[i + 1];
                            if (!transitionSymbolSet.contains(symbol)) {
                                transitionSymbolSet.insert(symbol);
                                transitionSymbols.emplace_back(symbol);
                            }
                        }
                        break;
                    }
                }
            }
        }
        // Generate a new state for each symbol.
        for (const auto& transitionSymbol : transitionSymbols) {
            FiniteAutomatonNode newNode;
            newNode.label = automaton->newNodeLabel();
            for (const auto& head : grammar._ordering) {
                const auto& productions = grammar._productions.at(head);
                for (const auto& production : productions) {
                    for (size_t i = 0; i < production.size(); ++i) {
                        if (production[i] == DOT_SYMBOL) {
                            if (i + 1 >= production.size()) {
                                break;
                            }
                            if (production[i + 1] == transitionSymbol) {
                                auto newProduction = production;
                                swap(newProduction[i], newProduction[i + 1]);
                                newNode.kernel.addProduction(head, newProduction);
                            }
                            break;
                        }
                    }
                }
            }
            newNode.nonKernel = computeClosure(newNode.kernel);
            const size_t v = automaton->addNode(newNode);
            automaton->addEdge(u, v, transitionSymbol);
        }
    }
    return automaton;
}
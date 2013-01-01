#include "cfg.h"
#include "automaton.h"
#include <unordered_set>

using namespace std;

/**
 * Build LR(0) automaton (canonical collection of LR(0) item sets).
 *
 * Each state contains:
 * - kernel: items that define the state (from GOTO transitions)
 * - nonKernel: items added by closure
 *
 * Algorithm:
 * 1. Create initial state with S' -> · S (augmented grammar)
 * 2. For each state, find all symbols after the dot (·)
 * 3. For each such symbol X, create GOTO(state, X) by advancing the dot
 * 4. Deduplicate states by their item sets
 */
unique_ptr<FiniteAutomaton> ContextFreeGrammar::computeLR0Automaton() {
    if (_ordering.empty()) {
        throw runtime_error("Can not find a start symbol");
    }
    // Augment grammar: add S' -> S where S is the original start symbol.
    // updateOrdering=false: S' is only for LR parsing, not part of the grammar output.
    const auto& startSymbol = _ordering[0];
    const auto newStartSymbol = generatePrimedSymbol(startSymbol, false);

    auto automaton = make_unique<FiniteAutomaton>();

    // Initial state: I₀ with kernel {S' -> · S}
    FiniteAutomatonNode initialNode;
    initialNode.label = automaton->newNodeLabel();
    initialNode.kernel.addProduction(newStartSymbol, {DOT_SYMBOL, startSymbol});
    initialNode.nonKernel = computeClosure(initialNode.kernel);
    automaton->addNode(initialNode);

    // BFS: process each state to find transitions.
    // Note: automaton->size() grows as new states are added.
    for (size_t u = 0; u < automaton->size(); ++u) {
        auto& node = automaton->nodeAt(u);
        auto grammar = node.kernel | node.nonKernel;

        // Collect all symbols that appear immediately after the dot.
        // These are the possible transitions from this state.
        unordered_set<Symbol> transitionSymbolSet;
        vector<Symbol> transitionSymbols;
        for (const auto& head : grammar._ordering) {
            const auto& productions = grammar._productions.at(head);
            for (const auto& production : productions) {
                for (size_t i = 0; i < production.size(); ++i) {
                    if (production[i] == DOT_SYMBOL) {
                        if (i + 1 == production.size()) {
                            // Dot at end: A -> α · (reduce item)
                            // Mark accept state if this is S' -> S ·
                            if (head == newStartSymbol) {
                                node.accept = true;
                            }
                        } else {
                            // Dot before symbol: A -> α · X β
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

        // GOTO(I, X): advance dot over X for all items A -> α · X β
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
                                // A -> α · X β becomes A -> α X · β
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
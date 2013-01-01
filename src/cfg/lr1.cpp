#include "cfg.h"
#include "automaton.h"
#include <unordered_set>
#include <ranges>
#include <algorithm>

using namespace std;

/**
 * Compute FIRST set for a sequence of symbols followed by a lookahead.
 * Used for LR(1) closure to determine lookaheads for new items.
 *
 * @param symbols The sequence β in [A -> α · B β, a]
 * @param lookahead The lookahead 'a'
 * @param firstFollow Precomputed FIRST/FOLLOW sets
 * @return Set of terminals that can appear first in βa
 */
static unordered_set<Symbol> computeFirstOfSequence(const vector<Symbol>& symbols, const Symbol& lookahead, const FirstAndFollowSet& firstFollow) {
    unordered_set<Symbol> result;
    bool allNullable = true;

    for (const auto& symbol : symbols) {
        if (firstFollow.first.contains(symbol)) {
            for (const auto& s : firstFollow.first.at(symbol)) {
                if (s != ContextFreeGrammar::EMPTY_SYMBOL) {
                    result.insert(s);
                }
            }
            if (!firstFollow.first.at(symbol).contains(ContextFreeGrammar::EMPTY_SYMBOL)) {
                allNullable = false;
                break;
            }
        } else {
            result.insert(symbol);
            allNullable = false;
            break;
        }
    }
    if (allNullable) {
        result.insert(lookahead);
    }
    return result;
}

/**
 * Extract all lookaheads from an LR(1) item production.
 * LR(1) items are encoded as: A -> α · β ﹐ lookahead1／lookahead2／...
 * Multiple lookaheads are separated by LOOKAHEAD_INNER_SEPARATOR.
 *
 * @param production
 * @return Set of lookahead symbols
 */
static unordered_set<Symbol> extractLookaheads(const Production& production) {
    unordered_set<Symbol> result;
    for (size_t i = production.size(); i > 0; --i) {
        if (production[i - 1] == ContextFreeGrammar::LOOKAHEAD_SEPARATOR) {
            for (size_t j = i; j < production.size(); ++j) {
                if (production[j] != ContextFreeGrammar::LOOKAHEAD_INNER_SEPARATOR) {
                    result.insert(production[j]);
                }
            }
            break;
        }
    }
    if (result.empty()) {
        result.insert(ContextFreeGrammar::EOF_SYMBOL);
    }
    return result;
}

/**
 * Extract the core production (without lookahead) from an LR(1) item.
 * Returns everything before the ﹐ separator.
 */
static Production extractCore(const Production& production) {
    Production core;
    for (const auto& symbol : production) {
        if (symbol == ContextFreeGrammar::LOOKAHEAD_SEPARATOR) {
            break;
        }
        core.push_back(symbol);
    }
    return core;
}

/**
 * Create an LR(1) item production with multiple lookaheads.
 * Encodes [A -> α · β, {a, b, c}] as α · β ﹐ a／b／c
 * Lookaheads are sorted for deterministic output.
 */
static Production createLR1Production(const Production& core, const unordered_set<Symbol>& lookaheads) {
    Production result = core;
    result.push_back(ContextFreeGrammar::LOOKAHEAD_SEPARATOR);
    vector sortedLookaheads(lookaheads.begin(), lookaheads.end());
    ranges::sort(sortedLookaheads);
    for (size_t i = 0; i < sortedLookaheads.size(); ++i) {
        if (i > 0) {
            result.push_back(ContextFreeGrammar::LOOKAHEAD_INNER_SEPARATOR);
        }
        result.push_back(sortedLookaheads[i]);
    }
    return result;
}

/**
 * Create an LR(1) item production with a single lookahead.
 */
static Production createLR1Production(const Production& core, const Symbol& lookahead) {
    return createLR1Production(core, unordered_set{lookahead});
}

/**
 * Compute a key for the core part (without lookahead) of an LR(1) production.
 */
static string computeCoreKey(const Symbol& head, const Production& production) {
    string key = head + " ->";
    for (const auto& symbol : production) {
        if (symbol == ContextFreeGrammar::LOOKAHEAD_SEPARATOR) {
            break;
        }
        key += " " + symbol;
    }
    return key;
}

/**
 * Build LR(1) automaton (canonical collection of LR(1) item sets).
 *
 * LR(1) items are [A -> α · β, a] where 'a' is a lookahead terminal.
 * Items are encoded as: A -> α · β ﹐ a
 *
 * The closure operation for LR(1):
 * For each item [A -> α · B β, a], add [B -> · γ, b] for each production B -> γ
 * where b is in FIRST(βa).
 *
 * This produces more states than LR(0) but handles more grammars without conflicts.
 */
unique_ptr<FiniteAutomaton> ContextFreeGrammar::computeLR1Automaton() {
    if (_ordering.empty()) {
        throw runtime_error("Can not find a start symbol");
    }

    // Augment grammar: add S' -> S
    const auto& startSymbol = _ordering[0];
    const auto newStartSymbol = generatePrimedSymbol(startSymbol, false);

    // Precompute FIRST/FOLLOW sets for closure computation
    const auto firstFollow = computeFirstAndFollowSet();

    auto automaton = make_unique<FiniteAutomaton>();

    // Merge productions with the same core by combining their lookaheads
    auto mergeLookaheads = [](const ContextFreeGrammar& grammar) -> ContextFreeGrammar {
        unordered_map<string, unordered_set<Symbol>> coreLookaheads;
        unordered_map<string, pair<Symbol, Production>> coreToHeadAndCore;
        vector<string> coreOrdering;

        for (const auto& head : grammar._ordering) {
            for (const auto& production : grammar._productions.at(head)) {
                const string coreKey = computeCoreKey(head, production);
                if (!coreLookaheads.contains(coreKey)) {
                    coreLookaheads[coreKey] = {};
                    coreToHeadAndCore[coreKey] = {head, extractCore(production)};
                    coreOrdering.push_back(coreKey);
                }
                for (const auto& la : extractLookaheads(production)) {
                    coreLookaheads[coreKey].insert(la);
                }
            }
        }

        ContextFreeGrammar merged;
        for (const auto& coreKey : coreOrdering) {
            const auto& [head, core] = coreToHeadAndCore[coreKey];
            const auto& lookaheads = coreLookaheads[coreKey];
            merged.addProduction(head, createLR1Production(core, lookaheads));
        }
        return merged;
    };

    // Initial state: I₀ with kernel {[S' -> · S, ¥]}
    FiniteAutomatonNode initialNode;
    initialNode.label = automaton->newNodeLabel();
    initialNode.kernel.addProduction(newStartSymbol, createLR1Production({DOT_SYMBOL, startSymbol}, EOF_SYMBOL));

    auto computeLR1Closure = [&](const ContextFreeGrammar& kernel) -> ContextFreeGrammar {
        ContextFreeGrammar closure;
        unordered_set<string> addedItems;

        vector<pair<Symbol, Production>> worklist;
        for (const auto& head : kernel._ordering) {
            for (const auto& prod : kernel._productions.at(head)) {
                worklist.emplace_back(head, prod);
                addedItems.insert(computeProductionKey(head, prod));
            }
        }

        while (!worklist.empty()) {
            auto [head, production] = worklist.back();
            worklist.pop_back();

            // Find the dot position and symbol after dot
            for (size_t i = 0; i < production.size(); ++i) {
                if (production[i] == DOT_SYMBOL) {
                    if (i + 1 < production.size() && production[i + 1] != LOOKAHEAD_SEPARATOR) {
                        const auto& symbolAfterDot = production[i + 1];

                        // If symbol after dot is a non-terminal, expand it
                        if (isNonTerminal(symbolAfterDot)) {
                            // Extract β (symbols after B) and lookaheads
                            vector<Symbol> beta;
                            for (size_t j = i + 2; j < production.size() && production[j] != LOOKAHEAD_SEPARATOR; ++j) {
                                beta.push_back(production[j]);
                            }
                            auto lookaheads = extractLookaheads(production);

                            // Compute FIRST(βa) for each lookahead
                            unordered_set<Symbol> firstSet;
                            for (const auto& lookahead : lookaheads) {
                                auto fs = computeFirstOfSequence(beta, lookahead, firstFollow);
                                firstSet.insert(fs.begin(), fs.end());
                            }

                            // Add [B -> · γ, b] for each B -> γ and b in FIRST(βa)
                            if (_productions.contains(symbolAfterDot)) {
                                for (const auto& gamma : _productions.at(symbolAfterDot)) {
                                    Production newCore = {DOT_SYMBOL};
                                    for (const auto& s : gamma) {
                                        newCore.push_back(s);
                                    }
                                    for (const auto& b : firstSet) {
                                        auto newProd = createLR1Production(newCore, b);
                                        if (auto key = computeProductionKey(symbolAfterDot, newProd); !addedItems.contains(key)) {
                                            addedItems.insert(key);
                                            closure.addProduction(symbolAfterDot, newProd);
                                            worklist.emplace_back(symbolAfterDot, newProd);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }

        return mergeLookaheads(closure);
    };

    initialNode.nonKernel = computeLR1Closure(initialNode.kernel);
    automaton->addNode(initialNode);

    // BFS: process each state to find transitions
    for (size_t u = 0; u < automaton->size(); ++u) {
        auto& node = automaton->nodeAt(u);
        auto grammar = node.kernel | node.nonKernel;

        // Collect all symbols that appear immediately after the dot
        unordered_set<Symbol> transitionSymbolSet;
        vector<Symbol> transitionSymbols;
        for (const auto& head : grammar._ordering) {
            const auto& productions = grammar._productions.at(head);
            for (const auto& production : productions) {
                for (size_t i = 0; i < production.size(); ++i) {
                    if (production[i] == DOT_SYMBOL) {
                        if (i + 1 < production.size() && production[i + 1] != LOOKAHEAD_SEPARATOR) {
                            const auto& symbol = production[i + 1];
                            if (!transitionSymbolSet.contains(symbol)) {
                                transitionSymbolSet.insert(symbol);
                                transitionSymbols.emplace_back(symbol);
                            }
                        } else if (i + 1 == production.size() || production[i + 1] == LOOKAHEAD_SEPARATOR) {
                            // Dot at end (before lookahead): reduce item
                            if (head == newStartSymbol) {
                                node.accept = true;
                            }
                        }
                        break;
                    }
                }
            }
        }

        // GOTO(I, X): advance dot over X
        for (const auto& transitionSymbol : transitionSymbols) {
            FiniteAutomatonNode newNode;
            newNode.label = automaton->newNodeLabel();
            for (const auto& head : grammar._ordering) {
                const auto& productions = grammar._productions.at(head);
                for (const auto& production : productions) {
                    for (size_t i = 0; i < production.size(); ++i) {
                        if (production[i] == DOT_SYMBOL) {
                            if (i + 1 < production.size() && production[i + 1] == transitionSymbol) {
                                auto newProduction = production;
                                swap(newProduction[i], newProduction[i + 1]);
                                newNode.kernel.addProduction(head, newProduction);
                            }
                            break;
                        }
                    }
                }
            }
            newNode.kernel = mergeLookaheads(newNode.kernel);
            newNode.nonKernel = computeLR1Closure(newNode.kernel);
            const size_t v = automaton->addNode(newNode);
            automaton->addEdge(u, v, transitionSymbol);
        }
    }

    return automaton;
}

/**
 * Build LR(1) ACTION/GOTO table from the LR(1) automaton.
 *
 * The table has two parts:
 * - ACTION[state, terminal]: shift/reduce/accept actions
 * - GOTO[state, non-terminal]: next state after reduction
 *
 * LR(1) uses the lookahead stored in each item for reduce actions:
 * - For item [A -> α ·, a], add reduce A -> α only for terminal 'a'
 *
 * This is more precise than SLR(1) which uses FOLLOW sets.
 */
ActionGotoTable ContextFreeGrammar::computeLR1ActionGotoTable(const unique_ptr<FiniteAutomaton>& automaton) const {
    ActionGotoTable actionGotoTable(automaton->size());

    // Fill shift actions and GOTO entries from automaton edges
    for (const auto& [u, v, label] : automaton->edges()) {
        if (isTerminal(label)) {
            actionGotoTable.addShift(u, label, v);
        } else {
            actionGotoTable.addGoto(u, label, v);
        }
    }

    // Fill reduce actions and accept from automaton states
    for (size_t u = 0; u < automaton->size(); ++u) {
        if (const auto& node = automaton->nodeAt(u); node.accept) {
            actionGotoTable.actions[u][EOF_SYMBOL].emplace_back("accept");
        } else {
            // Find completed items [A -> α ·, a] and add reduce actions
            const auto findReduce = [&](const ContextFreeGrammar& grammar) {
                for (const auto& head : grammar._ordering) {
                    const auto& productions = grammar._productions.at(head);
                    for (const auto& production : productions) {
                        Production core = extractCore(production);
                        if (!core.empty() && core.back() == DOT_SYMBOL) {
                            // Extract lookaheads and add reduce action for each
                            auto lookaheads = extractLookaheads(production);
                            for (const auto& lookahead : lookaheads) {
                                actionGotoTable.addReduce(u, lookahead, head, core);
                            }
                        }
                    }
                }
            };
            findReduce(node.kernel);
            findReduce(node.nonKernel);
        }
    }

    return actionGotoTable;
}

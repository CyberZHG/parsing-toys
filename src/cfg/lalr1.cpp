#include "cfg.h"
#include "automaton.h"
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

using namespace std;

/**
 * Extract all lookaheads from an LR(1) item production.
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
 */
static Production createLR1Production(const Production& core, const unordered_set<Symbol>& lookaheads) {
    Production result = core;
    result.push_back(ContextFreeGrammar::LOOKAHEAD_SEPARATOR);
    vector<Symbol> sortedLookaheads(lookaheads.begin(), lookaheads.end());
    sort(sortedLookaheads.begin(), sortedLookaheads.end());
    for (size_t i = 0; i < sortedLookaheads.size(); ++i) {
        if (i > 0) {
            result.push_back(ContextFreeGrammar::LOOKAHEAD_INNER_SEPARATOR);
        }
        result.push_back(sortedLookaheads[i]);
    }
    return result;
}

/**
 * Compute a key for the core part (without lookahead) of an LR(1) production.
 */
static string computeItemCoreKey(const Symbol& head, const Production& production) {
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
 * Build LALR(1) automaton by merging LR(1) states with identical cores.
 *
 * LALR(1) is constructed by:
 * 1. Building the full LR(1) automaton
 * 2. Identifying states with identical cores (same items ignoring lookaheads)
 * 3. Merging those states by combining their lookahead sets
 *
 * This produces fewer states than LR(1) but may introduce reduce-reduce
 * conflicts that weren't present in the LR(1) parser.
 */
unique_ptr<FiniteAutomaton> ContextFreeGrammar::computeLALR1Automaton() {
    // First, build the full LR(1) automaton
    const auto lr1Automaton = computeLR1Automaton();

    auto removeLookaheads = [&](const ContextFreeGrammar& grammar) {
        ContextFreeGrammar newGrammar;
        for (const auto& [head, productions] : grammar._productions) {
            for (const auto& production : productions) {
                int separatorIndex = -1;
                for (size_t i = 0; i < production.size(); ++i) {
                    if (production[i] == LOOKAHEAD_SEPARATOR) {
                        separatorIndex = static_cast<int>(i);
                        break;
                    }
                }
                newGrammar.addProduction(head, {production.begin(), production.begin() + separatorIndex});
            }
        }
        newGrammar.deduplicate();
        return newGrammar;
    };

    // Merge productions with the same core by combining their lookaheads
    auto mergeLookaheads = [](const ContextFreeGrammar& grammar) -> ContextFreeGrammar {
        unordered_map<string, unordered_set<Symbol>> coreLookaheads;
        unordered_map<string, pair<Symbol, Production>> coreToHeadAndCore;
        vector<string> coreOrdering;

        for (const auto& head : grammar._ordering) {
            for (const auto& production : grammar._productions.at(head)) {
                const string coreKey = computeItemCoreKey(head, production);
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

    auto computeCoreKey = [&](const ContextFreeGrammar& kernel, const ContextFreeGrammar& nonKernel) -> string {
        const string kernelStr = removeLookaheads(kernel).toSortedString();
        const string nonKernelStr = removeLookaheads(nonKernel).toSortedString();
        return kernelStr + "###\n" + nonKernelStr;
    };

    unordered_map<string, size_t> coreKeyToMergedIndex;
    vector<size_t> lr1ToMerged(lr1Automaton->size());

    auto lalrAutomaton = make_unique<FiniteAutomaton>();

    // First pass: identify unique cores and create merged states
    for (size_t i = 0; i < lr1Automaton->size(); ++i) {
        const auto& node = lr1Automaton->nodeAt(i);
        const string coreKey = computeCoreKey(node.kernel, node.nonKernel);
        if (coreKeyToMergedIndex.contains(coreKey)) {
            lr1ToMerged[i] = coreKeyToMergedIndex[coreKey];
            // Merge lookaheads into existing state using operator|
            auto& mergedNode = lalrAutomaton->nodeAt(lr1ToMerged[i]);
            mergedNode.kernel = mergeLookaheads(mergedNode.kernel | node.kernel);
            mergedNode.nonKernel = mergeLookaheads(mergedNode.nonKernel | node.nonKernel);
            mergedNode.accept = mergedNode.accept || node.accept;
        } else {
            // New core, create a new merged state
            FiniteAutomatonNode newNode;
            newNode.label = lalrAutomaton->newNodeLabel();
            newNode.kernel = node.kernel;
            newNode.nonKernel = node.nonKernel;
            newNode.accept = node.accept;

            const size_t mergedIndex = lalrAutomaton->addNode(newNode);
            coreKeyToMergedIndex[coreKey] = mergedIndex;
            lr1ToMerged[i] = mergedIndex;
        }
    }

    // Second pass: add edges with remapped state indices
    unordered_set<string> addedEdges;
    for (const auto& [u, v, label] : lr1Automaton->edges()) {
        const size_t mergedU = lr1ToMerged[u];
        const size_t mergedV = lr1ToMerged[v];

        // Avoid duplicate edges
        const string edgeKey = to_string(mergedU) + "-" + label + "->" + to_string(mergedV);
        if (!addedEdges.contains(edgeKey)) {
            addedEdges.insert(edgeKey);
            lalrAutomaton->addEdge(mergedU, mergedV, label);
        }
    }

    return lalrAutomaton;
}

/**
 * Build LALR(1) ACTION/GOTO table from the LALR automaton.
 *
 * The table construction is the same as LR(1):
 * - Shift actions from edges on terminals
 * - GOTO entries from edges on non-terminals
 * - Reduce actions based on lookaheads in completed items
 *
 * LALR may have more conflicts than LR(1) due to merged lookaheads.
 */
ActionGotoTable ContextFreeGrammar::computeLALR1ActionGotoTable(const unique_ptr<FiniteAutomaton>& automaton) const {
    return computeLR1ActionGotoTable(automaton);
}

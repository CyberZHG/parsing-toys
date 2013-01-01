#include "cfg.h"
#include "automaton.h"
#include <unordered_set>
#include <unordered_map>

using namespace std;

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

    auto computeCoreKey = [&](const ContextFreeGrammar& kernel, const ContextFreeGrammar& nonKernel) -> string {
        const string kernelStr = kernel.toString();
        const string nonKernelStr = nonKernel.toString();

        auto removeLookaheads = [](const string& s) -> string {
            string result;
            size_t i = 0;
            while (i < s.length()) {
                if (s.substr(i, LOOKAHEAD_SEPARATOR.size()) == LOOKAHEAD_SEPARATOR) {
                    while (i < s.length() && s[i] != '\n') {
                        ++i;
                    }
                } else {
                    result += s[i];
                    ++i;
                }
            }
            return result;
        };

        return removeLookaheads(kernelStr) + "###" + removeLookaheads(nonKernelStr);
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
            mergedNode.kernel = mergedNode.kernel | node.kernel;
            mergedNode.nonKernel = mergedNode.nonKernel | node.nonKernel;
            mergedNode.kernel.deduplicate();
            mergedNode.nonKernel.deduplicate();
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

#include "cfg.h"
#include "automaton.h"
#include <unordered_set>

using namespace std;

/**
 * Build SLR(1) automaton.
 *
 * SLR(1) uses the same automaton as LR(0) - the canonical collection
 * of LR(0) item sets. The difference is only in how the ACTION table
 * is constructed (using FOLLOW sets for reduce actions).
 */
unique_ptr<FiniteAutomaton> ContextFreeGrammar::computeSLR1Automaton() {
    return computeLR0Automaton();
}

/**
 * Build SLR(1) ACTION/GOTO table from the automaton.
 *
 * The table has two parts:
 * - ACTION[state, terminal]: shift/reduce/accept actions
 * - GOTO[state, non-terminal]: next state after reduction
 *
 * SLR(1) improves on LR(0) by using 1-symbol lookahead via FOLLOW sets:
 * - LR(0): reduce A -> α on ALL terminals (causes many conflicts)
 * - SLR(1): reduce A -> α only on terminals in FOLLOW(A)
 *
 * This eliminates some conflicts but not all. Grammars that are not
 * SLR(1) may still have shift-reduce or reduce-reduce conflicts.
 */
ActionGotoTable ContextFreeGrammar::computeSLR1ActionGotoTable(const unique_ptr<FiniteAutomaton>& automaton) const {
    ActionGotoTable actionGotoTable(automaton->size());

    // Fill shift actions and GOTO entries from automaton edges.
    // Edge (u, v, X): if X is terminal -> ACTION[u,X] = shift v
    //                 if X is non-terminal -> GOTO[u,X] = v
    for (const auto& [u, v, label] : automaton->edges()) {
        if (isTerminal(label)) {
            actionGotoTable.addShift(u, label, v);
        } else {
            actionGotoTable.addGoto(u, label, v);
        }
    }

    // Precompute FIRST and FOLLOW sets for lookahead decisions.
    const auto firstFollowSet = computeFirstAndFollowSet();

    // Fill reduce actions and accept from automaton states.
    for (size_t u = 0; u < automaton->size(); ++u) {
        if (const auto& node = automaton->nodeAt(u); node.accept) {
            actionGotoTable.actions[u][EOF_SYMBOL].emplace_back("accept");
        } else {
            // Find completed items (A -> α ·) and add reduce actions.
            // SLR(1) uses lookahead: reduce only on FOLLOW(A).
            const auto findReduce = [&](const ContextFreeGrammar& grammar) {
                for (const auto& head : grammar._ordering) {
                    const auto& productions = grammar._productions.at(head);
                    for (const auto& production : productions) {
                        if (production.back() == DOT_SYMBOL) {
                            // Add reduce only to terminals in FOLLOW(head)
                            for (const auto& symbol : firstFollowSet.follow.at(head)) {
                                actionGotoTable.addReduce(u, symbol, head, production);
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

#include "cfg.h"
#include "automaton.h"
#include <unordered_set>

using namespace std;

unique_ptr<FiniteAutomaton> ContextFreeGrammar::computeSLR1Automaton() {
    return computeLR0Automaton();
}

ActionGotoTable ContextFreeGrammar::computeSLR1ActionGotoTable(const unique_ptr<FiniteAutomaton>& automaton) const {
    ActionGotoTable actionGotoTable(automaton->size());

    for (const auto& [u, v, label] : automaton->edges()) {
        if (isTerminal(label)) {
            actionGotoTable.addShift(u, label, v);
        } else {
            actionGotoTable.addGoto(u, label, v);
        }
    }

    const auto firstFollowSet = computeFirstAndFollowSet();
    for (size_t u = 0; u < automaton->size(); ++u) {
        if (const auto& node = automaton->nodeAt(u); node.accept) {
            actionGotoTable.actions[u][EOF_SYMBOL].emplace_back("accept");
        } else {
            const auto findReduce = [&](const ContextFreeGrammar& grammar) {
                for (const auto& head : grammar._ordering) {
                    const auto& productions = grammar._productions.at(head);
                    for (const auto& production : productions) {
                        if (production.back() == DOT_SYMBOL) {
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

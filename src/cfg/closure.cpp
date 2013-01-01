#include "cfg.h"

using namespace std;

/** Closure computation for LR parsing.
 * Given kernel items (e.g., S' -> · S), adds all items reachable by expanding non-terminals.
 *
 * Algorithm: For each item A -> α · B β where B is non-terminal,
 * add B -> · γ for all productions B -> γ.
 * Repeat until no new items are added.
 */
ContextFreeGrammar ContextFreeGrammar::computeClosure(const ContextFreeGrammar& kernel) const {
    ContextFreeGrammar nonKernel;
    unordered_set<string> nonKernelKeys;
    // Collect all items to process
    Productions kernelProductions;
    for (const auto& [head, productions]: kernel._productions) {
        for (const auto& production : productions) {
            nonKernelKeys.insert(computeProductionKey(head, production));
            kernelProductions.emplace_back(production);
        }
    }

    // Expand until no new items.
    bool hasUpdate = true;
    while (hasUpdate) {
        hasUpdate = false;
        for (size_t productionIndex = 0; productionIndex < kernelProductions.size(); ++productionIndex) {
            const auto& production = kernelProductions[productionIndex];
            for (size_t i = 0; i < production.size(); ++i) {
                if (production[i] == DOT_SYMBOL && i + 1 < production.size()) {
                    // Found A -> α · B β, expand B
                    if (const auto& symbol = production[i + 1]; isNonTerminal(symbol)) {
                        for (const auto& expandProduction : _productions.at(symbol)) {
                            Production newProduction = {DOT_SYMBOL};
                            if (!(expandProduction.size() == 1 && expandProduction[0] == EMPTY_SYMBOL)) {
                                // B -> γ becomes B -> · γ
                                // B -> ε becomes B -> · (empty after dot)
                                newProduction.insert(newProduction.end(), expandProduction.begin(), expandProduction.end());
                            }
                            if (const auto key = computeProductionKey(symbol, newProduction); !nonKernelKeys.contains(key)) {
                                nonKernelKeys.insert(key);
                                nonKernel.addProduction(symbol, newProduction);
                                kernelProductions.emplace_back(newProduction);
                                hasUpdate = true;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    return nonKernel;
}

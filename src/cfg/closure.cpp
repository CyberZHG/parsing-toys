#include "cfg.h"
#include <ranges>

ContextFreeGrammar ContextFreeGrammar::computeClosure(const ContextFreeGrammar& kernel) const {
    ContextFreeGrammar nonKernel;
    unordered_set<string> nonKernelKeys;
    auto kernelProductions = kernel._productions;
    for (const auto& [head, productions]: kernelProductions) {
        for (const auto& production : productions) {
            nonKernelKeys.insert(computeProductionKey(head, production));
        }
    }

    bool hasUpdate = true;
    while (hasUpdate) {
        hasUpdate = false;
        for (const auto &productions: kernelProductions | views::values) {
            for (const auto& production : productions) {
                for (size_t i = 0; i < production.size(); ++i) {
                    if (production[i] == DOT_SYMBOL && i + 1 < production.size()) {
                        // Found A -> α · B β
                        if (const auto& symbol = production[i + 1]; isNonTerminal(symbol)) {
                            for (const auto& expandProduction : _productions.at(symbol)) {
                                Production newProduction = {DOT_SYMBOL};
                                if (!(expandProduction.size() == 1 && expandProduction[0] == EMPTY_SYMBOL)) {
                                    // Add B -> · γ to non-kernel
                                    // Only add B -> · to non-kernel if B -> ε
                                    newProduction.insert(newProduction.end(), expandProduction.begin(), expandProduction.end());
                                }
                                if (const auto key = computeProductionKey(symbol, newProduction); !nonKernelKeys.contains(key)) {
                                    nonKernelKeys.insert(key);
                                    nonKernel.addProduction(symbol, newProduction);
                                    kernelProductions[symbol].emplace_back(newProduction);
                                    hasUpdate = true;
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    return nonKernel;
}

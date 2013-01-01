#include "cfg.h"
#include <format>

bool ContextFreeGrammar::leftRecursionElimination() {
    bool eliminable = true;
    unordered_set<Symbol> eliminated;
    for (auto head : _ordering) {
        const auto& productions = _productions[head];
        // Find indirect recursions.
        Productions recursiveProductions, nonRecursiveProductions;
        unordered_set<string> productionKeys;
        for (const auto& production : productions) {
            if (!production.empty()) {
                if (eliminated.contains(production[0])) {
                    for (auto newProduction : _productions[production[0]]) {
                        if (newProduction.size() == 1 && newProduction[0] == EMPTY_SYMBOL) {
                            newProduction.resize(0);
                        }
                        newProduction.insert(newProduction.end(), production.begin() + 1, production.end());
                        if (const auto key = computeProductionKey(newProduction); !productionKeys.contains(key)) {
                            productionKeys.insert(key);
                            if (!newProduction.empty() && newProduction[0] == head) {
                                recursiveProductions.emplace_back(newProduction);
                            } else {
                                nonRecursiveProductions.emplace_back(newProduction);
                            }
                        }
                    }
                } else if (production[0] == head) {
                    if (const auto key = computeProductionKey(production); !productionKeys.contains(key)) {
                        productionKeys.insert(key);
                        recursiveProductions.emplace_back(production);
                    }
                } else {
                    if (production.size() == 1 && production[0] == EMPTY_SYMBOL) {
                        if (!productionKeys.contains("")) {
                            productionKeys.insert("");
                            nonRecursiveProductions.emplace_back();
                        }
                    } else {
                        if (const auto key = computeProductionKey(production); !productionKeys.contains(key)) {
                            nonRecursiveProductions.emplace_back(production);
                        }
                    }
                }
            }
        }
        eliminated.insert(head);
        if (recursiveProductions.empty()) {
            continue;
        }
        // Try to eliminate left recursions.
        if (!recursiveProductions.empty() && nonRecursiveProductions.empty()) {
            eliminable = false;
            _errorMessage = format("Left recursion cannot be eliminated for {}.", head);
            break;
        }
        const auto primedSymbol = generatePrimedSymbol(head);
        for (auto& production : nonRecursiveProductions) {
            production.emplace_back(primedSymbol);
        }
        for (auto& production : recursiveProductions) {
            for (size_t i = 0; i + 1 < production.size(); ++i) {
                production[i] = production[i + 1];
            }
            production[production.size() - 1] = primedSymbol;
        }
        recursiveProductions.emplace_back(vector{EMPTY_SYMBOL});
        _terminals.insert(EMPTY_SYMBOL);
        _productions[head] = nonRecursiveProductions;
        _productions[primedSymbol] = recursiveProductions;
    }
    return eliminable;
}

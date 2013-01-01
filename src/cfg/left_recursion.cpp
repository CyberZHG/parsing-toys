#include "cfg.h"
#include <format>
#include <functional>

using namespace std;

bool ContextFreeGrammar::leftRecursionElimination() {
    bool eliminable = true;
    unordered_set<Symbol> eliminated;
    for (auto head : _ordering) {
        const auto& productions = _productions[head];
        // Find indirect recursions.
        Productions recursiveProductions, nonRecursiveProductions;
        unordered_set<string> productionKeys;
        const auto addToRecursiveSet = [&](const Production& production) {
            if (const auto key = computeProductionKey(production); !productionKeys.contains(key)) {
                productionKeys.insert(key);
                recursiveProductions.emplace_back(production);
            }
        };
        const auto addToNonRecursiveSet = [&](const Production& production) {
            if (auto key = computeProductionKey(production); !productionKeys.contains(key)) {
                productionKeys.insert(key);
                nonRecursiveProductions.emplace_back(production);
            }
        };
        const function<void(const Production&)> expand = [&](const Production& production) -> void {
            if (!production.empty()) {
                if (eliminated.contains(production[0])) {
                    for (auto newProduction : _productions[production[0]]) {
                        if (newProduction.size() == 1 && newProduction[0] == EMPTY_SYMBOL) {
                            newProduction.resize(0);
                        }
                        newProduction.insert(newProduction.end(), production.begin() + 1, production.end());
                        if (!newProduction.empty() && newProduction[0] == head) {
                            addToRecursiveSet(newProduction);
                        } else {
                            expand(newProduction);
                        }
                    }
                } else if (production[0] == head) {
                    if (const auto key = computeProductionKey(production); !productionKeys.contains(key)) {
                        productionKeys.insert(key);
                        recursiveProductions.emplace_back(production);
                    }
                } else {
                    if (production.size() == 1 && production[0] == EMPTY_SYMBOL) {
                        addToNonRecursiveSet(Production());
                    } else {
                       addToNonRecursiveSet(production);
                    }
                }
            }
        };
        for (const auto& production : productions) {
            expand(production);
        }
        eliminated.insert(head);
        if (recursiveProductions.empty()) {
            continue;
        }
        // Try to eliminate left recursions.
        if (!recursiveProductions.empty() && nonRecursiveProductions.empty()) {
            eliminable = false;
            _errorMessage = format("Left recursion cannot be eliminated for \"{}\".", head);
            break;
        }
        const auto onlySelfLoop = recursiveProductions.size() == 1
            && recursiveProductions[0].size() == 1
            && recursiveProductions[0][0] == head;
        if (!onlySelfLoop) {
            const auto primedSymbol = generatePrimedSymbol(head);
            for (auto& production : nonRecursiveProductions) {
                production.emplace_back(primedSymbol);
            }
            for (auto& production : recursiveProductions) {
                for (size_t i = 0; i + 1 < production.size(); ++i) {
                    production[i] = production[i + 1];
                }
                production[production.size() - 1] = primedSymbol;
                _productions[primedSymbol].emplace_back(production);
            }
            _productions[primedSymbol].emplace_back(vector{EMPTY_SYMBOL});
            _terminals.insert(EMPTY_SYMBOL);
        }
        _productions[head] = nonRecursiveProductions;
    }
    return eliminable;
}

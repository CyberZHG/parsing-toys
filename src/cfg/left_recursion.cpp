#include "cfg.h"
#include <format>
#include <functional>

using namespace std;

/**
 * Eliminate left recursion using the standard algorithm.
 *
 * For direct left recursion A -> A α | β:
 *   Transform to: A -> β A', A' -> α A' | ε
 *
 * For indirect left recursion (A -> B ..., B -> A ...):
 *   Process non-terminals in order, substituting earlier non-terminals
 *   to convert indirect recursion to direct, then eliminate.
 *
 * @return false if elimination is impossible (all productions are left-recursive).
 */
bool ContextFreeGrammar::leftRecursionElimination() {
    bool eliminable = true;
    unordered_set<Symbol> eliminated;  // Non-terminals already processed

    for (auto head : _ordering) {
        const auto& productions = _productions[head];
        Productions recursiveProductions, nonRecursiveProductions;
        unordered_set<string> productionKeys;  // For deduplication

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

        // Recursively expand productions starting with already-eliminated non-terminals.
        // This converts indirect left recursion to direct.
        const function<void(const Production&)> expand = [&](const Production& production) -> void {
            if (!production.empty()) {
                if (eliminated.contains(production[0])) {
                    // First symbol is an eliminated non-terminal: substitute its productions.
                    for (auto newProduction : _productions[production[0]]) {
                        if (newProduction.size() == 1 && newProduction[0] == EMPTY_SYMBOL) {
                            newProduction.resize(0);  // ε becomes empty
                        }
                        // Append the rest: B γ where B -> δ becomes δ γ
                        newProduction.insert(newProduction.end(), production.begin() + 1, production.end());
                        if (!newProduction.empty() && newProduction[0] == head) {
                            addToRecursiveSet(newProduction);
                        } else {
                            expand(newProduction);
                        }
                    }
                } else if (production[0] == head) {
                    // Direct left recursion: A -> A α
                    if (const auto key = computeProductionKey(production); !productionKeys.contains(key)) {
                        productionKeys.insert(key);
                        recursiveProductions.emplace_back(production);
                    }
                } else {
                    // Non-recursive production
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
            continue;  // No left recursion for this non-terminal
        }

        // Cannot eliminate if ALL productions are left-recursive (no β exists).
        if (!recursiveProductions.empty() && nonRecursiveProductions.empty()) {
            eliminable = false;
            _errorMessage = format("Left recursion cannot be eliminated for \"{}\".", head);
            break;
        }

        // Special case: A -> A (self-loop only) - just remove it
        const auto onlySelfLoop = recursiveProductions.size() == 1
            && recursiveProductions[0].size() == 1
            && recursiveProductions[0][0] == head;

        if (!onlySelfLoop) {
            // Standard transformation:
            // A -> A α₁ | A α₂ | β₁ | β₂
            // becomes:
            // A  -> β₁ A' | β₂ A'
            // A' -> α₁ A' | α₂ A' | ε
            const auto primedSymbol = generatePrimedSymbol(head);

            // β -> β A'
            for (auto& production : nonRecursiveProductions) {
                production.emplace_back(primedSymbol);
            }

            // A α -> α A' (remove leading A, append A')
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

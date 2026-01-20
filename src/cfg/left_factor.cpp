#include "cfg.h"
#include "production_trie.h"
#include <functional>

using namespace std;

void ContextFreeGrammar::leftFactoring(const bool expand) {
    unordered_set<Symbol> primedSymbols;
    const vector<Symbol> originalNonTerminals = _ordering;
    for (int step = 0; step <= static_cast<int>(expand); ++step) {
        bool expandProductions = step > 0;
        bool hasUpdate = true;
        while (hasUpdate) {
            hasUpdate = false;
            for (const auto& head: originalNonTerminals) {
                ProductionTrie trie;
                int expansionIndex = 0;
                auto& productions = _productions[head];
                unordered_set expanded = {head};
                unordered_map<int, int> parents;
                const function<int(const Production&, size_t, int)> expandProduction = [&](const Production& production, size_t index, const int originalProductionIndex) -> int {
                    const int currentExpansionIndex = expansionIndex++;
                    while (index < production.size() &&
                        (primedSymbols.contains(production[index]) || !_productions.contains(production[index]) || expanded.contains(production[index]))) {
                        ++index;
                    }
                    if (index < production.size()) {
                        auto childIndex = expandProduction(production, index + 1, originalProductionIndex);
                        parents[childIndex] = currentExpansionIndex;
                        expanded.insert(production[index]);
                        for (const auto& expandedProduction : _productions[production[index]]) {
                            Production newProduction;
                            newProduction.insert(newProduction.begin(), production.begin(), production.begin() + static_cast<int>(index));
                            newProduction.insert(newProduction.end(), expandedProduction.begin(), expandedProduction.end());
                            newProduction.insert(newProduction.end(), production.begin() + static_cast<int>(index) + 1, production.end());
                            childIndex = expandProduction(newProduction, index, originalProductionIndex);
                            parents[childIndex] = currentExpansionIndex;
                        }
                        expanded.erase(production[index]);
                    } else {
                        trie.insert(production, originalProductionIndex, currentExpansionIndex);
                    }
                    return currentExpansionIndex;
                };
                for (size_t i = 0; i < productions.size(); ++i) {
                    if (expandProductions) {
                        expandProduction(productions[i], 0, static_cast<int>(i));
                    } else {
                        trie.insert(productions[i], static_cast<int>(i));
                    }
                }
                unordered_set<int> toBeRemoved;
                Productions newProductions;
                auto [prefix, node] = trie.findLongestCommonPrefix();
                if (prefix.empty()) {
                    continue;
                }
                if (node->originalIndices.size() > 1) {
                    for (const auto& index : node->originalIndices) {
                        toBeRemoved.insert(index);
                    }
                    const auto suffices = ProductionTrie::computeProductionsUnderPrefix(node, &parents);
                    if (suffices.size() == 1) {
                        if (!(suffices[0].size() == 1 && suffices[0][0] == EMPTY_SYMBOL)) {
                            prefix.insert(prefix.end(), suffices[0].begin(), suffices[0].end());
                        }
                        newProductions.emplace_back(prefix);
                    } else {
                        const auto primedSymbol = generatePrimedSymbol(head);
                        addProductions(primedSymbol, suffices);
                        primedSymbols.insert(primedSymbol);
                        prefix.emplace_back(primedSymbol);
                        newProductions.emplace_back(prefix);
                    }
                    hasUpdate = true;
                }
                size_t m = 0;
                for (size_t i = 0; i < productions.size(); ++i) {
                    if (!toBeRemoved.contains(static_cast<int>(i))) {
                        if (m != i) {
                            productions[m] = productions[i];
                        }
                        ++m;
                    }
                }
                productions.resize(m);
                for (const auto& production : newProductions) {
                    productions.emplace_back(production);
                }
            }
        }
    }
}
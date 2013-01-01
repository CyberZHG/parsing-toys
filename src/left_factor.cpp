#include "cfg.h"
#include "production_trie.h"
#include <functional>

void ContextFreeGrammar::leftFactoring() {
    bool hasUpdate = true;
    unordered_set<Symbol> primedSymbols;
    const vector<Symbol> originalNonTerminals = _ordering;
    while (hasUpdate) {
        hasUpdate = false;
        for (const auto& head: originalNonTerminals) {
            ProductionTrie trie;
            int expansionIndex = 0;
            auto& productions = _productions[head];
            unordered_set expanded = {head};
            unordered_map<int, int> parents;
            const function<int(const Production&, size_t, int)> expand = [&](const Production& production, size_t index, const int originalProductionIndex) -> int {
                const int currentExpansionIndex = expansionIndex++;
                while (index < production.size() &&
                    (primedSymbols.contains(production[index]) || !_productions.contains(production[index]) || expanded.contains(production[index]))) {
                    ++index;
                }
                if (index < production.size()) {
                    auto childIndex = expand(production, index + 1, originalProductionIndex);
                    parents[childIndex] = currentExpansionIndex;
                    expanded.insert(production[index]);
                    for (const auto& expandProduction : _productions[production[index]]) {
                        Production newProduction;
                        newProduction.insert(newProduction.begin(), production.begin(), production.begin() + static_cast<int>(index));
                        newProduction.insert(newProduction.end(), expandProduction.begin(), expandProduction.end());
                        newProduction.insert(newProduction.end(), production.begin() + static_cast<int>(index) + 1, production.end());
                        childIndex = expand(newProduction, index, originalProductionIndex);
                        parents[childIndex] = currentExpansionIndex;
                    }
                    expanded.erase(production[index]);
                } else {
                    trie.insert(production, originalProductionIndex, currentExpansionIndex);
                }
                return currentExpansionIndex;
            };
            for (size_t i = 0; i < productions.size(); ++i) {
                expand(productions[i], 0, static_cast<int>(i));
            }
            unordered_set<int> toBeRemoved;
            Productions newProductions;
            auto [prefix, node] = trie.findLongestCommonPrefix();
            if (prefix.empty()) {
                break;
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

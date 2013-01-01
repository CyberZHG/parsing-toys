#include "cfg.h"
#include "production_trie.h"

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
            for (size_t i = 0; i < productions.size(); ++i) {
                trie.insert(productions[i], static_cast<int>(i), expansionIndex++);
            }
            unordered_set<int> toBeRemoved;
            Productions newProductions;
            while (true) {
                auto [prefix, node] = trie.findLongestCommonPrefix();
                if (prefix.empty()) {
                    break;
                }
                for (const auto& index : node->originalIndices) {
                    toBeRemoved.insert(index);
                }
                const auto primedSymbol = generatePrimedSymbol(head);
                addProductions(primedSymbol, ProductionTrie::computeProductionsUnderPrefix(node));
                primedSymbols.insert(primedSymbol);
                prefix.emplace_back(primedSymbol);
                newProductions.emplace_back(prefix);
                ProductionTrie::removeNode(node);
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

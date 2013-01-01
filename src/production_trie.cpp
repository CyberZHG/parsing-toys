#include "production_trie.h"
#include "string_utils.h"
#include "cfg.h"
#include <functional>
#include <ranges>
#include <algorithm>

ProductionTrie::ProductionTrie() {
    _head = make_shared<ProductionTrieNode>();
}

void ProductionTrie::insert(const vector<string>& production, const int index) const {
    auto current = _head;
    ++current->count;
    current->indices.emplace_back(index);
    for (const auto& symbol: production) {
        if (!current->children.contains(symbol)) {
            current->children[symbol] = make_shared<ProductionTrieNode>();
        }
        const auto parent = current.get();
        current = current->children[symbol];
        ++current->count;
        current->indices.emplace_back(index);
        current->parent = parent;
    }
    current->productionEnd = true;
}

pair<vector<string>, shared_ptr<ProductionTrieNode>> ProductionTrie::findLongestCommonPrefix() const {
    shared_ptr<ProductionTrieNode> bestNode = _head;
    vector<string> prefix, longestPrefix;
    const function<void(const shared_ptr<ProductionTrieNode>&)> search = [&](const shared_ptr<ProductionTrieNode>& node) -> void {
        if (node->count >= 2) {
            if (prefix.size() > longestPrefix.size() ||
                (prefix.size() == longestPrefix.size() && node->count > bestNode->count) ||
                (prefix.size() == longestPrefix.size() && node->count == bestNode->count && prefix < longestPrefix)) {
                longestPrefix = prefix;
                bestNode = node;
            }
            for (const auto& [symbol, child]: node->children) {
                prefix.emplace_back(symbol);
                search(child);
                prefix.pop_back();
            }
        }
    };
    search(_head);
    return {longestPrefix, bestNode};
}

vector<vector<string>> ProductionTrie::computeProductionsUnderPrefix(const shared_ptr<ProductionTrieNode>& node) {
    vector<vector<string>> productions;
    vector<string> production;
    const function<void(const shared_ptr<ProductionTrieNode>&)> search = [&](const shared_ptr<ProductionTrieNode>& _node) -> void {
        if (_node->productionEnd) {
            if (production.empty()) {
                productions.emplace_back(vector{ContextFreeGrammar::EMPTY_SYMBOL});
            } else {
                productions.emplace_back(production);
            }
        }
        for (const auto& [symbol, child]: _node->children) {
            production.emplace_back(symbol);
            search(child);
            production.pop_back();
        }
    };
    search(node);
    ranges::sort(productions);
    return productions;
}

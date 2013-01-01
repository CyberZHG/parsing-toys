#include "production_trie.h"
#include "string_utils.h"
#include "cfg.h"
#include <functional>
#include <ranges>
#include <algorithm>

using namespace std;

ProductionTrie::ProductionTrie() {
    _head = make_shared<ProductionTrieNode>();
}

void ProductionTrie::insert(const vector<string>& production, const int originalIndex, const int expansionIndex) const {
    auto current = _head;
    ++current->count;
    current->originalIndices.insert(originalIndex);
    for (const auto& symbol: production) {
        if (!current->children.contains(symbol)) {
            current->children[symbol] = make_shared<ProductionTrieNode>();
        }
        const auto parent = current.get();
        current = current->children[symbol];
        ++current->count;
        current->originalIndices.insert(originalIndex);
        current->parent = parent;
    }
    current->expansionIndices.insert(expansionIndex);
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

vector<vector<string>> ProductionTrie::computeProductionsUnderPrefix(const shared_ptr<ProductionTrieNode>& node, const unordered_map<int ,int>* parents) {
    vector<vector<string>> productions;
    unordered_set<int> allExpansionIndices;
    vector<unordered_set<int>> expansionIndices;
    vector<string> production;
    const function<void(const shared_ptr<ProductionTrieNode>&)> search = [&](const shared_ptr<ProductionTrieNode>& _node) -> void {
        if (!_node->expansionIndices.empty()) {
            if (production.empty()) {
                productions.emplace_back(vector{ContextFreeGrammar::EMPTY_SYMBOL});
            } else {
                productions.emplace_back(production);
            }
            if (parents != nullptr) {
                expansionIndices.emplace_back(_node->expansionIndices);
                for (const auto index : _node->expansionIndices) {
                    allExpansionIndices.insert(index);
                }
            }
        }
        for (const auto& [symbol, child]: _node->children) {
            production.emplace_back(symbol);
            search(child);
            production.pop_back();
        }
    };
    search(node);
    if (parents != nullptr) {
        // Remove productions whose parent is also in the list.
        size_t m = 0;
        for (size_t i = 0; i < productions.size(); ++i) {
            bool valid = true;
            for (const auto index : expansionIndices[i]) {
                auto it = parents->find(index);
                while (it != parents->end() && it->second != NO_EXPANSION) {
                    const auto parent = it->second;
                    if (!expansionIndices[i].contains(parent) && allExpansionIndices.contains(parent)) {
                        valid = false;
                        break;
                    }
                    it = parents->find(parent);
                }
            }
            if (valid) {
                if (m != i) {
                    productions[m] = productions[i];
                }
                ++m;
            }
        }
        productions.resize(m);
    }
    ranges::sort(productions);
    return productions;
}

void ProductionTrie::removeNode(const shared_ptr<ProductionTrieNode>& node) {
    if (auto parent = node->parent; parent != nullptr) {
        for (const auto& [head, child]: parent->children) {
            if (child.get() == node.get()) {
                parent->children.erase(head);
                break;
            }
        }
        while (parent != nullptr) {
            parent->count -= node->count;
            parent = parent->parent;
        }
    }
}

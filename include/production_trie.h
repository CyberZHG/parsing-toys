#ifndef PARSINGTOYS_SYMBOL_TRIE_H
#define PARSINGTOYS_SYMBOL_TRIE_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
using namespace std;

struct ProductionTrieNode {
    int count = 0;
    ProductionTrieNode *parent = nullptr;
    unordered_set<int> originalIndices;  // Keep track of the indices in the original productions
    unordered_set<int> expansionIndices;
    unordered_map<string, shared_ptr<ProductionTrieNode>> children;
};

class ProductionTrie {
public:
    ProductionTrie();
    ~ProductionTrie() = default;

    static constexpr int NO_EXPANSION = -1;

    /**
     * Insert a production to the trie.
     * @param production
     * @param originalIndex The index before expansion.
     * @param expansionIndex The index after expansion.
     */
    void insert(const vector<string>& production, int originalIndex, int expansionIndex = NO_EXPANSION) const;

    /**
     * Find the longest common prefix in the current trie.
     * If there are multiple prefixes with the same maximum length,
     * choose the one with the highest frequency;
     * if the frequency is still the same, choose the lexicographically smallest one.
     * @return The prefix and corresponding trie node.
     */
    [[nodiscard]] pair<vector<string>, shared_ptr<ProductionTrieNode>> findLongestCommonPrefix() const;

    /**
     * Find all child productions under the current node and sort them in lexicographical order.
     * @param node A trie node.
     * @param parents The parent relation of expansion indices.
     * @return
     */
    static vector<vector<string>> computeProductionsUnderPrefix(const shared_ptr<ProductionTrieNode>& node, const unordered_map<int ,int>* parents = nullptr);

private:
    shared_ptr<ProductionTrieNode> _head;
};

#endif //PARSINGTOYS_SYMBOL_TRIE_H

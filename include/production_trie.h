#ifndef PARSINGTOYS_SYMBOL_TRIE_H
#define PARSINGTOYS_SYMBOL_TRIE_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

struct ProductionTrieNode {
    int count = 0;
    bool productionEnd = false;
    ProductionTrieNode *parent = nullptr;
    vector<int> indices;  // Keep track of the indices in the original productions
    unordered_map<string, shared_ptr<ProductionTrieNode>> children;
};

class ProductionTrie {
public:
    ProductionTrie();
    ~ProductionTrie() = default;

    /**
     * Insert a production to the trie.
     * @param production
     * @param index The index before expansion.
     */
    void insert(const vector<string>& production, int index) const;

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
     * @return
     */
    static vector<vector<string>> computeProductionsUnderPrefix(const shared_ptr<ProductionTrieNode>& node) ;

private:
    shared_ptr<ProductionTrieNode> _head;
};

#endif //PARSINGTOYS_SYMBOL_TRIE_H

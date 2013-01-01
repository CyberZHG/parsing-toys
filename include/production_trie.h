#ifndef PARSING_TOYS_PRODUCTION_TRIE_H
#define PARSING_TOYS_PRODUCTION_TRIE_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

struct ProductionTrieNode {
    int count = 0;
    ProductionTrieNode *parent = nullptr;
    std::unordered_set<int> originalIndices;  // Keep track of the indices in the original productions
    std::unordered_set<int> expansionIndices;
    std::unordered_map<std::string, std::shared_ptr<ProductionTrieNode>> children;
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
    void insert(const std::vector<std::string>& production, int originalIndex, int expansionIndex = NO_EXPANSION) const;

    /**
     * Find the longest common prefix in the current trie.
     * If there are multiple prefixes with the same maximum length,
     * choose the one with the highest frequency;
     * if the frequency is still the same, choose the lexicographically smallest one.
     * @return The prefix and corresponding trie node.
     */
    [[nodiscard]] std::pair<std::vector<std::string>, std::shared_ptr<ProductionTrieNode>> findLongestCommonPrefix() const;

    /**
     * Find all child productions under the current node and sort them in lexicographical order.
     * @param node A trie node.
     * @param parents The parent relation of expansion indices.
     * @return
     */
    static std::vector<std::vector<std::string>> computeProductionsUnderPrefix(const std::shared_ptr<ProductionTrieNode>& node, const std::unordered_map<int ,int>* parents = nullptr);

    /**
     * Remove a node from the trie.
     * @param node A trie node.
     */
    static void removeNode(const std::shared_ptr<ProductionTrieNode>& node);

private:
    std::shared_ptr<ProductionTrieNode> _head;
};

#endif //PARSING_TOYS_PRODUCTION_TRIE_H

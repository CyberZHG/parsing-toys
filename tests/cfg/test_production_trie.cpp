#include "production_trie.h"
#include "string_utils.h"
#include <gtest/gtest.h>
#include <ranges>
#include <algorithm>

using namespace std;

TEST(TestProductionTrie, LongestCommonPrefixEmpty) {
    const ProductionTrie trie;
    const auto [prefix, node] = trie.findLongestCommonPrefix();
    EXPECT_EQ(0, prefix.size());
}

TEST(TestProductionTrie, LongestCommonPrefixSingleProduction) {
    const ProductionTrie trie;
    trie.insert(vector<string>{"a", "b", "c"}, 0);
    const auto [prefix, node] = trie.findLongestCommonPrefix();
    EXPECT_EQ(0, prefix.size());
}

TEST(TestProductionTrie, LongestCommonPrefixNoOverlap) {
    const ProductionTrie trie;
    trie.insert(vector<string>{"a", "b", "c"}, 0);
    trie.insert(vector<string>{"b", "b", "c"}, 1);
    trie.insert(vector<string>{"c", "b", "c"}, 2);
    const auto [prefix, node] = trie.findLongestCommonPrefix();
    EXPECT_EQ(0, prefix.size());
}

TEST(TestProductionTrie, LongestCommonPrefixCommon1) {
    const ProductionTrie trie;
    trie.insert(vector<string>{"a", "b", "c"}, 0);
    trie.insert(vector<string>{"a", "b", "d"}, 1);
    const auto [prefix, node] = trie.findLongestCommonPrefix();
    ASSERT_EQ(2, prefix.size());
    EXPECT_EQ("a", prefix[0]);
    EXPECT_EQ("b", prefix[1]);
    ASSERT_EQ(2, node->originalIndices.size());
    EXPECT_TRUE(node->originalIndices.contains(0));
    EXPECT_TRUE(node->originalIndices.contains(1));
    const auto productions = ProductionTrie::computeProductionsUnderPrefix(node);
    ASSERT_EQ(2, productions.size());
    EXPECT_EQ(vector<string>{"c"}, productions[0]);
    EXPECT_EQ(vector<string>{"d"}, productions[1]);
}

TEST(TestProductionTrie, LongestCommonPrefixCommon2) {
    const ProductionTrie trie;
    trie.insert(vector<string>{"a", "b", "c"}, 0);
    trie.insert(vector<string>{"a", "b", "c", "d"}, 1);
    trie.insert(vector<string>{"a", "b", "d"}, 2);
    const auto [prefix, node] = trie.findLongestCommonPrefix();
    ASSERT_EQ(3, prefix.size());
    EXPECT_EQ("a", prefix[0]);
    EXPECT_EQ("b", prefix[1]);
    EXPECT_EQ("c", prefix[2]);
    ASSERT_EQ(2, node->originalIndices.size());
    EXPECT_TRUE(node->originalIndices.contains(0));
    EXPECT_TRUE(node->originalIndices.contains(1));
    const auto productions = ProductionTrie::computeProductionsUnderPrefix(node);
    ASSERT_EQ(2, productions.size());
    EXPECT_EQ(vector<string>{"d"}, productions[0]);
    EXPECT_EQ(vector<string>{"ε"}, productions[1]);
}

TEST(TestProductionTrie, LongestCommonPrefixSameCount1) {
    const ProductionTrie trie;
    trie.insert(vector<string>{"a", "b", "c"}, 0);
    trie.insert(vector<string>{"a", "b", "c", "d"}, 1);
    trie.insert(vector<string>{"a", "b", "d"}, 2);
    trie.insert(vector<string>{"a", "b", "d", "e"}, 3);
    const auto [prefix, node] = trie.findLongestCommonPrefix();
    ASSERT_EQ(3, prefix.size());
    EXPECT_EQ("a", prefix[0]);
    EXPECT_EQ("b", prefix[1]);
    EXPECT_EQ("c", prefix[2]);
    ASSERT_EQ(2, node->originalIndices.size());
    EXPECT_TRUE(node->originalIndices.contains(0));
    EXPECT_TRUE(node->originalIndices.contains(1));
    const auto productions = ProductionTrie::computeProductionsUnderPrefix(node);
    ASSERT_EQ(2, productions.size());
    EXPECT_EQ(vector<string>{"d"}, productions[0]);
    EXPECT_EQ(vector<string>{"ε"}, productions[1]);
}

TEST(TestProductionTrie, LongestCommonPrefixSameCount2) {
    const ProductionTrie trie;
    trie.insert(vector<string>{"a", "b", "d"}, 0);
    trie.insert(vector<string>{"a", "b", "d", "e"}, 1);
    trie.insert(vector<string>{"a", "b", "c"}, 2);
    trie.insert(vector<string>{"a", "b", "c", "d"}, 3);
    const auto [prefix, node] = trie.findLongestCommonPrefix();
    ASSERT_EQ(3, prefix.size());
    EXPECT_EQ("a", prefix[0]);
    EXPECT_EQ("b", prefix[1]);
    EXPECT_EQ("c", prefix[2]);
    ASSERT_EQ(2, node->originalIndices.size());
    EXPECT_TRUE(node->originalIndices.contains(2));
    EXPECT_TRUE(node->originalIndices.contains(3));
    const auto productions = ProductionTrie::computeProductionsUnderPrefix(node);
    ASSERT_EQ(2, productions.size());
    EXPECT_EQ(vector<string>{"d"}, productions[0]);
    EXPECT_EQ(vector<string>{"ε"}, productions[1]);
}

TEST(TestProductionTrie, LongestCommonPrefixExpansionIndices1) {
    const ProductionTrie trie;
    trie.insert(vector<string>{"a", "b", "S"}, 0, 0);
    trie.insert(vector<string>{"a", "b", "c", "d"}, 1, 1);
    const auto [prefix, node] = trie.findLongestCommonPrefix();
    const unordered_map<int, int> parents({{1, 0}});
    const auto productions = ProductionTrie::computeProductionsUnderPrefix(node, &parents);
    ASSERT_EQ(1, productions.size());
    EXPECT_EQ(vector<string>{"S"}, productions[0]);
}

TEST(TestProductionTrie, LongestCommonPrefixExpansionIndices2) {
    const ProductionTrie trie;
    trie.insert(vector<string>{"a", "b", "S"}, 0, 0);
    trie.insert(vector<string>{"a", "b", "c", "d"}, 1, 1);
    trie.insert(vector<string>{"a", "b", "e"}, 1, 2);
    const auto [prefix, node] = trie.findLongestCommonPrefix();
    const unordered_map<int, int> parents({{1, 0}, {2, 1}});
    const auto productions = ProductionTrie::computeProductionsUnderPrefix(node, &parents);
    ASSERT_EQ(1, productions.size());
    EXPECT_EQ(vector<string>{"S"}, productions[0]);
}

TEST(TestProductionTrie, LongestCommonPrefixExpansionIndices3) {
    const ProductionTrie trie;
    trie.insert(vector<string>{"a", "d", "S"}, 0, 0);
    trie.insert(vector<string>{"a", "b", "c", "d"}, 1, 1);
    trie.insert(vector<string>{"a", "b", "e"}, 1, 2);
    const auto [prefix, node] = trie.findLongestCommonPrefix();
    const unordered_map<int, int> parents({{1, 0}, {2, 1}});
    const auto productions = ProductionTrie::computeProductionsUnderPrefix(node, &parents);
    ASSERT_EQ(1, productions.size());
    EXPECT_EQ(vector<string>({"c", "d"}), productions[0]);
}

TEST(TestProductionTrie, LongestCommonPrefixExpansionIndices4) {
    const ProductionTrie trie;
    trie.insert(vector<string>{"a", "d", "S"}, 0, 0);
    trie.insert(vector<string>{"a", "b", "c", "d"}, 1, 1);
    trie.insert(vector<string>{"a", "b", "e"}, 1, 2);
    const auto [prefix, node] = trie.findLongestCommonPrefix();
    const unordered_map<int, int> parents({{1, 0}, {2, 0}});
    const auto productions = ProductionTrie::computeProductionsUnderPrefix(node, &parents);
    ASSERT_EQ(2, productions.size());
    EXPECT_EQ(vector<string>({"c", "d"}), productions[0]);
    EXPECT_EQ(vector<string>({"e"}), productions[1]);
}

TEST(TestProductionTrie, LongestCommonPrefixRemoveNode) {
    const ProductionTrie trie;
    trie.insert(vector<string>{"a", "b", "d"}, 0);
    trie.insert(vector<string>{"a", "b", "d", "e"}, 1);
    trie.insert(vector<string>{"a", "b", "c"}, 2);
    trie.insert(vector<string>{"a", "b", "c", "d"}, 3);
    const auto [prefixPrev, nodePrev] = trie.findLongestCommonPrefix();
    ProductionTrie::removeNode(nodePrev);
    const auto [prefix, node] = trie.findLongestCommonPrefix();
    ASSERT_EQ(3, prefix.size());
    EXPECT_EQ("a", prefix[0]);
    EXPECT_EQ("b", prefix[1]);
    EXPECT_EQ("d", prefix[2]);
    ASSERT_EQ(2, node->originalIndices.size());
    EXPECT_TRUE(node->originalIndices.contains(0));
    EXPECT_TRUE(node->originalIndices.contains(1));
    const auto productions = ProductionTrie::computeProductionsUnderPrefix(node);
    ASSERT_EQ(2, productions.size());
    EXPECT_EQ(vector<string>{"e"}, productions[0]);
    EXPECT_EQ(vector<string>{"ε"}, productions[1]);
    ProductionTrie::removeNode(node);
    const auto [prefixAfter, nodeAfter] = trie.findLongestCommonPrefix();
    ASSERT_EQ(0, prefixAfter.size());
}

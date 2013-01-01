#include "production_trie.h"
#include "string_utils.h"
#include <gtest/gtest.h>
#include <ranges>
#include <algorithm>

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
    ASSERT_EQ(2, node->indices.size());
    EXPECT_EQ(0, node->indices[0]);
    EXPECT_EQ(1, node->indices[1]);
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
    ASSERT_EQ(2, node->indices.size());
    EXPECT_EQ(0, node->indices[0]);
    EXPECT_EQ(1, node->indices[1]);
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
    ASSERT_EQ(2, node->indices.size());
    EXPECT_EQ(0, node->indices[0]);
    EXPECT_EQ(1, node->indices[1]);
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
    ASSERT_EQ(2, node->indices.size());
    EXPECT_EQ(2, node->indices[0]);
    EXPECT_EQ(3, node->indices[1]);
    const auto productions = ProductionTrie::computeProductionsUnderPrefix(node);
    ASSERT_EQ(2, productions.size());
    EXPECT_EQ(vector<string>{"d"}, productions[0]);
    EXPECT_EQ(vector<string>{"ε"}, productions[1]);
}

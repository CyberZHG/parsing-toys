#include "re.h"
#include <gtest/gtest.h>

using namespace std;

TEST(TestRegexParse, Empty) {
    const RegularExpression re("ε");
    const auto ast = re.ast();
    ASSERT_NE(ast, nullptr);
    EXPECT_EQ(ast->type, RegexNode::Type::EMPTY);
    EXPECT_EQ(ast->begin, 0);
    EXPECT_EQ(ast->end, 1);
}

TEST(TestRegexParse, Text) {
    const RegularExpression re("a");
    const auto ast = re.ast();
    ASSERT_NE(ast, nullptr);
    EXPECT_EQ(ast->type, RegexNode::Type::TEXT);
    EXPECT_EQ(ast->text, "a");
    EXPECT_EQ(ast->begin, 0);
    EXPECT_EQ(ast->end, 1);
}

TEST(TestRegexParse, Cat) {
    const RegularExpression re("ab");
    const auto ast = re.ast();
    ASSERT_NE(ast, nullptr);
    EXPECT_EQ(ast->type, RegexNode::Type::CAT);
    EXPECT_EQ(ast->begin, 0);
    EXPECT_EQ(ast->end, 2);
    ASSERT_EQ(ast->parts.size(), 2);

    EXPECT_EQ(ast->parts[0]->type, RegexNode::Type::TEXT);
    EXPECT_EQ(ast->parts[0]->text, "a");
    EXPECT_EQ(ast->parts[0]->begin, 0);
    EXPECT_EQ(ast->parts[0]->end, 1);

    EXPECT_EQ(ast->parts[1]->type, RegexNode::Type::TEXT);
    EXPECT_EQ(ast->parts[1]->text, "b");
    EXPECT_EQ(ast->parts[1]->begin, 1);
    EXPECT_EQ(ast->parts[1]->end, 2);
}

TEST(TestRegexParse, Or) {
    const RegularExpression re("a|b");
    const auto ast = re.ast();
    ASSERT_NE(ast, nullptr);
    EXPECT_EQ(ast->type, RegexNode::Type::OR);
    EXPECT_EQ(ast->begin, 0);
    EXPECT_EQ(ast->end, 3);
    ASSERT_EQ(ast->parts.size(), 2);

    EXPECT_EQ(ast->parts[0]->type, RegexNode::Type::TEXT);
    EXPECT_EQ(ast->parts[0]->text, "a");

    EXPECT_EQ(ast->parts[1]->type, RegexNode::Type::TEXT);
    EXPECT_EQ(ast->parts[1]->text, "b");
}

TEST(TestRegexParse, Brackets) {
    const RegularExpression re("(a)");
    const auto ast = re.ast();
    ASSERT_NE(ast, nullptr);
    EXPECT_EQ(ast->type, RegexNode::Type::TEXT);
    EXPECT_EQ(ast->text, "a");
    EXPECT_EQ(ast->begin, 0);
    EXPECT_EQ(ast->end, 3);
}

TEST(TestRegexParse, Star) {
    const RegularExpression re("a*");
    const auto ast = re.ast();
    ASSERT_NE(ast, nullptr);
    EXPECT_EQ(ast->type, RegexNode::Type::STAR);
    EXPECT_EQ(ast->begin, 0);
    EXPECT_EQ(ast->end, 2);
    ASSERT_NE(ast->sub, nullptr);
    EXPECT_EQ(ast->sub->type, RegexNode::Type::TEXT);
    EXPECT_EQ(ast->sub->text, "a");
}

TEST(TestRegexParse, Example373a) {
    const RegularExpression re("(a|b)*");
    const auto ast = re.ast();
    ASSERT_NE(ast, nullptr);
    EXPECT_EQ(ast->type, RegexNode::Type::STAR);
    EXPECT_EQ(ast->begin, 0);
    EXPECT_EQ(ast->end, 6);
    ASSERT_NE(ast->sub, nullptr);
    EXPECT_EQ(ast->sub->type, RegexNode::Type::OR);
    ASSERT_EQ(ast->sub->parts.size(), 2);
    EXPECT_EQ(ast->sub->parts[0]->type, RegexNode::Type::TEXT);
    EXPECT_EQ(ast->sub->parts[0]->text, "a");
    EXPECT_EQ(ast->sub->parts[1]->type, RegexNode::Type::TEXT);
    EXPECT_EQ(ast->sub->parts[1]->text, "b");
}

TEST(TestRegexParse, Example373b) {
    const RegularExpression re("(a*|b*)*");
    const auto ast = re.ast();
    ASSERT_NE(ast, nullptr);
    EXPECT_EQ(ast->type, RegexNode::Type::STAR);
    EXPECT_EQ(ast->begin, 0);
    EXPECT_EQ(ast->end, 8);
    ASSERT_NE(ast->sub, nullptr);
    EXPECT_EQ(ast->sub->type, RegexNode::Type::OR);
    ASSERT_EQ(ast->sub->parts.size(), 2);
    EXPECT_EQ(ast->sub->parts[0]->type, RegexNode::Type::STAR);
    EXPECT_EQ(ast->sub->parts[0]->sub->text, "a");
    EXPECT_EQ(ast->sub->parts[1]->type, RegexNode::Type::STAR);
    EXPECT_EQ(ast->sub->parts[1]->sub->text, "b");
}

TEST(TestRegexParse, Example373c) {
    const RegularExpression re("((ε|a)b*)*");
    const auto ast = re.ast();
    ASSERT_NE(ast, nullptr);
    EXPECT_EQ(ast->type, RegexNode::Type::STAR);
    EXPECT_EQ(ast->begin, 0);
    EXPECT_EQ(ast->end, 10);
    ASSERT_NE(ast->sub, nullptr);
    EXPECT_EQ(ast->sub->type, RegexNode::Type::CAT);
    ASSERT_EQ(ast->sub->parts.size(), 2);
    EXPECT_EQ(ast->sub->parts[0]->type, RegexNode::Type::OR);
    EXPECT_EQ(ast->sub->parts[0]->parts[0]->type, RegexNode::Type::EMPTY);
    EXPECT_EQ(ast->sub->parts[0]->parts[1]->type, RegexNode::Type::TEXT);
    EXPECT_EQ(ast->sub->parts[1]->type, RegexNode::Type::STAR);
    EXPECT_EQ(ast->sub->parts[1]->sub->text, "b");
}

TEST(TestRegexParse, Example373d) {
    const RegularExpression re("(a|b)*abb(a|b)*");
    const auto ast = re.ast();
    ASSERT_NE(ast, nullptr);
    EXPECT_EQ(ast->type, RegexNode::Type::CAT);
    EXPECT_EQ(ast->begin, 0);
    EXPECT_EQ(ast->end, 15);
    ASSERT_EQ(ast->parts.size(), 5);
    EXPECT_EQ(ast->parts[0]->type, RegexNode::Type::STAR);
    EXPECT_EQ(ast->parts[1]->type, RegexNode::Type::TEXT);
    EXPECT_EQ(ast->parts[1]->text, "a");
    EXPECT_EQ(ast->parts[2]->type, RegexNode::Type::TEXT);
    EXPECT_EQ(ast->parts[2]->text, "b");
    EXPECT_EQ(ast->parts[3]->type, RegexNode::Type::TEXT);
    EXPECT_EQ(ast->parts[3]->text, "b");
    EXPECT_EQ(ast->parts[4]->type, RegexNode::Type::STAR);
}

TEST(TestRegexParse, ErrorMissingBracket) {
    RegularExpression re;
    EXPECT_FALSE(re.parse("(a"));
    EXPECT_EQ(re.errorMessage(), "Error: missing right bracket for 1.");
}

TEST(TestRegexParse, ErrorUnexpectedStar) {
    RegularExpression re;
    EXPECT_FALSE(re.parse("*"));
    EXPECT_EQ(re.errorMessage(), "Error: unexpected * at 0.");
}

TEST(TestRegexParse, ErrorUnexpectedPlus) {
    RegularExpression re;
    EXPECT_FALSE(re.parse("+"));
    EXPECT_EQ(re.errorMessage(), "Error: unexpected + at 0.");
}

TEST(TestRegexParse, ErrorEmptyInput) {
    RegularExpression re;
    EXPECT_FALSE(re.parse(""));
    EXPECT_EQ(re.errorMessage(), "Error: empty input at 0.");
}

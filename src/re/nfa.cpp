#include "re.h"
#include "string_utils.h"

using namespace std;

const string RegularExpression::EPSILON = "ε";

RegularExpression::RegularExpression(const string& pattern) {
    parse(pattern);
}

bool RegularExpression::parse(const string& pattern) {
    _errorMessage.clear();
    const auto graphemes = segmentGraphemes(pattern);
    _ast = parseSub(graphemes, 0, graphemes.size(), true);
    return _ast != nullptr;
}

const string& RegularExpression::errorMessage() const {
    return _errorMessage;
}

shared_ptr<RegexNode> RegularExpression::ast() const {
    return _ast;
}

shared_ptr<RegexNode> RegularExpression::parseSub(const vector<string>& graphemes, const size_t begin, const size_t end, const bool first) {
    auto node = make_shared<RegexNode>();
    node->begin = begin;
    node->end = end;

    if (begin == end) {
        _errorMessage = "Error: empty input at " + to_string(begin) + ".";
        return nullptr;
    }

    if (first) {
        size_t last = begin;
        int stack = 0;
        vector<shared_ptr<RegexNode>> parts;

        for (size_t i = begin; i <= end; ++i) {
            const string& ch = (i < end) ? graphemes[i] : "";
            if (i == end || (ch == "|" && stack == 0)) {
                if (last == begin && i == end) {
                    return parseSub(graphemes, last, i, false);
                }
                auto sub = parseSub(graphemes, last, i, true);
                if (!sub) {
                    return nullptr;
                }
                parts.push_back(sub);
                last = i + 1;
            } else if (ch == "(") {
                stack++;
            } else if (ch == ")") {
                stack--;
            }
        }

        if (parts.size() == 1) {
            return parts[0];
        }
        node->type = RegexNode::Type::OR;
        node->parts = parts;
    } else {
        size_t i = begin;
        vector<shared_ptr<RegexNode>> parts;

        while (i < end) {
            if (const string& ch = graphemes[i]; ch == "(") {
                const size_t last = i + 1;
                i++;
                int stack = 1;
                while (i < end && stack != 0) {
                    if (const string& c = graphemes[i]; c == "(") {
                        stack++;
                    } else if (c == ")") {
                        stack--;
                    }
                    i++;
                }
                if (stack != 0) {
                    _errorMessage = "Error: missing right bracket for " + to_string(last) + ".";
                    return nullptr;
                }
                i--;
                auto sub = parseSub(graphemes, last, i, true);
                if (!sub) {
                    return nullptr;
                }
                sub->begin = last - 1;
                sub->end = i + 1;
                parts.push_back(sub);
            } else if (ch == "*") {
                if (parts.empty()) {
                    _errorMessage = "Error: unexpected * at " + to_string(i) + ".";
                    return nullptr;
                }
                auto tempNode = make_shared<RegexNode>();
                tempNode->begin = parts.back()->begin;
                tempNode->end = parts.back()->end + 1;
                tempNode->type = RegexNode::Type::STAR;
                tempNode->sub = parts.back();
                parts.back() = tempNode;
            } else if (ch == "+") {
                if (parts.empty()) {
                    _errorMessage = "Error: unexpected + at " + to_string(i) + ".";
                    return nullptr;
                }
                auto virNode = make_shared<RegexNode>();
                virNode->begin = parts.back()->begin;
                virNode->end = parts.back()->end + 1;
                virNode->type = RegexNode::Type::STAR;
                virNode->sub = parts.back();

                const auto tempNode = make_shared<RegexNode>();
                tempNode->begin = parts.back()->begin;
                tempNode->end = parts.back()->end + 1;
                tempNode->type = RegexNode::Type::CAT;
                tempNode->parts = {parts.back(), virNode};
                parts.back() = tempNode;
            } else if (ch == "?") {
                if (parts.empty()) {
                    _errorMessage = "Error: unexpected ? at " + to_string(i) + ".";
                    return nullptr;
                }
                auto virNode = make_shared<RegexNode>();
                virNode->begin = parts.back()->begin;
                virNode->end = parts.back()->end + 1;
                virNode->type = RegexNode::Type::EMPTY;

                const auto tempNode = make_shared<RegexNode>();
                tempNode->begin = parts.back()->begin;
                tempNode->end = parts.back()->end + 1;
                tempNode->type = RegexNode::Type::OR;
                tempNode->parts = {parts.back(), virNode};
                parts.back() = tempNode;
            } else if (ch == EPSILON) {
                auto tempNode = make_shared<RegexNode>();
                tempNode->begin = i;
                tempNode->end = i + 1;
                tempNode->type = RegexNode::Type::EMPTY;
                parts.push_back(tempNode);
            } else {
                auto tempNode = make_shared<RegexNode>();
                tempNode->begin = i;
                tempNode->end = i + 1;
                tempNode->type = RegexNode::Type::TEXT;
                tempNode->text = ch;
                parts.push_back(tempNode);
            }
            i++;
        }

        if (parts.size() == 1) {
            return parts[0];
        }
        node->type = RegexNode::Type::CAT;
        node->parts = parts;
    }

    return node;
}

static size_t generateGraph(const shared_ptr<RegexNode>& node, const shared_ptr<NFAState>& start, const shared_ptr<NFAState>& end, size_t count) {
    if (start->id == NFAState::UNASSIGNED_ID) {
        start->id = count++;
    }

    switch (node->type) {
        case RegexNode::Type::EMPTY:
            start->edges.emplace_back(RegularExpression::EPSILON, end);
            break;
        case RegexNode::Type::TEXT:
            start->edges.emplace_back(node->text, end);
            break;
        case RegexNode::Type::CAT: {
            auto last = start;
            for (size_t i = 0; i < node->parts.size() - 1; ++i) {
                auto temp = make_shared<NFAState>();
                count = generateGraph(node->parts[i], last, temp, count);
                last = temp;
            }
            count = generateGraph(node->parts.back(), last, end, count);
            break;
        }
        case RegexNode::Type::OR:
            for (const auto& part : node->parts) {
                auto tempStart = make_shared<NFAState>();
                auto tempEnd = make_shared<NFAState>();
                tempEnd->edges.emplace_back(RegularExpression::EPSILON, end);
                start->edges.emplace_back(RegularExpression::EPSILON, tempStart);
                count = generateGraph(part, tempStart, tempEnd, count);
            }
            break;
        case RegexNode::Type::STAR: {
            auto tempStart = make_shared<NFAState>();
            const auto tempEnd = make_shared<NFAState>();
            tempEnd->edges.emplace_back(RegularExpression::EPSILON, tempStart);
            tempEnd->edges.emplace_back(RegularExpression::EPSILON, end);
            start->edges.emplace_back(RegularExpression::EPSILON, tempStart);
            start->edges.emplace_back(RegularExpression::EPSILON, end);
            count = generateGraph(node->sub, tempStart, tempEnd, count);
            break;
        }
    }

    if (end->id == NFAState::UNASSIGNED_ID) {
        end->id = count++;
    }

    return count;
}

shared_ptr<NFAState> RegularExpression::toNFA() const {
    if (!_ast) {
        return nullptr;
    }

    auto start = make_shared<NFAState>();
    start->type = "start";
    const auto accept = make_shared<NFAState>();
    accept->type = "accept";

    generateGraph(_ast, start, accept, 0);

    return start;
}

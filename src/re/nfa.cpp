#include "re.h"
#include "string_utils.h"

using namespace std;

const string RegularExpression::EPSILON = "ε";

bool RegexNode::operator==(const RegexNode& other) const {
    if (type != other.type || begin != other.begin || end != other.end) {
        return false;
    }
    if (type == Type::TEXT && text != other.text) {
        return false;
    }
    if (type == Type::STAR) {
        if (!sub || !other.sub) {
            return sub == other.sub;
        }
        return *sub == *other.sub;
    }
    if (type == Type::CAT || type == Type::OR) {
        if (parts.size() != other.parts.size()) {
            return false;
        }
        for (size_t i = 0; i < parts.size(); ++i) {
            if (!parts[i] || !other.parts[i]) {
                if (parts[i] != other.parts[i]) {
                    return false;
                }
            } else if (*parts[i] != *other.parts[i]) {
                return false;
            }
        }
    }
    return true;
}

string RegexNode::toString() const {
    string result;
    switch (type) {
        case Type::EMPTY:
            result = "ε";
            break;
        case Type::TEXT:
            result = text;
            break;
        case Type::CAT:
            for (const auto& part : parts) {
                result += part->toString();
            }
            break;
        case Type::OR:
            result = "(";
            for (size_t i = 0; i < parts.size(); ++i) {
                if (i > 0) {
                    result += "|";
                }
                result += parts[i]->toString();
            }
            result += ")";
            break;
        case Type::STAR:
            result = "(" + sub->toString() + ")*";
            break;
    }
    return result;
}

bool NFAState::operator==(const NFAState& other) const {
    if (id != other.id || type != other.type || edges.size() != other.edges.size()) {
        return false;
    }
    for (size_t i = 0; i < edges.size(); ++i) {
        if (edges[i].first != other.edges[i].first) {
            return false;
        }
        if (!edges[i].second || !other.edges[i].second) {
            if (edges[i].second != other.edges[i].second) {
                return false;
            }
        } else if (edges[i].second->id != other.edges[i].second->id) {
            return false;
        }
    }
    return true;
}

string NFAState::toString() const {
    string result = "State " + to_string(id) + " (" + type + ")";
    for (const auto& [label, target] : edges) {
        result += " --" + label + "--> " + to_string(target->id);
    }
    return result;
}

RegularExpression::RegularExpression(const string& pattern) {
    parse(pattern);
}

bool RegularExpression::parse(const string& pattern) {
    _errorMessage.clear();
    _ast = parseSub(pattern, 0, utf8Length(pattern), true);
    return _ast != nullptr;
}

const string& RegularExpression::errorMessage() const {
    return _errorMessage;
}

shared_ptr<RegexNode> RegularExpression::ast() const {
    return _ast;
}

shared_ptr<RegexNode> RegularExpression::parseSub(const string& text, size_t begin, size_t end, bool first) {
    auto node = make_shared<RegexNode>();
    node->begin = begin;
    node->end = end;

    size_t len = utf8Length(text);
    if (len == 0) {
        _errorMessage = "Error: empty input at " + to_string(begin) + ".";
        return nullptr;
    }

    if (first) {
        size_t last = 0;
        int stack = 0;
        vector<shared_ptr<RegexNode>> parts;

        for (size_t i = 0; i <= len; ++i) {
            const string ch = (i < len) ? utf8CharAt(text, i) : "";
            if (i == len || (ch == "|" && stack == 0)) {
                if (last == 0 && i == len) {
                    return parseSub(text, begin + last, begin + i, false);
                }
                string substr = utf8Substring(text, last, i - last);
                auto sub = parseSub(substr, begin + last, begin + i, true);
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
        size_t i = 0;
        vector<shared_ptr<RegexNode>> parts;

        while (i < len) {
            if (const string ch = utf8CharAt(text, i); ch == "(") {
                size_t last = i + 1;
                i++;
                int stack = 1;
                while (i < len && stack != 0) {
                    if (string c = utf8CharAt(text, i); c == "(") {
                        stack++;
                    } else if (c == ")") {
                        stack--;
                    }
                    i++;
                }
                if (stack != 0) {
                    _errorMessage = "Error: missing right bracket for " + to_string(begin + last) + ".";
                    return nullptr;
                }
                i--;
                string substr = utf8Substring(text, last, i - last);
                auto sub = parseSub(substr, begin + last, begin + i, true);
                if (!sub) {
                    return nullptr;
                }
                sub->begin = begin + last - 1;
                sub->end = begin + i + 1;
                parts.push_back(sub);
            } else if (ch == "*") {
                if (parts.empty()) {
                    _errorMessage = "Error: unexpected * at " + to_string(begin + i) + ".";
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
                    _errorMessage = "Error: unexpected + at " + to_string(begin + i) + ".";
                    return nullptr;
                }
                auto virNode = make_shared<RegexNode>();
                virNode->begin = parts.back()->begin;
                virNode->end = parts.back()->end + 1;
                virNode->type = RegexNode::Type::STAR;
                virNode->sub = parts.back();

                auto tempNode = make_shared<RegexNode>();
                tempNode->begin = parts.back()->begin;
                tempNode->end = parts.back()->end + 1;
                tempNode->type = RegexNode::Type::CAT;
                tempNode->parts = {parts.back(), virNode};
                parts.back() = tempNode;
            } else if (ch == "?") {
                if (parts.empty()) {
                    _errorMessage = "Error: unexpected ? at " + to_string(begin + i) + ".";
                    return nullptr;
                }
                auto virNode = make_shared<RegexNode>();
                virNode->begin = parts.back()->begin;
                virNode->end = parts.back()->end + 1;
                virNode->type = RegexNode::Type::EMPTY;

                auto tempNode = make_shared<RegexNode>();
                tempNode->begin = parts.back()->begin;
                tempNode->end = parts.back()->end + 1;
                tempNode->type = RegexNode::Type::OR;
                tempNode->parts = {parts.back(), virNode};
                parts.back() = tempNode;
            } else if (ch == EPSILON) {
                auto tempNode = make_shared<RegexNode>();
                tempNode->begin = begin + i;
                tempNode->end = begin + i + 1;
                tempNode->type = RegexNode::Type::EMPTY;
                parts.push_back(tempNode);
            } else {
                auto tempNode = make_shared<RegexNode>();
                tempNode->begin = begin + i;
                tempNode->end = begin + i + 1;
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
    if (!_ast) return nullptr;

    auto start = make_shared<NFAState>();
    start->type = "start";
    const auto accept = make_shared<NFAState>();
    accept->type = "accept";

    generateGraph(_ast, start, accept, 0);

    return start;
}

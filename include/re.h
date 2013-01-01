#ifndef PARSING_TOYS_RE_H
#define PARSING_TOYS_RE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

struct RegexNode {
    enum class Type {
        EMPTY,
        TEXT,
        CAT,
        OR,
        STAR,
    };

    Type type;
    std::size_t begin = 0;
    std::size_t end = 0;
    std::string text;
    std::shared_ptr<RegexNode> sub;
    std::vector<std::shared_ptr<RegexNode>> parts;

    bool operator==(const RegexNode& other) const;
    [[nodiscard]] std::string toString() const;
};

struct NFAState {
    static constexpr std::size_t UNASSIGNED_ID = static_cast<std::size_t>(-1);
    std::size_t id = UNASSIGNED_ID;
    std::string type;
    std::vector<std::pair<std::string, std::shared_ptr<NFAState>>> edges;

    bool operator==(const NFAState& other) const;
    [[nodiscard]] std::string toString() const;
};

struct DFAState {
    std::string id;
    std::string key;
    std::string type;
    std::vector<std::shared_ptr<NFAState>> items;
    std::vector<std::string> symbols;
    std::vector<std::pair<std::string, std::shared_ptr<DFAState>>> edges;
    std::unordered_map<std::string, std::shared_ptr<DFAState>> trans;

    bool operator==(const DFAState& other) const;
    [[nodiscard]] std::string toString() const;
};

class RegularExpression {
public:
    static const std::string EPSILON;

    RegularExpression() = default;
    explicit RegularExpression(const std::string& pattern);

    bool parse(const std::string& pattern);
    [[nodiscard]] const std::string& errorMessage() const;
    [[nodiscard]] std::shared_ptr<RegexNode> ast() const;

    [[nodiscard]] std::shared_ptr<NFAState> toNFA() const;
    [[nodiscard]] static std::shared_ptr<DFAState> toDFA(const std::shared_ptr<NFAState>& nfa);
    [[nodiscard]] static std::shared_ptr<DFAState> toMinDFA(const std::shared_ptr<DFAState>& dfa);

private:
    std::string _errorMessage;
    std::shared_ptr<RegexNode> _ast;

    std::shared_ptr<RegexNode> parseSub(const std::string& text, std::size_t begin, std::size_t end, bool first);
};

#endif //PARSING_TOYS_RE_H

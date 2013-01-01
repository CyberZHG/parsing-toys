#ifndef PARSING_TOYS_CFG_H
#define PARSING_TOYS_CFG_H

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
using namespace std;

struct ContextFreeGrammarToken {
    enum class Type {
        SYMBOL,
        PRODUCTION,
        ALTERNATION,
    };
    Type type;
    string symbol;
    size_t line, column;

    bool operator==(const ContextFreeGrammarToken& other) const;
};

class ContextFreeGrammar {
public:
    using Symbol = string;
    using Production = vector<string>;
    using Productions = vector<Production>;

    ContextFreeGrammar() = default;

    static vector<ContextFreeGrammarToken> tokenize(const string& s);
    bool parse(const string& s);
    [[nodiscard]] const string& errorMessage() const;
    [[nodiscard]] string toString() const;

private:
    static const string EMPTY_SYMBOL;

    string _errorMessage;
    vector<Symbol> _ordering;
    unordered_map<Symbol, Productions> _productions;
    unordered_set<Symbol> _terminals;
};

void PrintTo(const ContextFreeGrammarToken& token, ostream* os);

#endif //PARSING_TOYS_CFG_H
#include "cfg.h"

const string ContextFreeGrammar::EMPTY_SYMBOL = "ϵ";

bool ContextFreeGrammarToken::operator==(const ContextFreeGrammarToken& other) const {
    return type == other.type && symbol == other.symbol && line == other.line && column == other.column;
}

vector<ContextFreeGrammarToken> ContextFreeGrammar::tokenize(const string& s) {
    const auto n = s.size();
    vector<ContextFreeGrammarToken> tokens;
    size_t line = 1, column = 1;
    for (size_t i = 0; i < n;) {
        if (i + 1 < n && s[i] == '\r' && s[i + 1] == '\n') {
            ++line;
            column = 1;
            i += 2;
        } else if (s[i] == '\n' || s[i] == '\r') {
            ++line;
            column = 1;
            ++i;
        } else if (isspace(s[i])) {
            ++column;
            ++i;
        } else if (i + 1 < n && s[i] == '-' && s[i + 1] == '>') {
            tokens.emplace_back(ContextFreeGrammarToken{ContextFreeGrammarToken::Type::PRODUCE_SIGN, "->", line, column});
            i += 2;
            column += 2;
        } else if (s[i] == '|') {
            tokens.emplace_back(ContextFreeGrammarToken{ContextFreeGrammarToken::Type::OR_SIGN, "|", line, column});
            ++i;
            ++column;
        } else {
            for (size_t j = i + 1; j <= n; ++j) {
                if (j == n || isspace(s[j]) || s[j] == '|' || (j + 1 < n && s[j] == '-' && s[j + 1] == '>')) {
                    tokens.emplace_back(ContextFreeGrammarToken{ContextFreeGrammarToken::Type::SYMBOL, s.substr(i, j - i), line, column});
                    column += j - i;
                    i = j;
                    break;
                }
            }
        }
    }
    return tokens;
}

void PrintTo(const ContextFreeGrammarToken& token, ostream* os) {
    *os << "{" << static_cast<int>(token.type) << "," << token.symbol << "," << token.line << "," << token.column << "}";
}

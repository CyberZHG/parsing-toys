#ifndef PARSING_TOYS_CFG_H
#define PARSING_TOYS_CFG_H

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
using namespace std;

/**
 * Tokenized results.
 */
struct ContextFreeGrammarToken {
    enum class Type {
        SYMBOL,
        PRODUCTION,  // The '->' sign
        ALTERNATION,  // The '|' sign
    };
    Type type;
    string symbol;
    size_t line, column;

    bool operator==(const ContextFreeGrammarToken& other) const;
};

struct FirstAndFollowSet {
    vector<string> ordering;
    unordered_map<string, unordered_set<string>> first;
    unordered_map<string, unordered_set<string>> follow;

    [[nodiscard]] size_t size() const;
    [[nodiscard]] string symbolAt(size_t index) const;
    [[nodiscard]] bool getNullable(const string &symbol) const;
    [[nodiscard]] vector<string> getFirstSet(const string& symbol) const;
    [[nodiscard]] vector<string> getFollowSet(const string& symbol) const;
};

class ContextFreeGrammar {
public:
    using Symbol = string;
    using Production = vector<string>;
    using Productions = vector<Production>;

    ContextFreeGrammar() = default;

    static const string EMPTY_SYMBOL;

    /**
     * Tokenization.
     * @param s A string representing a context-free grammar.
     * @return Tokens
     */
    static vector<ContextFreeGrammarToken> tokenize(const string& s);

    /**
     * Tokenize and parse a context-free grammar.
     * @param s A string representing a context-free grammar.
     * @return
     */
    bool parse(const string& s);

    /**
     * TPossible error messages during parsing.
     * @return Error message.
     */
    [[nodiscard]] const string& errorMessage() const;

    /**
     * A formatted context-free grammar string.
     * @return Formatted string.
     */
    [[nodiscard]] string toString() const;

    /**
     * Find all terminals in the existing productions.
     */
    void initTerminals();

    /**
     * A helper function for highlighting.
     * @return All the terminals.
     */
    [[nodiscard]] vector<Symbol> terminals() const;
    /**
     * A helper function for highlighting.
     * @return All the non-terminals.
     */
    [[nodiscard]] vector<Symbol> nonTerminals() const;

    [[nodiscard]] bool isTerminal(const Symbol& symbol) const;
    [[nodiscard]] bool isNonTerminal(const Symbol& symbol) const;

    /**
     * Compute a key for a production to deduplicate.
     * @param production
     * @return
     */
    static string computeProductionKey(const Production& production);

    /**
     * Remove duplicated productions for each non-terminal.
     */
    void deduplicate();

    /**
     * Generate a primed symbol.
     * @param symbol Existing non-terminal symbol.
     * @return The primed symbol.
     */
    string generatePrimedSymbol(const Symbol& symbol);

    void addProductions(const Symbol& head, const Productions& productions);

    [[nodiscard]] bool expandable(const Production& production) const;

    /**
     * Find and group the longest common prefixes.
     */
    void leftFactoring();

    /**
     * Try to eliminate left recursions based on the current ordering.
     *
     * When all right-hand sides of a non-terminal’s productions start with that non-terminal,
     * the left recursion cannot be eliminated.
     *
     * @attention This function does not provide atomicity.
     *
     * @return False if the left recursion cannot be eliminated.
     */
    bool leftRecursionElimination();

    /**
     * FIRST set: The set of terminals that can appear as the first symbol.
     * FOLLOW set: The set of terminals that can appear immediately to the right of the non-terminal.
     *
     * @return First and follow sets.
     */
    [[nodiscard]] FirstAndFollowSet computeFirstAndFollowSet() const;

private:
    string _errorMessage;
    vector<Symbol> _ordering;  // The output ordering
    unordered_map<Symbol, Productions> _productions;  // All the productions
    unordered_map<Symbol, unordered_set<string>> _productionKeys;  // Helper member for checking the existence of a production
    unordered_set<Symbol> _terminals;  // Helper member for checking the existence of terminals
};

void PrintTo(const ContextFreeGrammarToken& token, ostream* os);

#endif //PARSING_TOYS_CFG_H
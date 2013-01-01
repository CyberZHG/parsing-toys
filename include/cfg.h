#ifndef PARSING_TOYS_CFG_H
#define PARSING_TOYS_CFG_H

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;

using Symbol = string;
using Production = vector<string>;
using Productions = vector<Production>;

class FiniteAutomaton;

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
    Symbol symbol;
    size_t line, column;

    bool operator==(const ContextFreeGrammarToken& other) const;
};

struct FirstAndFollowSet {
    vector<Symbol> ordering;
    unordered_map<Symbol, unordered_set<Symbol>> first;
    unordered_map<Symbol, unordered_set<Symbol>> follow;

    [[nodiscard]] size_t size() const;
    [[nodiscard]] Symbol symbolAt(size_t index) const;
    [[nodiscard]] bool getNullable(const Symbol &symbol) const;
    [[nodiscard]] vector<Symbol> getFirstSet(const Symbol& symbol) const;
    [[nodiscard]] vector<Symbol> getFollowSet(const Symbol& symbol) const;
};

class ContextFreeGrammar {
public:
    ContextFreeGrammar() = default;

    static const string EMPTY_SYMBOL;
    static const string DOT_SYMBOL;

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
     * Compute a key for a production to deduplicate.
     * @param head
     * @param production
     * @return
     */
    static string computeProductionKey(const Symbol& head, const Production& production);

    /**
     * Remove duplicated productions for each non-terminal.
     */
    void deduplicate();

    /**
     * Generate a primed symbol.
     * @param symbol Existing non-terminal symbol.
     * @param updateOrdering Whether to update the output order.
     * @return The primed symbol.
     */
    string generatePrimedSymbol(const Symbol& symbol, bool updateOrdering = true);

    void addProduction(const Symbol &head, const Production &production);
    void addProductions(const Symbol& head, const Productions& productions);

    ContextFreeGrammar operator|(const ContextFreeGrammar& other) const;

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

    /**
     * Compute the closure of an item set. The closure adds all items for non-terminals that can be expanded next.
     *
     * @param kernel The kernel set.
     * @return The non-kernel set.
     */
    [[nodiscard]] ContextFreeGrammar computeClosure(const ContextFreeGrammar& kernel) const;

    /**
     * Compute an automaton for LR(0) parsing.
     * LR(0) parses input using states built from items, with no lookahead.
     *
     * @return A deterministic finite automaton.
     */
    unique_ptr<FiniteAutomaton> computeLR0Automaton();
private:
    string _errorMessage;
    vector<Symbol> _ordering;  // The output ordering
    unordered_map<Symbol, Productions> _productions;  // All the productions
    unordered_map<Symbol, unordered_set<string>> _productionKeys;  // Helper member for checking the existence of a production
    unordered_set<Symbol> _terminals;  // Helper member for checking the existence of terminals
};

void PrintTo(const ContextFreeGrammarToken& token, ostream* os);

#endif //PARSING_TOYS_CFG_H
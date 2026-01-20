#ifndef PARSING_TOYS_CFG_H
#define PARSING_TOYS_CFG_H

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>

using Symbol = std::string;
using Production = std::vector<std::string>;
using Productions = std::vector<Production>;

class FiniteAutomaton;
class ContextFreeGrammar;

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
    std::size_t line, column;

    bool operator==(const ContextFreeGrammarToken& other) const;
};

struct FirstAndFollowSet {
    std::vector<Symbol> ordering;
    std::unordered_map<Symbol, std::unordered_set<Symbol>> first;
    std::unordered_map<Symbol, std::unordered_set<Symbol>> follow;

    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] Symbol symbolAt(std::size_t index) const;
    [[nodiscard]] bool getNullable(const Symbol &symbol) const;
    [[nodiscard]] std::vector<Symbol> getFirstSet(const Symbol& symbol) const;
    [[nodiscard]] std::vector<Symbol> getFollowSet(const Symbol& symbol) const;
};

struct LRParsingSteps {
    std::vector<std::vector<std::size_t>> stack;
    std::vector<std::vector<Symbol>> symbols;
    std::vector<std::vector<Symbol>> remainingInputs;
    std::vector<std::string> actions;

    void addStep(const std::vector<std::size_t>& _stack, const std::vector<std::string>& _symbols, const std::vector<std::string>& _remainingInputs, const std::string &action);

    /** For unit tests only. */
    [[nodiscard]] std::string toString() const;
};

struct ParseTreeNode {
    bool terminal;
    std::string label;
    std::vector<std::shared_ptr<ParseTreeNode>> children;

    [[nodiscard]] size_t size() const;
    [[nodiscard]] std::string toSVG(bool darkMode = false) const;

    /** For unit tests only. */
    [[nodiscard]] std::string toString(int indent = 0) const;
};

struct ActionGotoTable {
    std::vector<std::unordered_map<Symbol, std::vector<std::string>>> actions;
    std::vector<std::unordered_map<Symbol, std::size_t>> nextStates;
    std::vector<std::unordered_map<Symbol, Symbol>> reduceHeads;
    std::vector<std::unordered_map<Symbol, Production>> reduceProductions;
    std::shared_ptr<ParseTreeNode> parseTree = nullptr;

    ActionGotoTable() = default;
    explicit ActionGotoTable(const std::size_t n) : actions(n), nextStates(n), reduceHeads(n), reduceProductions(n) {}

    void addShift(size_t index, const Symbol& symbol, size_t nextState);
    void addGoto(size_t index, const Symbol& symbol, size_t nextState);
    void addReduce(size_t index, const Symbol& symbol, const Symbol& head, Production production);

    [[nodiscard]] bool hasConflict() const;
    [[nodiscard]] bool hasConflict(size_t index, const Symbol& symbol) const;
    [[nodiscard]] std::string toString(size_t index, const Symbol& symbol, const std::string& separator = " / ") const;

    [[nodiscard]] LRParsingSteps parse(const std::string& s);

    /** For unit tests only. */
    [[nodiscard]] std::string toString(const ContextFreeGrammar& grammar, const std::string& separator = " / ") const;
};

struct LLParsingSteps {
    std::vector<std::vector<Symbol>> stack;
    std::vector<std::vector<Symbol>> remainingInputs;
    std::vector<std::string> actions;

    void addStep(const std::vector<Symbol>& _stack, const std::vector<Symbol>& _remainingInputs, const std::string& action);
    [[nodiscard]] std::string toString() const;
};

struct MTable {
    std::vector<Symbol> nonTerminals;
    std::vector<Symbol> terminals;
    std::unordered_map<Symbol, std::size_t> nonTerminalIndex;
    std::unordered_map<Symbol, std::size_t> terminalIndex;
    std::vector<std::vector<std::vector<Production>>> entries;
    std::shared_ptr<ParseTreeNode> parseTree = nullptr;

    void addEntry(const Symbol& nonTerminal, const Symbol& terminal, const Production& production);
    [[nodiscard]] bool hasConflict() const;
    [[nodiscard]] bool hasConflict(const Symbol& nonTerminal, const Symbol& terminal) const;
    [[nodiscard]] std::string getCell(const Symbol& nonTerminal, const Symbol& terminal, const std::string& separator = " / ") const;
    [[nodiscard]] std::size_t numNonTerminals() const;
    [[nodiscard]] std::size_t numTerminals() const;
    [[nodiscard]] Symbol getNonTerminal(std::size_t index) const;
    [[nodiscard]] Symbol getTerminal(std::size_t index) const;

    [[nodiscard]] LLParsingSteps parse(const std::string& s);
    [[nodiscard]] std::string toString(const std::string& separator = " / ") const;
};

struct CYKTable {
    std::size_t n = 0;
    std::vector<std::vector<std::vector<Symbol>>> table;
    std::shared_ptr<ParseTreeNode> parseTree = nullptr;
    bool accepted = false;

    explicit CYKTable(std::size_t size);
    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] std::vector<Symbol> getCell(std::size_t r, std::size_t c) const;
    [[nodiscard]] std::string getCellString(std::size_t r, std::size_t c, const std::string& separator = ", ") const;
};

class ContextFreeGrammar {
public:
    ContextFreeGrammar() = default;

    static const std::string EMPTY_SYMBOL;
    static const std::string DOT_SYMBOL;
    static const std::string EOF_SYMBOL;
    static const std::string LOOKAHEAD_SEPARATOR;
    static const std::string LOOKAHEAD_INNER_SEPARATOR;

    /**
     * Tokenization.
     * @param s A string representing a context-free grammar.
     * @return Tokens
     */
    static std::vector<ContextFreeGrammarToken> tokenize(const std::string& s);

    /**
     * Tokenize and parse a context-free grammar.
     * @param s A string representing a context-free grammar.
     * @return
     */
    bool parse(const std::string& s);

    /**
     * TPossible error messages during parsing.
     * @return Error message.
     */
    [[nodiscard]] const std::string& errorMessage() const;

    /**
     * A formatted context-free grammar string.
     * @return Formatted string.
     */
    [[nodiscard]] std::string toString() const;

    /**
     * A formatted context-free grammar string in lexical order.
     * @return Formatted string.
     */
    [[nodiscard]] std::string toSortedString() const;

    /**
     * Find all terminals in the existing productions.
     */
    void initTerminals();

    /**
     * A helper function for highlighting.
     * @return All the terminals.
     */
    [[nodiscard]] std::vector<Symbol> terminals() const;
    /**
     * A helper function for highlighting.
     * @return All the non-terminals.
     */
    [[nodiscard]] std::vector<Symbol> nonTerminals() const;
    [[nodiscard]] const std::vector<Symbol>& orderedNonTerminals() const;

    [[nodiscard]] bool isTerminal(const Symbol& symbol) const;
    [[nodiscard]] bool isNonTerminal(const Symbol& symbol) const;

    /**
     * Compute a key for a production to deduplicate.
     * @param production
     * @return
     */
    static std::string computeProductionKey(const Production& production);
    /**
     * Compute a key for a production to deduplicate.
     * @param head
     * @param production
     * @return
     */
    static std::string computeProductionKey(const Symbol& head, const Production& production);

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
    std::string generatePrimedSymbol(const Symbol& symbol, bool updateOrdering = true);

    void addProduction(const Symbol &head, const Production &production);
    void addProductions(const Symbol& head, const Productions& productions);

    ContextFreeGrammar operator|(const ContextFreeGrammar& other) const;

    /**
     * Find and group the longest common prefixes.
     *
     * @param expand Whether to expand expressions when checking for common prefixes.
     */
    void leftFactoring(bool expand = false);

    /**
     * Try to eliminate left recursions based on the current ordering.
     *
     * When all right-hand sides of a non-terminal's productions start with that non-terminal,
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
    std::unique_ptr<FiniteAutomaton> computeLR0Automaton();

    /**
     * Compute the ACTION/GOTO table for LR(0) parsing.
     *
     * @param automaton
     * @return
     */
    [[nodiscard]] ActionGotoTable computeLR0ActionGotoTable(const std::unique_ptr<FiniteAutomaton>& automaton) const;

    /**
     * Compute an automaton for SLR(1) parsing.
     * SLR(1) extends LR(0) by adding 1-symbol lookahead using FOLLOW sets.
     * The automaton should be the same as LR(0).
     *
     * @return A deterministic finite automaton.
     */
    std::unique_ptr<FiniteAutomaton> computeSLR1Automaton();

    /**
     * Compute the ACTION/GOTO table for SLR(1) parsing.
     *
     * @param automaton
     * @return
     */
    [[nodiscard]] ActionGotoTable computeSLR1ActionGotoTable(const std::unique_ptr<FiniteAutomaton>& automaton) const;

    /**
     * Compute an automaton for LR(1) parsing.
     * LR(1) uses 1-symbol lookahead stored with each item.
     * Items are [A -> α · β, a] where 'a' is the lookahead terminal.
     *
     * @return A deterministic finite automaton.
     */
    std::unique_ptr<FiniteAutomaton> computeLR1Automaton();

    /**
     * Compute the ACTION/GOTO table for LR(1) parsing.
     *
     * @param automaton
     * @return
     */
    [[nodiscard]] ActionGotoTable computeLR1ActionGotoTable(const std::unique_ptr<FiniteAutomaton>& automaton) const;

    /**
     * Compute an automaton for LALR(1) parsing.
     * LALR(1) merges LR(1) states with identical cores (ignoring lookaheads).
     * This produces fewer states than LR(1) but may introduce reduce-reduce conflicts.
     *
     * @return A deterministic finite automaton.
     */
    std::unique_ptr<FiniteAutomaton> computeLALR1Automaton();

    /**
     * Compute the ACTION/GOTO table for LALR(1) parsing.
     *
     * @param automaton
     * @return
     */
    [[nodiscard]] ActionGotoTable computeLALR1ActionGotoTable(const std::unique_ptr<FiniteAutomaton>& automaton) const;

    /**
     * Compute the LL(1) predictive parsing table.
     * For each production A -> α, add M[A, a] = α for all a in FIRST(α).
     * If ε is in FIRST(α), also add M[A, b] = α for all b in FOLLOW(A).
     *
     * @return The LL(1) parsing table.
     */
    [[nodiscard]] MTable computeLL1Table() const;

    [[nodiscard]] bool isChomskyNormalForm() const;
    void toChomskyNormalForm();

    [[nodiscard]] CYKTable cykParse(const std::string& s) const;

private:
    std::string _errorMessage;
    std::vector<Symbol> _ordering;  // The output ordering
    std::unordered_map<Symbol, Productions> _productions;  // All the productions
    std::unordered_map<Symbol, std::unordered_set<std::string>> _productionKeys;  // Helper member for checking the existence of a production
    std::unordered_set<Symbol> _terminals;  // Helper member for checking the existence of terminals
};

void PrintTo(const ContextFreeGrammarToken& token, std::ostream* os);

#endif //PARSING_TOYS_CFG_H
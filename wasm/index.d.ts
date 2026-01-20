export interface VectorString {
    size(): number
    get(index: number): string
    set(index: number, value: string): void
    push_back(value: string): void
    resize(size: number, value?: string): void
    delete(): void
}

export interface VectorSizeT {
    size(): number
    get(index: number): number
    delete(): void
}

export class FirstAndFollowSet {
    constructor()
    size(): number
    symbolAt(index: number): string
    getNullable(symbol: string): boolean
    _getFirstSet(symbol: string): VectorString
    getFirstSet(symbol: string): string[]
    _getFollowSet(symbol: string): VectorString
    getFollowSet(symbol: string): string[]
}

export class FiniteAutomaton {
    constructor()
    size(): number
    toSVG(darkMode?: boolean): string
}

export class LRParsingSteps {
    constructor()
    size(): number
    _getStack(index: number): VectorSizeT
    getStack(index: number): number[]
    _getSymbols(index: number): VectorString
    getSymbols(index: number): string[]
    _getRemainingInputs(index: number): VectorString
    getRemainingInputs(index: number): string[]
    getAction(index: number): string
}

export class ParseTreeNode {
    constructor()
    toSVG(darkMode?: boolean): string
    delete(): void
}

export class ActionGotoTable {
    constructor()
    size(): number
    hasConflict(): boolean
    hasConflictAt(index: number, symbol: string): boolean
    getCell(index: number, symbol: string, separator: string): string
    parse(input: string): LRParsingSteps
    getParseTree(): ParseTreeNode | null
}

export class LLParsingSteps {
    constructor()
    size(): number
    _getStack(index: number): VectorString
    getStack(index: number): string[]
    _getRemainingInputs(index: number): VectorString
    getRemainingInputs(index: number): string[]
    getAction(index: number): string
}

export class MTable {
    constructor()
    numNonTerminals(): number
    numTerminals(): number
    getNonTerminal(index: number): string
    getTerminal(index: number): string
    hasConflict(): boolean
    hasConflictAt(nonTerminal: string, terminal: string): boolean
    getCell(nonTerminal: string, terminal: string, separator: string): string
    parse(input: string): LLParsingSteps
    getParseTree(): ParseTreeNode | null
}

export class CYKTable {
    constructor(size: number)
    size(): number
    _getCell(i: number, j: number): VectorString
    getCell(i: number, j: number): string[]
    getCellString(i: number, j: number, separator: string): string
    isAccepted(): boolean
    getParseTree(): ParseTreeNode | null
}

export class ContextFreeGrammar {
    constructor()
    parse(code: string): boolean
    errorMessage(): string
    toString(): string
    _terminals(): VectorString
    terminals(): string[]
    _nonTerminals(): VectorString
    nonTerminals(): string[]
    _orderedNonTerminals(): VectorString
    orderedNonTerminals(): string[]
    leftFactoring(expand: boolean): void
    leftRecursionElimination(): boolean
    computeFirstAndFollowSet(): FirstAndFollowSet
    computeLR0Automaton(): FiniteAutomaton
    computeLR0ActionGotoTable(automaton: FiniteAutomaton): ActionGotoTable
    computeSLR1Automaton(): FiniteAutomaton
    computeSLR1ActionGotoTable(automaton: FiniteAutomaton): ActionGotoTable
    computeLR1Automaton(): FiniteAutomaton
    computeLR1ActionGotoTable(automaton: FiniteAutomaton): ActionGotoTable
    computeLALR1Automaton(): FiniteAutomaton
    computeLALR1ActionGotoTable(automaton: FiniteAutomaton): ActionGotoTable
    computeLL1Table(): MTable
    isChomskyNormalForm(): boolean
    toChomskyNormalForm(): void
    cykParse(input: string): CYKTable
}

export class NFAState {
    constructor()
    delete(): void
}

export class DFAState {
    constructor()
    delete(): void
}

export class NFAGraph {
    size(): number
    toSVG(darkMode?: boolean): string
    stateAt(index: number): string
    numEdges(): number
    edgeFrom(index: number): number
    edgeTo(index: number): number
    edgeLabel(index: number): string
}

export class DFAGraph {
    size(): number
    toSVG(darkMode?: boolean): string
    stateIdAt(index: number): string
    stateKeyAt(index: number): string
    stateTypeAt(index: number): string
    numEdges(): number
    edgeFrom(index: number): number
    edgeTo(index: number): number
    edgeLabel(index: number): string
}

export class RegularExpression {
    constructor()
    parse(pattern: string): boolean
    errorMessage(): string
    toNFA(): NFAState
    static toDFA(nfa: NFAState): DFAState
    static toMinDFA(dfa: DFAState): DFAState
    static toNFAGraph(nfa: NFAState): NFAGraph
    static toDFAGraph(dfa: DFAState): DFAGraph
}

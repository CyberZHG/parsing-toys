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
    toSVG(): string
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
    toSVG(): string
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
    leftFactoring(): void
    leftRecursionElimination(): boolean
    computeFirstAndFollowSet(): FirstAndFollowSet
    computeLR0Automaton(): FiniteAutomaton
    computeLR0ActionGotoTable(automaton: FiniteAutomaton): ActionGotoTable
    computeSLR1Automaton(): FiniteAutomaton
    computeSLR1ActionGotoTable(automaton: FiniteAutomaton): ActionGotoTable
}

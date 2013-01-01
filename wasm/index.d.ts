export interface VectorString {
    size(): number
    get(index: number): string
    set(index: number, value: string): void
    push_back(value: string): void
    resize(size: number, value?: string): void
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

export class ContextFreeGrammar {
    constructor()
    parse(code: string): boolean
    errorMessage(): string
    toString(): string
    _terminals(): VectorString
    terminals(): string[]
    _nonTerminals(): VectorString
    nonTerminals(): string[]
    leftFactoring(): void
    leftRecursionElimination(): boolean
    computeFirstAndFollowSet(): FirstAndFollowSet
    computeLR0Automaton(): FiniteAutomaton
}

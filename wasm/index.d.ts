export interface VectorString {
    size(): number
    get(index: number): string
    set(index: number, value: string): void
    push_back(value: string): void
    resize(size: number, value?: string): void
    delete(): void
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
}

import { expect, test } from 'vitest'
import { ContextFreeGrammar } from '../../../wasm/index.js'

test('example-1', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> S S + | S S * | a")
    const automaton = cfg.computeLR0Automaton()
    expect(automaton.size()).eq(6)
})

test('example-2', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("E -> E + T | T  T -> T * F | F  F -> ( E ) | id")
    const automaton = cfg.computeLR0Automaton()
    expect(automaton.size()).eq(12)
})

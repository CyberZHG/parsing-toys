import { expect, test } from 'vitest'
import { ContextFreeGrammar } from '../../../wasm/index.js'

test('example-1', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> S S + | S S * | a")
    const automaton = cfg.computeLR0Automaton()
    expect(automaton.size()).eq(6)

    const table = cfg.computeLR0ActionGotoTable(automaton)
    const steps = table.parse("a a + a a + *")
    expect(steps.size()).eq(15)
    expect(steps.getStack(0)).toStrictEqual([0])
    expect(steps.getSymbols(0)).toStrictEqual([])
    expect(steps.getRemainingInputs(0)).toStrictEqual(["a", "a", "+", "a", "a", "+", "*", "¥"])
    expect(steps.getAction(0)).eq("shift 2")
    expect(steps.getAction(14)).eq("accept")
})

test('example-2', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("E -> E + T | T  T -> T * F | F  F -> ( E ) | id")
    const automaton = cfg.computeLR0Automaton()
    expect(automaton.size()).eq(12)

    const table = cfg.computeLR0ActionGotoTable(automaton)
    const steps = table.parse("id * ( id + id )")
    expect(steps.size()).eq(4)
})

import { expect, test } from 'vitest'
import { ContextFreeGrammar } from '../../../wasm/index.js'

test('example-1: epsilon productions', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> A B\nA -> a | ε\nB -> b | ε")
    expect(cfg.isChomskyNormalForm()).toBe(false)
    cfg.toChomskyNormalForm()
    expect(cfg.isChomskyNormalForm()).toBe(true)
})

test('example-2: start on RHS', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> a S b | a b")
    expect(cfg.isChomskyNormalForm()).toBe(false)
    cfg.toChomskyNormalForm()
    expect(cfg.isChomskyNormalForm()).toBe(true)
})

test('example-3: arithmetic expression', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("E -> E + T | T\nT -> T * F | F\nF -> ( E ) | id")
    expect(cfg.isChomskyNormalForm()).toBe(false)
    cfg.toChomskyNormalForm()
    expect(cfg.isChomskyNormalForm()).toBe(true)
})

test('example-4: unit productions', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> A\nA -> B\nB -> a")
    expect(cfg.isChomskyNormalForm()).toBe(false)
    cfg.toChomskyNormalForm()
    expect(cfg.isChomskyNormalForm()).toBe(true)
})

test('already CNF: single terminal', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> a")
    expect(cfg.isChomskyNormalForm()).toBe(true)
})

test('already CNF: two non-terminals', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> A B\nA -> a\nB -> b")
    expect(cfg.isChomskyNormalForm()).toBe(true)
})

test('already CNF: start epsilon', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> ε")
    expect(cfg.isChomskyNormalForm()).toBe(true)
})

test('not CNF: terminal in long production', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> a B\nB -> b")
    expect(cfg.isChomskyNormalForm()).toBe(false)
    cfg.toChomskyNormalForm()
    expect(cfg.isChomskyNormalForm()).toBe(true)
})

test('not CNF: three symbols', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> A B C\nA -> a\nB -> b\nC -> c")
    expect(cfg.isChomskyNormalForm()).toBe(false)
    cfg.toChomskyNormalForm()
    expect(cfg.isChomskyNormalForm()).toBe(true)
})

import { expect, test } from 'vitest'
import { ContextFreeGrammar } from '../../../wasm/index.js'

test('example-1', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> i E t S | i E t S e S | a  E -> b")
    cfg.leftFactoring(true)
    expect(cfg.toString()).toBe(` S -> a
    | i E t S S'
S' -> e S
    | ε
 E -> b
`)
})

test('example-2', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> S S + | S S * | a")
    cfg.leftFactoring(true)
    expect(cfg.toString()).toBe(` S -> a
    | S S S'
S' -> *
    | +
`)
})

test('example-3', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> S + S | S S | ( S ) | S * | a")
    cfg.leftFactoring(true)
    expect(cfg.toString()).toBe(` S -> ( S )
    | a
    | S S'
S' -> *
    | + S
    | S
`)
})

test('example-4', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("A -> id | B | a  B -> C  C -> D  D -> id b")
    cfg.leftFactoring(true)
    expect(cfg.toString()).toBe(` A -> a
    | id A'
A' -> b
    | ε
 B -> C
 C -> D
 D -> id b
`)
})

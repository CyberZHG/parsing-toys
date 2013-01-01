import { expect, test } from 'vitest'
import { ContextFreeGrammar } from '../../../wasm/index.js'

test('example-1', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> S S + | S S * | a")
    expect(cfg.leftRecursionElimination()).toBeTruthy()
    expect(cfg.toString()).toBe(` S -> a S'
S' -> S + S'
    | S * S'
    | ε
`)
})

test('example-2', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> ( L ) | a  L -> L , S | S")
    expect(cfg.leftRecursionElimination()).toBeTruthy()
    expect(cfg.toString()).toBe(` S -> ( L )
    | a
 L -> ( L ) L'
    | a L'
L' -> , S L'
    | ε
`)
})

test('example-3', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("A -> C | c d  B -> C | c d  C -> A | B | c d")
    expect(cfg.leftRecursionElimination()).toBeTruthy()
    expect(cfg.toString()).toBe(`A -> C
   | c d
B -> C
   | c d
C -> c d
`)
})

test('example-4', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse(`bexpr -> bexpr or bterm | bterm  
    bterm -> bterm and bfactor | bfactor  
    bfactor -> not bfactor | ( bexpr ) | true | false`)
    expect(cfg.leftRecursionElimination()).toBeTruthy()
    expect(cfg.toString()).toBe(`  bexpr -> bterm bexpr'
 bexpr' -> or bterm bexpr'
         | ε
  bterm -> bfactor bterm'
 bterm' -> and bfactor bterm'
         | ε
bfactor -> not bfactor
         | ( bexpr )
         | true
         | false
`)
})

test('example-5', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse(`A -> B  B -> C  C->D  D->A`)
    expect(cfg.leftRecursionElimination()).toBeFalsy()
})

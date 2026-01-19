import { expect, test } from 'vitest'
import { ContextFreeGrammar } from '../../../wasm/index.js'

test('example-1', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("E -> E + T | T  T -> T * F | F  F -> ( E ) | id")
    cfg.leftRecursionElimination()
    cfg.leftFactoring()
    cfg.computeLL1Table()
})

test('example-2', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> i E t S | i E t S e S | a  E -> b")
    cfg.leftRecursionElimination()
    cfg.leftFactoring()
    cfg.computeLL1Table()
})

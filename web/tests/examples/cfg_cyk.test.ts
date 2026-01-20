import { expect, test } from 'vitest'
import { ContextFreeGrammar } from '../../../wasm/index.js'

test('CYK: empty string accepted', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> ε")
    const result = cfg.cykParse("")
    expect(result.isAccepted()).toBe(true)
})

test('CYK: empty string rejected', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> a")
    const result = cfg.cykParse("")
    expect(result.isAccepted()).toBe(false)
})

test('CYK: single terminal accepted', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> a")
    const result = cfg.cykParse("a")
    expect(result.isAccepted()).toBe(true)
    expect(result.size()).toBe(1)
})

test('CYK: single terminal rejected', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> a")
    const result = cfg.cykParse("b")
    expect(result.isAccepted()).toBe(false)
})

test('CYK: two non-terminals', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> A B\nA -> a\nB -> b")
    const result = cfg.cykParse("a b")
    expect(result.isAccepted()).toBe(true)
    expect(result.size()).toBe(2)
})

test('CYK: arithmetic expression after CNF conversion', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("E -> E + T | T\nT -> T * F | F\nF -> ( E ) | id")
    cfg.toChomskyNormalForm()
    const result = cfg.cykParse("id + id * id")
    expect(result.isAccepted()).toBe(true)
})

test('CYK: arithmetic expression rejected', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("E -> E + T | T\nT -> T * F | F\nF -> ( E ) | id")
    cfg.toChomskyNormalForm()
    const result = cfg.cykParse("id + + id")
    expect(result.isAccepted()).toBe(false)
})

test('CYK: epsilon productions', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> A B\nA -> a | ε\nB -> b | ε")
    cfg.toChomskyNormalForm()

    expect(cfg.cykParse("a b").isAccepted()).toBe(true)
    expect(cfg.cykParse("a").isAccepted()).toBe(true)
    expect(cfg.cykParse("b").isAccepted()).toBe(true)
    expect(cfg.cykParse("").isAccepted()).toBe(true)
})

test('CYK: cell contents', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> A B\nA -> a\nB -> b")
    const result = cfg.cykParse("a b")
    expect(result.size()).toBe(2)

    const cell00 = result.getCell(0, 0)
    expect(cell00).toContain("A")

    const cell11 = result.getCell(1, 1)
    expect(cell11).toContain("B")

    const cell01 = result.getCell(0, 1)
    expect(cell01).toContain("S")
})

test('CYK: cell string format', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> A B\nA -> a\nB -> b")
    const result = cfg.cykParse("a b")
    const cellStr = result.getCellString(0, 1, ", ")
    expect(cellStr).toContain("S")
})

test('CYK: parse tree exists on accept', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> a")
    const result = cfg.cykParse("a")
    expect(result.isAccepted()).toBe(true)
    const parseTree = result.getParseTree()
    expect(parseTree).not.toBeNull()
    if (parseTree) {
        const svg = parseTree.toSVG(false)
        expect(svg).toContain("svg")
        parseTree.delete()
    }
})

test('CYK: no parse tree on reject', () => {
    const cfg = new ContextFreeGrammar()
    cfg.parse("S -> a")
    const result = cfg.cykParse("b")
    expect(result.isAccepted()).toBe(false)
    const parseTree = result.getParseTree()
    expect(parseTree).toBeNull()
})

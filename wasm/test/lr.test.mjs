import assert from 'node:assert';
import { describe, it } from 'mocha';
import { ContextFreeGrammar } from '../index.js';

describe('LR(0) Parsing', function () {
    it('should compute LR(0) automaton', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse('S -> a');
        const automaton = cfg.computeLR0Automaton();
        assert.ok(automaton.size() > 0);
    });

    it('should generate automaton SVG', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse('S -> a');
        const automaton = cfg.computeLR0Automaton();
        const svg = automaton.toSVG(false);
        assert.ok(svg.includes('<svg'));
    });

    it('should compute LR(0) ACTION/GOTO table', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse('S -> a');
        const automaton = cfg.computeLR0Automaton();
        const table = cfg.computeLR0ActionGotoTable(automaton);
        assert.ok(table.size() > 0);
    });
});

describe('SLR(1) Parsing', function () {
    it('should compute SLR(1) automaton', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        `);
        const automaton = cfg.computeSLR1Automaton();
        assert.ok(automaton.size() > 0);
    });

    it('should compute SLR(1) ACTION/GOTO table', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        `);
        const automaton = cfg.computeSLR1Automaton();
        const table = cfg.computeSLR1ActionGotoTable(automaton);
        assert.ok(table.size() > 0);
    });

    it('should have no conflict for SLR(1) grammar', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        `);
        const automaton = cfg.computeSLR1Automaton();
        const table = cfg.computeSLR1ActionGotoTable(automaton);
        assert.strictEqual(table.hasConflict(), false);
    });

    it('should parse input string', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        `);
        const automaton = cfg.computeSLR1Automaton();
        const table = cfg.computeSLR1ActionGotoTable(automaton);
        const steps = table.parse('id + id * id');
        assert.ok(steps.size() > 0);
    });

    it('should return parsing steps', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        `);
        const automaton = cfg.computeSLR1Automaton();
        const table = cfg.computeSLR1ActionGotoTable(automaton);
        const steps = table.parse('id + id');

        for (let i = 0; i < steps.size(); i++) {
            const stack = steps.getStack(i);
            const symbols = steps.getSymbols(i);
            const remaining = steps.getRemainingInputs(i);
            const action = steps.getAction(i);
            assert.ok(Array.isArray(stack));
            assert.ok(Array.isArray(symbols));
            assert.ok(Array.isArray(remaining));
            assert.ok(typeof action === 'string');
        }
    });

    it('should generate parse tree', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        `);
        const automaton = cfg.computeSLR1Automaton();
        const table = cfg.computeSLR1ActionGotoTable(automaton);
        table.parse('id + id');

        const parseTree = table.getParseTree();
        if (parseTree) {
            const svg = parseTree.toSVG(false);
            assert.ok(svg.includes('<svg'));
        }
    });
});

describe('LR(1) Parsing', function () {
    it('should compute LR(1) automaton', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> C C
            C -> c C | d
        `);
        const automaton = cfg.computeLR1Automaton();
        assert.ok(automaton.size() > 0);
    });

    it('should compute LR(1) ACTION/GOTO table', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> C C
            C -> c C | d
        `);
        const automaton = cfg.computeLR1Automaton();
        const table = cfg.computeLR1ActionGotoTable(automaton);
        assert.ok(table.size() > 0);
    });

    it('should parse input string', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> C C
            C -> c C | d
        `);
        const automaton = cfg.computeLR1Automaton();
        const table = cfg.computeLR1ActionGotoTable(automaton);
        const steps = table.parse('c d c d');
        assert.ok(steps.size() > 0);
    });
});

describe('LALR(1) Parsing', function () {
    it('should compute LALR(1) automaton', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> C C
            C -> c C | d
        `);
        const automaton = cfg.computeLALR1Automaton();
        assert.ok(automaton.size() > 0);
    });

    it('should compute LALR(1) ACTION/GOTO table', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> C C
            C -> c C | d
        `);
        const automaton = cfg.computeLALR1Automaton();
        const table = cfg.computeLALR1ActionGotoTable(automaton);
        assert.ok(table.size() > 0);
    });

    it('should have fewer or equal states than LR(1)', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> C C
            C -> c C | d
        `);
        const lr1Automaton = cfg.computeLR1Automaton();
        const lalr1Automaton = cfg.computeLALR1Automaton();
        assert.ok(lalr1Automaton.size() <= lr1Automaton.size());
    });

    it('should parse input string', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> C C
            C -> c C | d
        `);
        const automaton = cfg.computeLALR1Automaton();
        const table = cfg.computeLALR1ActionGotoTable(automaton);
        const steps = table.parse('c d c d');
        assert.ok(steps.size() > 0);
    });
});

describe('ActionGotoTable', function () {
    it('should return cell content', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse('S -> a');
        const automaton = cfg.computeSLR1Automaton();
        const table = cfg.computeSLR1ActionGotoTable(automaton);
        const cell = table.getCell(0, 'a', ' / ');
        assert.ok(typeof cell === 'string');
    });

    it('should check conflict at specific cell', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        `);
        const automaton = cfg.computeSLR1Automaton();
        const table = cfg.computeSLR1ActionGotoTable(automaton);
        const terminals = cfg.terminals();

        for (let i = 0; i < table.size(); i++) {
            for (const terminal of terminals) {
                const result = table.hasConflictAt(i, terminal);
                assert.ok(typeof result === 'boolean');
            }
        }
    });
});

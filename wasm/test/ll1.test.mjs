import assert from 'node:assert';
import { describe, it } from 'mocha';
import { ContextFreeGrammar } from '../index.js';

describe('LL(1) Parsing', function () {
    describe('MTable', function () {
        it('should compute LL(1) table', function () {
            const cfg = new ContextFreeGrammar();
            cfg.parse(`
                E -> T E'
                E' -> + T E' | ε
                T -> F T'
                T' -> * F T' | ε
                F -> ( E ) | id
            `);
            const table = cfg.computeLL1Table();
            assert.ok(table.numNonTerminals() > 0);
            assert.ok(table.numTerminals() > 0);
        });

        it('should have no conflict for LL(1) grammar', function () {
            const cfg = new ContextFreeGrammar();
            cfg.parse(`
                E -> T E'
                E' -> + T E' | ε
                T -> F T'
                T' -> * F T' | ε
                F -> ( E ) | id
            `);
            const table = cfg.computeLL1Table();
            assert.strictEqual(table.hasConflict(), false);
        });

        it('should have conflict for non-LL(1) grammar', function () {
            const cfg = new ContextFreeGrammar();
            cfg.parse(`
                S -> a | a b
            `);
            const table = cfg.computeLL1Table();
            assert.strictEqual(table.hasConflict(), true);
        });

        it('should return correct non-terminals', function () {
            const cfg = new ContextFreeGrammar();
            cfg.parse(`
                E -> T E'
                E' -> + T E' | ε
                T -> F T'
                T' -> * F T' | ε
                F -> ( E ) | id
            `);
            const table = cfg.computeLL1Table();
            const nonTerminals = [];
            for (let i = 0; i < table.numNonTerminals(); i++) {
                nonTerminals.push(table.getNonTerminal(i));
            }
            assert.ok(nonTerminals.includes('E'));
            assert.ok(nonTerminals.includes('T'));
            assert.ok(nonTerminals.includes('F'));
        });

        it('should return correct terminals', function () {
            const cfg = new ContextFreeGrammar();
            cfg.parse(`
                E -> T E'
                E' -> + T E' | ε
                T -> F T'
                T' -> * F T' | ε
                F -> ( E ) | id
            `);
            const table = cfg.computeLL1Table();
            const terminals = [];
            for (let i = 0; i < table.numTerminals(); i++) {
                terminals.push(table.getTerminal(i));
            }
            assert.ok(terminals.includes('+'));
            assert.ok(terminals.includes('*'));
            assert.ok(terminals.includes('id'));
        });

        it('should return correct cell content', function () {
            const cfg = new ContextFreeGrammar();
            cfg.parse(`
                E -> T E'
                E' -> + T E' | ε
                T -> F T'
                T' -> * F T' | ε
                F -> ( E ) | id
            `);
            const table = cfg.computeLL1Table();
            const cell = table.getCell('F', 'id', ' / ');
            assert.ok(cell.includes('id'));
        });
    });

    describe('parsing', function () {
        it('should parse input string', function () {
            const cfg = new ContextFreeGrammar();
            cfg.parse(`
                E -> T E'
                E' -> + T E' | ε
                T -> F T'
                T' -> * F T' | ε
                F -> ( E ) | id
            `);
            const table = cfg.computeLL1Table();
            const steps = table.parse('id + id * id');
            assert.ok(steps.size() > 0);
        });

        it('should return parsing steps', function () {
            const cfg = new ContextFreeGrammar();
            cfg.parse(`
                E -> T E'
                E' -> + T E' | ε
                T -> F T'
                T' -> * F T' | ε
                F -> ( E ) | id
            `);
            const table = cfg.computeLL1Table();
            const steps = table.parse('id + id');

            for (let i = 0; i < steps.size(); i++) {
                const stack = steps.getStack(i);
                const remaining = steps.getRemainingInputs(i);
                const action = steps.getAction(i);
                assert.ok(Array.isArray(stack));
                assert.ok(Array.isArray(remaining));
                assert.ok(typeof action === 'string');
            }
        });

        it('should generate parse tree', function () {
            const cfg = new ContextFreeGrammar();
            cfg.parse(`
                E -> T E'
                E' -> + T E' | ε
                T -> F T'
                T' -> * F T' | ε
                F -> ( E ) | id
            `);
            const table = cfg.computeLL1Table();
            table.parse('id + id');

            const parseTree = table.getParseTree();
            if (parseTree) {
                const svg = parseTree.toSVG(false);
                assert.ok(svg.includes('<svg'));
            }
        });
    });
});

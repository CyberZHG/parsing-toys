import assert from 'node:assert';
import { describe, it } from 'mocha';
import { ContextFreeGrammar } from '../index.js';

describe('ContextFreeGrammar', function () {
    describe('parse', function () {
        it('should parse a valid grammar', function () {
            const cfg = new ContextFreeGrammar();
            const result = cfg.parse(`
                E -> E + T | T
                T -> T * F | F
                F -> ( E ) | id
            `);
            assert.strictEqual(result, true);
            assert.strictEqual(cfg.errorMessage(), '');
        });

        it('should fail on invalid grammar', function () {
            const cfg = new ContextFreeGrammar();
            const result = cfg.parse('invalid grammar without arrow');
            assert.strictEqual(result, false);
            assert.notStrictEqual(cfg.errorMessage(), '');
        });
    });

    describe('terminals and nonTerminals', function () {
        it('should return correct terminals', function () {
            const cfg = new ContextFreeGrammar();
            cfg.parse(`
                E -> E + T | T
                T -> T * F | F
                F -> ( E ) | id
            `);
            const terminals = cfg.terminals();
            assert.ok(terminals.includes('+'));
            assert.ok(terminals.includes('*'));
            assert.ok(terminals.includes('('));
            assert.ok(terminals.includes(')'));
            assert.ok(terminals.includes('id'));
        });

        it('should return correct non-terminals', function () {
            const cfg = new ContextFreeGrammar();
            cfg.parse(`
                E -> E + T | T
                T -> T * F | F
                F -> ( E ) | id
            `);
            const nonTerminals = cfg.nonTerminals();
            assert.ok(nonTerminals.includes('E'));
            assert.ok(nonTerminals.includes('T'));
            assert.ok(nonTerminals.includes('F'));
        });
    });

    describe('toString', function () {
        it('should return formatted grammar', function () {
            const cfg = new ContextFreeGrammar();
            cfg.parse('S -> a');
            const output = cfg.toString();
            assert.ok(output.includes('S'));
            assert.ok(output.includes('->'));
            assert.ok(output.includes('a'));
        });
    });
});

describe('Left Factoring', function () {
    it('should perform left factoring', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> a B | a C
            B -> b
            C -> c
        `);
        cfg.leftFactoring(false);
        const output = cfg.toString();
        assert.ok(output.includes('S'));
    });
});

describe('Left Recursion Elimination', function () {
    it('should eliminate left recursion', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> E + T | T
            T -> T * F | F
            F -> id
        `);
        const result = cfg.leftRecursionElimination();
        assert.strictEqual(result, true);
    });

    it('should fail when all productions are left-recursive', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            A -> A a
        `);
        const result = cfg.leftRecursionElimination();
        assert.strictEqual(result, false);
    });
});

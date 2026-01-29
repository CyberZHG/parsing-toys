import assert from 'node:assert';
import { describe, it } from 'mocha';
import { ContextFreeGrammar } from '../index.js';

describe('FirstAndFollowSet', function () {
    it('should compute first and follow sets', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> T E'
            E' -> + T E' | ε
            T -> F T'
            T' -> * F T' | ε
            F -> ( E ) | id
        `);
        const ff = cfg.computeFirstAndFollowSet();
        assert.ok(ff.size() > 0);
    });

    it('should return correct symbols', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> T E'
            E' -> + T E' | ε
            T -> F T'
            T' -> * F T' | ε
            F -> ( E ) | id
        `);
        const ff = cfg.computeFirstAndFollowSet();
        const symbols = [];
        for (let i = 0; i < ff.size(); i++) {
            symbols.push(ff.symbolAt(i));
        }
        assert.ok(symbols.includes('E'));
        assert.ok(symbols.includes('T'));
        assert.ok(symbols.includes('F'));
    });

    it('should compute correct FIRST sets', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> T E'
            E' -> + T E' | ε
            T -> F T'
            T' -> * F T' | ε
            F -> ( E ) | id
        `);
        const ff = cfg.computeFirstAndFollowSet();
        const firstE = ff.getFirstSet('E');
        assert.ok(firstE.includes('('));
        assert.ok(firstE.includes('id'));

        const firstF = ff.getFirstSet('F');
        assert.ok(firstF.includes('('));
        assert.ok(firstF.includes('id'));
    });

    it('should compute correct FOLLOW sets', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> T E'
            E' -> + T E' | ε
            T -> F T'
            T' -> * F T' | ε
            F -> ( E ) | id
        `);
        const ff = cfg.computeFirstAndFollowSet();
        const followE = ff.getFollowSet('E');
        assert.ok(followE.includes(')') || followE.includes('¥'));
    });

    it('should compute correct nullable', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> T E'
            E' -> + T E' | ε
            T -> F T'
            T' -> * F T' | ε
            F -> ( E ) | id
        `);
        const ff = cfg.computeFirstAndFollowSet();
        assert.strictEqual(ff.getNullable("E'"), true);
        assert.strictEqual(ff.getNullable("T'"), true);
        assert.strictEqual(ff.getNullable('E'), false);
        assert.strictEqual(ff.getNullable('F'), false);
    });
});

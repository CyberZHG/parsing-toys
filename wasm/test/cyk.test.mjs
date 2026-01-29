import assert from 'node:assert';
import { describe, it } from 'mocha';
import { ContextFreeGrammar } from '../index.js';

describe('Chomsky Normal Form', function () {
    it('should detect non-CNF grammar', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> A B C
            A -> a
            B -> b
            C -> c
        `);
        assert.strictEqual(cfg.isChomskyNormalForm(), false);
    });

    it('should detect CNF grammar', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> A B
            A -> a
            B -> b
        `);
        assert.strictEqual(cfg.isChomskyNormalForm(), true);
    });

    it('should convert to CNF', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> A B C
            A -> a
            B -> b
            C -> c
        `);
        cfg.toChomskyNormalForm();
        assert.strictEqual(cfg.isChomskyNormalForm(), true);
    });

    it('should convert complex grammar to CNF', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        `);
        cfg.toChomskyNormalForm();
        assert.strictEqual(cfg.isChomskyNormalForm(), true);
    });
});

describe('CYK Parsing', function () {
    it('should accept valid input', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> A B
            A -> a
            B -> b
        `);
        cfg.toChomskyNormalForm();
        const result = cfg.cykParse('a b');
        assert.strictEqual(result.isAccepted(), true);
    });

    it('should reject invalid input', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> A B
            A -> a
            B -> b
        `);
        cfg.toChomskyNormalForm();
        const result = cfg.cykParse('a a');
        assert.strictEqual(result.isAccepted(), false);
    });

    it('should return correct table size', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> A B
            A -> a
            B -> b
        `);
        cfg.toChomskyNormalForm();
        const result = cfg.cykParse('a b');
        assert.strictEqual(result.size(), 2);
    });

    it('should return cell content', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> A B
            A -> a
            B -> b
        `);
        cfg.toChomskyNormalForm();
        const result = cfg.cykParse('a b');
        const cell = result.getCell(0, 0);
        assert.ok(Array.isArray(cell));
    });

    it('should return cell string', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> A B
            A -> a
            B -> b
        `);
        cfg.toChomskyNormalForm();
        const result = cfg.cykParse('a b');
        const cellStr = result.getCellString(0, 0, ', ');
        assert.ok(typeof cellStr === 'string');
    });

    it('should generate parse tree', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> A B
            A -> a
            B -> b
        `);
        cfg.toChomskyNormalForm();
        const result = cfg.cykParse('a b');

        assert.strictEqual(result.isAccepted(), true);
        const parseTree = result.getParseTree();
        if (parseTree) {
            const svg = parseTree.toSVG(false);
            assert.ok(svg.includes('<svg'));
        }
    });

    it('should handle complex grammar', function () {
        const cfg = new ContextFreeGrammar();
        cfg.parse(`
            S -> A B | B C
            A -> B A | a
            B -> C C | b
            C -> A B | a
        `);
        cfg.toChomskyNormalForm();
        const result = cfg.cykParse('b a a b a');
        assert.ok(typeof result.isAccepted() === 'boolean');
    });
});

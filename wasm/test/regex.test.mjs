import assert from 'node:assert';
import { describe, it } from 'mocha';
import { RegularExpression } from '../index.js';

describe('RegularExpression', function () {
    describe('parse', function () {
        it('should parse valid regex', function () {
            const re = new RegularExpression();
            const result = re.parse('(a|b)*abb');
            assert.strictEqual(result, true);
            assert.strictEqual(re.errorMessage(), '');
        });

        it('should fail on invalid regex', function () {
            const re = new RegularExpression();
            const result = re.parse('(a|b');
            assert.strictEqual(result, false);
            assert.notStrictEqual(re.errorMessage(), '');
        });
    });
});

describe('NFA', function () {
    it('should convert to NFA', function () {
        const re = new RegularExpression();
        re.parse('(a|b)*abb');
        const nfa = re.toNFA();
        assert.ok(nfa !== null);
    });

    it('should create NFA graph', function () {
        const re = new RegularExpression();
        re.parse('(a|b)*abb');
        const nfa = re.toNFA();
        const nfaGraph = RegularExpression.toNFAGraph(nfa);
        assert.ok(nfaGraph.size() > 0);
        assert.ok(nfaGraph.numEdges() > 0);
    });

    it('should return NFA states', function () {
        const re = new RegularExpression();
        re.parse('a b');
        const nfa = re.toNFA();
        const nfaGraph = RegularExpression.toNFAGraph(nfa);

        for (let i = 0; i < nfaGraph.size(); i++) {
            const state = nfaGraph.stateAt(i);
            assert.ok(typeof state === 'string');
        }
    });

    it('should return NFA edges', function () {
        const re = new RegularExpression();
        re.parse('a b');
        const nfa = re.toNFA();
        const nfaGraph = RegularExpression.toNFAGraph(nfa);

        for (let i = 0; i < nfaGraph.numEdges(); i++) {
            const from = nfaGraph.edgeFrom(i);
            const to = nfaGraph.edgeTo(i);
            const label = nfaGraph.edgeLabel(i);
            assert.ok(typeof from === 'number');
            assert.ok(typeof to === 'number');
            assert.ok(typeof label === 'string');
        }
    });

    it('should generate NFA SVG', function () {
        const re = new RegularExpression();
        re.parse('(a|b)*abb');
        const nfa = re.toNFA();
        const nfaGraph = RegularExpression.toNFAGraph(nfa);
        const svg = nfaGraph.toSVG(false);
        assert.ok(svg.includes('<svg'));
    });

    it('should generate NFA SVG in dark mode', function () {
        const re = new RegularExpression();
        re.parse('a|b');
        const nfa = re.toNFA();
        const nfaGraph = RegularExpression.toNFAGraph(nfa);
        const svg = nfaGraph.toSVG(true);
        assert.ok(svg.includes('<svg'));
    });
});

describe('DFA', function () {
    it('should convert to DFA', function () {
        const re = new RegularExpression();
        re.parse('(a|b)*abb');
        const nfa = re.toNFA();
        const dfa = RegularExpression.toDFA(nfa);
        assert.ok(dfa !== null);
    });

    it('should create DFA graph', function () {
        const re = new RegularExpression();
        re.parse('(a|b)*abb');
        const nfa = re.toNFA();
        const dfa = RegularExpression.toDFA(nfa);
        const dfaGraph = RegularExpression.toDFAGraph(dfa);
        assert.ok(dfaGraph.size() > 0);
    });

    it('should return DFA states', function () {
        const re = new RegularExpression();
        re.parse('a|b');
        const nfa = re.toNFA();
        const dfa = RegularExpression.toDFA(nfa);
        const dfaGraph = RegularExpression.toDFAGraph(dfa);

        for (let i = 0; i < dfaGraph.size(); i++) {
            const stateId = dfaGraph.stateIdAt(i);
            const stateKey = dfaGraph.stateKeyAt(i);
            const stateType = dfaGraph.stateTypeAt(i);
            assert.ok(typeof stateId === 'string');
            assert.ok(typeof stateKey === 'string');
            assert.ok(typeof stateType === 'string');
        }
    });

    it('should return DFA edges', function () {
        const re = new RegularExpression();
        re.parse('a|b');
        const nfa = re.toNFA();
        const dfa = RegularExpression.toDFA(nfa);
        const dfaGraph = RegularExpression.toDFAGraph(dfa);

        for (let i = 0; i < dfaGraph.numEdges(); i++) {
            const from = dfaGraph.edgeFrom(i);
            const to = dfaGraph.edgeTo(i);
            const label = dfaGraph.edgeLabel(i);
            assert.ok(typeof from === 'number');
            assert.ok(typeof to === 'number');
            assert.ok(typeof label === 'string');
        }
    });

    it('should generate DFA SVG', function () {
        const re = new RegularExpression();
        re.parse('(a|b)*abb');
        const nfa = re.toNFA();
        const dfa = RegularExpression.toDFA(nfa);
        const dfaGraph = RegularExpression.toDFAGraph(dfa);
        const svg = dfaGraph.toSVG(false);
        assert.ok(svg.includes('<svg'));
    });
});

describe('Minimized DFA', function () {
    it('should minimize DFA', function () {
        const re = new RegularExpression();
        re.parse('(a|b)*abb');
        const nfa = re.toNFA();
        const dfa = RegularExpression.toDFA(nfa);
        const minDfa = RegularExpression.toMinDFA(dfa);
        assert.ok(minDfa !== null);
    });

    it('should have fewer or equal states', function () {
        const re = new RegularExpression();
        re.parse('(a|b)*abb');
        const nfa = re.toNFA();
        const dfa = RegularExpression.toDFA(nfa);
        const minDfa = RegularExpression.toMinDFA(dfa);

        const dfaGraph = RegularExpression.toDFAGraph(dfa);
        const minDfaGraph = RegularExpression.toDFAGraph(minDfa);

        assert.ok(minDfaGraph.size() <= dfaGraph.size());
    });

    it('should generate minimized DFA SVG', function () {
        const re = new RegularExpression();
        re.parse('(a|b)*abb');
        const nfa = re.toNFA();
        const dfa = RegularExpression.toDFA(nfa);
        const minDfa = RegularExpression.toMinDFA(dfa);
        const minDfaGraph = RegularExpression.toDFAGraph(minDfa);
        const svg = minDfaGraph.toSVG(false);
        assert.ok(svg.includes('<svg'));
    });
});

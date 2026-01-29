# Parsing Toys

WebAssembly bindings for Parsing Toys - an educational tool for visualizing Context-Free Grammar (CFG) parsing algorithms.

## Installation

```bash
npm install parsing-toys
```

## Usage

### Context-Free Grammar

```javascript
import { ContextFreeGrammar } from 'parsing-toys';

// Parse a grammar
const cfg = new ContextFreeGrammar();
cfg.parse(`
    E -> E + T | T
    T -> T * F | F
    F -> ( E ) | id
`);

console.log(cfg.toString());

// Get terminals and non-terminals
console.log('Terminals:', cfg.terminals());
console.log('Non-terminals:', cfg.nonTerminals());
```

### Left Factoring & Left Recursion Elimination

```javascript
import { ContextFreeGrammar } from 'parsing-toys';

const cfg = new ContextFreeGrammar();
cfg.parse(`
    S -> a B | a C
    B -> b
    C -> c
`);

cfg.leftFactoring(false);
console.log('After left factoring:');
console.log(cfg.toString());
```

### FIRST and FOLLOW Sets

```javascript
import { ContextFreeGrammar } from 'parsing-toys';

const cfg = new ContextFreeGrammar();
cfg.parse(`
    E -> T E'
    E' -> + T E' | ε
    T -> F T'
    T' -> * F T' | ε
    F -> ( E ) | id
`);

const ff = cfg.computeFirstAndFollowSet();
for (let i = 0; i < ff.size(); i++) {
    const symbol = ff.symbolAt(i);
    console.log(`${symbol}:`);
    console.log(`  FIRST: ${ff.getFirstSet(symbol)}`);
    console.log(`  FOLLOW: ${ff.getFollowSet(symbol)}`);
}
```

### LL(1) Parsing

```javascript
import { ContextFreeGrammar } from 'parsing-toys';

const cfg = new ContextFreeGrammar();
cfg.parse(`
    E -> T E'
    E' -> + T E' | ε
    T -> F T'
    T' -> * F T' | ε
    F -> ( E ) | id
`);

const table = cfg.computeLL1Table();
console.log('Has conflict:', table.hasConflict());

const steps = table.parse('id + id * id');
for (let i = 0; i < steps.size(); i++) {
    console.log(steps.getAction(i));
}
```

### LR Parsing (LR(0), SLR(1), LR(1), LALR(1))

```javascript
import { ContextFreeGrammar } from 'parsing-toys';

const cfg = new ContextFreeGrammar();
cfg.parse(`
    E -> E + T | T
    T -> T * F | F
    F -> ( E ) | id
`);

// SLR(1) parsing
const automaton = cfg.computeSLR1Automaton();
const table = cfg.computeSLR1ActionGotoTable(automaton);

console.log('Has conflict:', table.hasConflict());

const steps = table.parse('id + id * id');
for (let i = 0; i < steps.size(); i++) {
    console.log(steps.getAction(i));
}

// Get parse tree SVG
const parseTree = table.getParseTree();
if (parseTree) {
    const svg = parseTree.toSVG();
    console.log(svg);
}
```

### CYK Parsing

```javascript
import { ContextFreeGrammar } from 'parsing-toys';

const cfg = new ContextFreeGrammar();
cfg.parse(`
    S -> A B
    A -> a
    B -> b
`);

// Convert to Chomsky Normal Form
cfg.toChomskyNormalForm();
console.log('Is CNF:', cfg.isChomskyNormalForm());

// Parse using CYK algorithm
const result = cfg.cykParse('a b');
console.log('Accepted:', result.isAccepted());
```

### Regular Expressions (NFA/DFA)

```javascript
import { RegularExpression } from 'parsing-toys';

const re = new RegularExpression();
re.parse('(a|b)*abb');

// Convert to NFA
const nfa = re.toNFA();
const nfaGraph = RegularExpression.toNFAGraph(nfa);
console.log('NFA states:', nfaGraph.size());

// Convert to DFA
const dfa = RegularExpression.toDFA(nfa);
const dfaGraph = RegularExpression.toDFAGraph(dfa);
console.log('DFA states:', dfaGraph.size());

// Minimize DFA
const minDfa = RegularExpression.toMinDFA(dfa);
const minDfaGraph = RegularExpression.toDFAGraph(minDfa);
console.log('Minimized DFA states:', minDfaGraph.size());

// Generate SVG visualization
const svg = nfaGraph.toSVG();
console.log(svg);
```

## API Reference

### ContextFreeGrammar

- `parse(code: string): boolean` - Parse a grammar string
- `errorMessage(): string` - Get error message if parsing failed
- `toString(): string` - Get formatted grammar string
- `terminals(): string[]` - Get all terminals
- `nonTerminals(): string[]` - Get all non-terminals
- `leftFactoring(expand: boolean): void` - Perform left factoring
- `leftRecursionElimination(): boolean` - Eliminate left recursion
- `computeFirstAndFollowSet(): FirstAndFollowSet` - Compute FIRST and FOLLOW sets
- `computeLL1Table(): MTable` - Compute LL(1) parsing table
- `computeLR0Automaton(): FiniteAutomaton` - Compute LR(0) automaton
- `computeLR0ActionGotoTable(automaton): ActionGotoTable` - Compute LR(0) ACTION/GOTO table
- `computeSLR1Automaton(): FiniteAutomaton` - Compute SLR(1) automaton
- `computeSLR1ActionGotoTable(automaton): ActionGotoTable` - Compute SLR(1) ACTION/GOTO table
- `computeLR1Automaton(): FiniteAutomaton` - Compute LR(1) automaton
- `computeLR1ActionGotoTable(automaton): ActionGotoTable` - Compute LR(1) ACTION/GOTO table
- `computeLALR1Automaton(): FiniteAutomaton` - Compute LALR(1) automaton
- `computeLALR1ActionGotoTable(automaton): ActionGotoTable` - Compute LALR(1) ACTION/GOTO table
- `toChomskyNormalForm(): void` - Convert grammar to CNF
- `isChomskyNormalForm(): boolean` - Check if grammar is in CNF
- `cykParse(input: string): CYKTable` - Parse using CYK algorithm

### RegularExpression

- `parse(pattern: string): boolean` - Parse a regular expression
- `errorMessage(): string` - Get error message if parsing failed
- `toNFA(): NFAState` - Convert to NFA
- `static toDFA(nfa: NFAState): DFAState` - Convert NFA to DFA
- `static toMinDFA(dfa: DFAState): DFAState` - Minimize DFA
- `static toNFAGraph(nfa: NFAState): NFAGraph` - Convert NFA to graph
- `static toDFAGraph(dfa: DFAState): DFAGraph` - Convert DFA to graph

## License

MIT License

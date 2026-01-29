# Parsing Toys

Python bindings for Parsing Toys - an educational tool for visualizing Context-Free Grammar (CFG) parsing algorithms.

## Installation

```bash
pip install sp-parsing-toys
```

## Usage

### Context-Free Grammar

```python
from parsing_toys import ContextFreeGrammar

# Parse a grammar
cfg = ContextFreeGrammar()
cfg.parse("""
    E -> E + T | T
    T -> T * F | F
    F -> ( E ) | id
""")

print(cfg)

# Get terminals and non-terminals
print("Terminals:", cfg.terminals())
print("Non-terminals:", cfg.non_terminals())
```

### Left Factoring & Left Recursion Elimination

```python
from parsing_toys import ContextFreeGrammar

cfg = ContextFreeGrammar()
cfg.parse("""
    S -> a B | a C
    B -> b
    C -> c
""")

cfg.left_factoring()
print("After left factoring:")
print(cfg)
```

### FIRST and FOLLOW Sets

```python
from parsing_toys import ContextFreeGrammar

cfg = ContextFreeGrammar()
cfg.parse("""
    E -> T E'
    E' -> + T E' | ε
    T -> F T'
    T' -> * F T' | ε
    F -> ( E ) | id
""")

ff = cfg.compute_first_and_follow_set()
for i in range(ff.size()):
    symbol = ff.symbol_at(i)
    print(f"{symbol}:")
    print(f"  FIRST: {ff.get_first_set(symbol)}")
    print(f"  FOLLOW: {ff.get_follow_set(symbol)}")
```

### LL(1) Parsing

```python
from parsing_toys import ContextFreeGrammar

cfg = ContextFreeGrammar()
cfg.parse("""
    E -> T E'
    E' -> + T E' | ε
    T -> F T'
    T' -> * F T' | ε
    F -> ( E ) | id
""")

table = cfg.compute_ll1_table()
print("Has conflict:", table.has_conflict())

steps = table.parse("id + id * id")
print(steps)
```

### LR Parsing (LR(0), SLR(1), LR(1), LALR(1))

```python
from parsing_toys import ContextFreeGrammar

cfg = ContextFreeGrammar()
cfg.parse("""
    E -> E + T | T
    T -> T * F | F
    F -> ( E ) | id
""")

# SLR(1) parsing
automaton = cfg.compute_slr1_automaton()
table = cfg.compute_slr1_action_goto_table(automaton)

print("Has conflict:", table.has_conflict())

steps = table.parse("id + id * id")
print(steps)

# Get parse tree
if table.parse_tree:
    svg = table.parse_tree.to_svg()
```

### CYK Parsing

```python
from parsing_toys import ContextFreeGrammar

cfg = ContextFreeGrammar()
cfg.parse("""
    S -> A B
    A -> a
    B -> b
""")

# Convert to Chomsky Normal Form
cfg.to_chomsky_normal_form()
print("Is CNF:", cfg.is_chomsky_normal_form())

# Parse using CYK algorithm
result = cfg.cyk_parse("a b")
print("Accepted:", result.accepted)
```

### Regular Expressions (NFA/DFA)

```python
from parsing_toys import RegularExpression

re = RegularExpression()
re.parse("(a|b)*abb")

# Convert to NFA
nfa = re.to_nfa()
nfa_graph = RegularExpression.to_nfa_graph(nfa)
print("NFA states:", nfa_graph.size())

# Convert to DFA
dfa = RegularExpression.to_dfa(nfa)
dfa_graph = RegularExpression.to_dfa_graph(dfa)
print("DFA states:", dfa_graph.size())

# Minimize DFA
min_dfa = RegularExpression.to_min_dfa(dfa)
min_dfa_graph = RegularExpression.to_dfa_graph(min_dfa)
print("Minimized DFA states:", min_dfa_graph.size())

# Generate SVG visualization
svg = nfa_graph.to_svg()
```

## API Reference

### ContextFreeGrammar

- `parse(s: str) -> bool`: Parse a grammar string
- `error_message() -> str`: Get error message if parsing failed
- `terminals() -> List[str]`: Get all terminals
- `non_terminals() -> List[str]`: Get all non-terminals
- `left_factoring(expand: bool = False)`: Perform left factoring
- `left_recursion_elimination() -> bool`: Eliminate left recursion
- `compute_first_and_follow_set() -> FirstAndFollowSet`: Compute FIRST and FOLLOW sets
- `compute_ll1_table() -> MTable`: Compute LL(1) parsing table
- `compute_lr0_automaton() -> FiniteAutomaton`: Compute LR(0) automaton
- `compute_lr0_action_goto_table(automaton) -> ActionGotoTable`: Compute LR(0) ACTION/GOTO table
- `compute_slr1_automaton() -> FiniteAutomaton`: Compute SLR(1) automaton
- `compute_slr1_action_goto_table(automaton) -> ActionGotoTable`: Compute SLR(1) ACTION/GOTO table
- `compute_lr1_automaton() -> FiniteAutomaton`: Compute LR(1) automaton
- `compute_lr1_action_goto_table(automaton) -> ActionGotoTable`: Compute LR(1) ACTION/GOTO table
- `compute_lalr1_automaton() -> FiniteAutomaton`: Compute LALR(1) automaton
- `compute_lalr1_action_goto_table(automaton) -> ActionGotoTable`: Compute LALR(1) ACTION/GOTO table
- `to_chomsky_normal_form()`: Convert grammar to CNF
- `is_chomsky_normal_form() -> bool`: Check if grammar is in CNF
- `cyk_parse(s: str) -> CYKTable`: Parse using CYK algorithm

### RegularExpression

- `parse(pattern: str) -> bool`: Parse a regular expression
- `error_message() -> str`: Get error message if parsing failed
- `to_nfa() -> NFAState`: Convert to NFA
- `to_dfa(nfa: NFAState) -> DFAState`: Convert NFA to DFA (static)
- `to_min_dfa(dfa: DFAState) -> DFAState`: Minimize DFA (static)
- `to_nfa_graph(nfa: NFAState) -> NFAGraph`: Convert NFA to graph (static)
- `to_dfa_graph(dfa: DFAState) -> DFAGraph`: Convert DFA to graph (static)

## License

MIT License

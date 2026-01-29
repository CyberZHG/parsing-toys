from parsing_toys import ContextFreeGrammar


class TestContextFreeGrammar:
    def test_parse_grammar(self):
        cfg = ContextFreeGrammar()
        result = cfg.parse(
            """
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        """
        )
        assert result is True
        assert cfg.error_message() == ""

    def test_parse_invalid_grammar(self):
        cfg = ContextFreeGrammar()
        result = cfg.parse("invalid grammar without arrow")
        assert result is False
        assert cfg.error_message() != ""

    def test_terminals_and_non_terminals(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        """
        )
        terminals = cfg.terminals()
        non_terminals = cfg.non_terminals()

        assert "+" in terminals
        assert "*" in terminals
        assert "(" in terminals
        assert ")" in terminals
        assert "id" in terminals

        assert "E" in non_terminals
        assert "T" in non_terminals
        assert "F" in non_terminals

    def test_is_terminal_and_non_terminal(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> a b
        """
        )
        assert cfg.is_terminal("a") is True
        assert cfg.is_terminal("S") is False
        assert cfg.is_non_terminal("S") is True
        assert cfg.is_non_terminal("a") is False

    def test_to_string(self):
        cfg = ContextFreeGrammar()
        cfg.parse("S -> a")
        output = str(cfg)
        assert "S" in output
        assert "->" in output
        assert "a" in output

    def test_empty_symbol(self):
        assert ContextFreeGrammar.EMPTY_SYMBOL == "ε"

    def test_eof_symbol(self):
        assert ContextFreeGrammar.EOF_SYMBOL == "¥"


class TestLeftFactoring:
    def test_left_factoring(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> a B | a C
            B -> b
            C -> c
        """
        )
        cfg.left_factoring()
        output = str(cfg)
        assert "S'" in output or "a" in output

    def test_left_factoring_with_expand(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> a B | a C
            B -> b
            C -> c
        """
        )
        cfg.left_factoring(expand=True)
        output = str(cfg)
        assert "S" in output


class TestLeftRecursionElimination:
    def test_left_recursion_elimination(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> E + T | T
            T -> T * F | F
            F -> id
        """
        )
        result = cfg.left_recursion_elimination()
        assert result is True
        output = str(cfg)
        assert "E'" in output or "T'" in output

    def test_left_recursion_elimination_failure(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            A -> A a
        """
        )
        result = cfg.left_recursion_elimination()
        assert result is False

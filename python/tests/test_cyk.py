from parsing_toys import ContextFreeGrammar


class TestChomskyNormalForm:
    def test_is_chomsky_normal_form_false(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> A B C
            A -> a
            B -> b
            C -> c
        """
        )
        assert cfg.is_chomsky_normal_form() is False

    def test_is_chomsky_normal_form_true(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> A B
            A -> a
            B -> b
        """
        )
        assert cfg.is_chomsky_normal_form() is True

    def test_to_chomsky_normal_form(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> A B C
            A -> a
            B -> b
            C -> c
        """
        )
        cfg.to_chomsky_normal_form()
        assert cfg.is_chomsky_normal_form() is True

    def test_cnf_complex_grammar(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        """
        )
        cfg.to_chomsky_normal_form()
        assert cfg.is_chomsky_normal_form() is True


class TestCYKParsing:
    def test_cyk_parse_simple(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> A B
            A -> a
            B -> b
        """
        )
        cfg.to_chomsky_normal_form()
        result = cfg.cyk_parse("a b")
        assert result.accepted is True

    def test_cyk_parse_rejected(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> A B
            A -> a
            B -> b
        """
        )
        cfg.to_chomsky_normal_form()
        result = cfg.cyk_parse("a a")
        assert result.accepted is False

    def test_cyk_table_size(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> A B
            A -> a
            B -> b
        """
        )
        cfg.to_chomsky_normal_form()
        result = cfg.cyk_parse("a b")
        assert result.size() == 2

    def test_cyk_get_cell(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> A B
            A -> a
            B -> b
        """
        )
        cfg.to_chomsky_normal_form()
        result = cfg.cyk_parse("a b")

        cell = result.get_cell(0, 0)
        assert isinstance(cell, list)

    def test_cyk_get_cell_string(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> A B
            A -> a
            B -> b
        """
        )
        cfg.to_chomsky_normal_form()
        result = cfg.cyk_parse("a b")

        cell_str = result.get_cell_string(0, 0)
        assert isinstance(cell_str, str)

    def test_cyk_parse_tree(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> A B
            A -> a
            B -> b
        """
        )
        cfg.to_chomsky_normal_form()
        result = cfg.cyk_parse("a b")

        assert result.accepted is True
        if result.parse_tree:
            assert result.parse_tree.size() > 0
            svg = result.parse_tree.to_svg()
            assert "<svg" in svg

    def test_cyk_complex_grammar(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> A B | B C
            A -> B A | a
            B -> C C | b
            C -> A B | a
        """
        )
        cfg.to_chomsky_normal_form()
        result = cfg.cyk_parse("b a a b a")
        assert isinstance(result.accepted, bool)

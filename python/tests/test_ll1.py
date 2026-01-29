from parsing_toys import ContextFreeGrammar


class TestLL1Parsing:
    def test_compute_ll1_table(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> T E'
            E' -> + T E' | ε
            T -> F T'
            T' -> * F T' | ε
            F -> ( E ) | id
        """
        )
        table = cfg.compute_ll1_table()

        assert table.num_non_terminals() > 0
        assert table.num_terminals() > 0

    def test_ll1_table_no_conflict(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> T E'
            E' -> + T E' | ε
            T -> F T'
            T' -> * F T' | ε
            F -> ( E ) | id
        """
        )
        table = cfg.compute_ll1_table()
        assert table.has_conflict() is False

    def test_ll1_table_with_conflict(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> a | a b
        """
        )
        table = cfg.compute_ll1_table()
        assert table.has_conflict() is True

    def test_ll1_get_non_terminal(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> T E'
            E' -> + T E' | ε
            T -> F T'
            T' -> * F T' | ε
            F -> ( E ) | id
        """
        )
        table = cfg.compute_ll1_table()

        non_terminals = [
            table.get_non_terminal(i) for i in range(table.num_non_terminals())
        ]
        assert "E" in non_terminals
        assert "T" in non_terminals
        assert "F" in non_terminals

    def test_ll1_get_terminal(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> T E'
            E' -> + T E' | ε
            T -> F T'
            T' -> * F T' | ε
            F -> ( E ) | id
        """
        )
        table = cfg.compute_ll1_table()

        terminals = [table.get_terminal(i) for i in range(table.num_terminals())]
        assert "+" in terminals
        assert "*" in terminals
        assert "id" in terminals

    def test_ll1_get_cell(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> T E'
            E' -> + T E' | ε
            T -> F T'
            T' -> * F T' | ε
            F -> ( E ) | id
        """
        )
        table = cfg.compute_ll1_table()

        cell = table.get_cell("F", "id")
        assert "id" in cell

    def test_ll1_parse(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> T E'
            E' -> + T E' | ε
            T -> F T'
            T' -> * F T' | ε
            F -> ( E ) | id
        """
        )
        table = cfg.compute_ll1_table()
        steps = table.parse("id + id * id")

        assert steps.size() > 0

    def test_ll1_parsing_steps(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> T E'
            E' -> + T E' | ε
            T -> F T'
            T' -> * F T' | ε
            F -> ( E ) | id
        """
        )
        table = cfg.compute_ll1_table()
        steps = table.parse("id + id")

        for i in range(steps.size()):
            stack = steps.get_stack(i)
            remaining = steps.get_remaining_inputs(i)
            action = steps.get_action(i)
            assert isinstance(stack, list)
            assert isinstance(remaining, list)
            assert isinstance(action, str)

    def test_ll1_parse_tree(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> T E'
            E' -> + T E' | ε
            T -> F T'
            T' -> * F T' | ε
            F -> ( E ) | id
        """
        )
        table = cfg.compute_ll1_table()
        table.parse("id + id")

        if table.parse_tree:
            assert table.parse_tree.size() > 0
            svg = table.parse_tree.to_svg()
            assert "<svg" in svg

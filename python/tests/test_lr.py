from parsing_toys import ContextFreeGrammar


class TestLR0Parsing:
    def test_compute_lr0_automaton(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> a
        """
        )
        automaton = cfg.compute_lr0_automaton()
        assert automaton.size() > 0

    def test_lr0_automaton_to_svg(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> a
        """
        )
        automaton = cfg.compute_lr0_automaton()
        svg = automaton.to_svg()
        assert "<svg" in svg

    def test_compute_lr0_action_goto_table(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> a
        """
        )
        automaton = cfg.compute_lr0_automaton()
        table = cfg.compute_lr0_action_goto_table(automaton)
        assert table.size() > 0


class TestSLR1Parsing:
    def test_compute_slr1_automaton(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        """
        )
        automaton = cfg.compute_slr1_automaton()
        assert automaton.size() > 0

    def test_compute_slr1_action_goto_table(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        """
        )
        automaton = cfg.compute_slr1_automaton()
        table = cfg.compute_slr1_action_goto_table(automaton)
        assert table.size() > 0

    def test_slr1_has_conflict(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        """
        )
        automaton = cfg.compute_slr1_automaton()
        table = cfg.compute_slr1_action_goto_table(automaton)
        assert table.has_conflict() is False

    def test_slr1_parse(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        """
        )
        automaton = cfg.compute_slr1_automaton()
        table = cfg.compute_slr1_action_goto_table(automaton)
        steps = table.parse("id + id * id")
        assert steps.size() > 0

    def test_slr1_parsing_steps(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        """
        )
        automaton = cfg.compute_slr1_automaton()
        table = cfg.compute_slr1_action_goto_table(automaton)
        steps = table.parse("id + id")

        for i in range(steps.size()):
            stack = steps.get_stack(i)
            symbols = steps.get_symbols(i)
            remaining = steps.get_remaining_inputs(i)
            action = steps.get_action(i)
            assert isinstance(stack, list)
            assert isinstance(symbols, list)
            assert isinstance(remaining, list)
            assert isinstance(action, str)

    def test_slr1_parse_tree(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        """
        )
        automaton = cfg.compute_slr1_automaton()
        table = cfg.compute_slr1_action_goto_table(automaton)
        table.parse("id + id")

        if table.parse_tree:
            assert table.parse_tree.size() > 0
            svg = table.parse_tree.to_svg()
            assert "<svg" in svg


class TestLR1Parsing:
    def test_compute_lr1_automaton(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> C C
            C -> c C | d
        """
        )
        automaton = cfg.compute_lr1_automaton()
        assert automaton.size() > 0

    def test_compute_lr1_action_goto_table(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> C C
            C -> c C | d
        """
        )
        automaton = cfg.compute_lr1_automaton()
        table = cfg.compute_lr1_action_goto_table(automaton)
        assert table.size() > 0

    def test_lr1_parse(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> C C
            C -> c C | d
        """
        )
        automaton = cfg.compute_lr1_automaton()
        table = cfg.compute_lr1_action_goto_table(automaton)
        steps = table.parse("c d c d")
        assert steps.size() > 0


class TestLALR1Parsing:
    def test_compute_lalr1_automaton(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> C C
            C -> c C | d
        """
        )
        automaton = cfg.compute_lalr1_automaton()
        assert automaton.size() > 0

    def test_compute_lalr1_action_goto_table(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> C C
            C -> c C | d
        """
        )
        automaton = cfg.compute_lalr1_automaton()
        table = cfg.compute_lalr1_action_goto_table(automaton)
        assert table.size() > 0

    def test_lalr1_fewer_states_than_lr1(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> C C
            C -> c C | d
        """
        )
        lr1_automaton = cfg.compute_lr1_automaton()
        lalr1_automaton = cfg.compute_lalr1_automaton()
        assert lalr1_automaton.size() <= lr1_automaton.size()

    def test_lalr1_parse(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> C C
            C -> c C | d
        """
        )
        automaton = cfg.compute_lalr1_automaton()
        table = cfg.compute_lalr1_action_goto_table(automaton)
        steps = table.parse("c d c d")
        assert steps.size() > 0


class TestActionGotoTable:
    def test_get_cell(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            S -> a
        """
        )
        automaton = cfg.compute_slr1_automaton()
        table = cfg.compute_slr1_action_goto_table(automaton)

        cell = table.get_cell(0, "a")
        assert isinstance(cell, str)

    def test_has_conflict_at(self):
        cfg = ContextFreeGrammar()
        cfg.parse(
            """
            E -> E + T | T
            T -> T * F | F
            F -> ( E ) | id
        """
        )
        automaton = cfg.compute_slr1_automaton()
        table = cfg.compute_slr1_action_goto_table(automaton)

        for i in range(table.size()):
            for terminal in cfg.terminals():
                result = table.has_conflict_at(i, terminal)
                assert isinstance(result, bool)

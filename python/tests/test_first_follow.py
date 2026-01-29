from parsing_toys import ContextFreeGrammar


class TestFirstAndFollowSet:
    def test_compute_first_and_follow_set(self):
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
        ff = cfg.compute_first_and_follow_set()

        assert ff.size() > 0

    def test_symbol_at(self):
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
        ff = cfg.compute_first_and_follow_set()

        symbols = [ff.symbol_at(i) for i in range(ff.size())]
        assert "E" in symbols
        assert "T" in symbols
        assert "F" in symbols

    def test_get_first_set(self):
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
        ff = cfg.compute_first_and_follow_set()

        first_e = ff.get_first_set("E")
        assert "(" in first_e
        assert "id" in first_e

        first_f = ff.get_first_set("F")
        assert "(" in first_f
        assert "id" in first_f

    def test_get_follow_set(self):
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
        ff = cfg.compute_first_and_follow_set()

        follow_e = ff.get_follow_set("E")
        assert ")" in follow_e or ContextFreeGrammar.EOF_SYMBOL in follow_e

    def test_get_nullable(self):
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
        ff = cfg.compute_first_and_follow_set()

        assert ff.get_nullable("E'") is True
        assert ff.get_nullable("T'") is True
        assert ff.get_nullable("E") is False
        assert ff.get_nullable("F") is False

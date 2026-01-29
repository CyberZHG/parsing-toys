from parsing_toys import RegularExpression


class TestRegularExpression:
    def test_parse_regex(self):
        re = RegularExpression()
        result = re.parse("(a|b)*abb")
        assert result is True
        assert re.error_message() == ""

    def test_parse_regex_with_constructor(self):
        re = RegularExpression("(a|b)*abb")
        assert re.error_message() == ""

    def test_parse_invalid_regex(self):
        re = RegularExpression()
        result = re.parse("(a|b")
        assert result is False
        assert re.error_message() != ""

    def test_epsilon_constant(self):
        assert RegularExpression.EPSILON == "ε"


class TestNFA:
    def test_to_nfa(self):
        re = RegularExpression()
        re.parse("(a|b)*abb")
        nfa = re.to_nfa()
        assert nfa is not None

    def test_nfa_graph(self):
        re = RegularExpression()
        re.parse("(a|b)*abb")
        nfa = re.to_nfa()
        nfa_graph = RegularExpression.to_nfa_graph(nfa)

        assert nfa_graph.size() > 0
        assert nfa_graph.num_edges() > 0

    def test_nfa_graph_states(self):
        re = RegularExpression()
        re.parse("a b")
        nfa = re.to_nfa()
        nfa_graph = RegularExpression.to_nfa_graph(nfa)

        for i in range(nfa_graph.size()):
            state = nfa_graph.state_at(i)
            assert isinstance(state, str)

    def test_nfa_graph_edges(self):
        re = RegularExpression()
        re.parse("a b")
        nfa = re.to_nfa()
        nfa_graph = RegularExpression.to_nfa_graph(nfa)

        for i in range(nfa_graph.num_edges()):
            from_state = nfa_graph.edge_from(i)
            to_state = nfa_graph.edge_to(i)
            label = nfa_graph.edge_label(i)
            assert isinstance(from_state, int)
            assert isinstance(to_state, int)
            assert isinstance(label, str)

    def test_nfa_graph_to_svg(self):
        re = RegularExpression()
        re.parse("(a|b)*abb")
        nfa = re.to_nfa()
        nfa_graph = RegularExpression.to_nfa_graph(nfa)

        svg = nfa_graph.to_svg()
        assert "<svg" in svg

    def test_nfa_graph_to_svg_dark_mode(self):
        re = RegularExpression()
        re.parse("a|b")
        nfa = re.to_nfa()
        nfa_graph = RegularExpression.to_nfa_graph(nfa)

        svg = nfa_graph.to_svg(dark_mode=True)
        assert "<svg" in svg


class TestDFA:
    def test_to_dfa(self):
        re = RegularExpression()
        re.parse("(a|b)*abb")
        nfa = re.to_nfa()
        dfa = RegularExpression.to_dfa(nfa)
        assert dfa is not None

    def test_dfa_graph(self):
        re = RegularExpression()
        re.parse("(a|b)*abb")
        nfa = re.to_nfa()
        dfa = RegularExpression.to_dfa(nfa)
        dfa_graph = RegularExpression.to_dfa_graph(dfa)

        assert dfa_graph.size() > 0

    def test_dfa_graph_states(self):
        re = RegularExpression()
        re.parse("a|b")
        nfa = re.to_nfa()
        dfa = RegularExpression.to_dfa(nfa)
        dfa_graph = RegularExpression.to_dfa_graph(dfa)

        for i in range(dfa_graph.size()):
            state_id = dfa_graph.state_id_at(i)
            state_key = dfa_graph.state_key_at(i)
            state_type = dfa_graph.state_type_at(i)
            assert isinstance(state_id, str)
            assert isinstance(state_key, str)
            assert isinstance(state_type, str)

    def test_dfa_graph_edges(self):
        re = RegularExpression()
        re.parse("a|b")
        nfa = re.to_nfa()
        dfa = RegularExpression.to_dfa(nfa)
        dfa_graph = RegularExpression.to_dfa_graph(dfa)

        for i in range(dfa_graph.num_edges()):
            from_state = dfa_graph.edge_from(i)
            to_state = dfa_graph.edge_to(i)
            label = dfa_graph.edge_label(i)
            assert isinstance(from_state, int)
            assert isinstance(to_state, int)
            assert isinstance(label, str)

    def test_dfa_graph_to_svg(self):
        re = RegularExpression()
        re.parse("(a|b)*abb")
        nfa = re.to_nfa()
        dfa = RegularExpression.to_dfa(nfa)
        dfa_graph = RegularExpression.to_dfa_graph(dfa)

        svg = dfa_graph.to_svg()
        assert "<svg" in svg


class TestMinDFA:
    def test_to_min_dfa(self):
        re = RegularExpression()
        re.parse("(a|b)*abb")
        nfa = re.to_nfa()
        dfa = RegularExpression.to_dfa(nfa)
        min_dfa = RegularExpression.to_min_dfa(dfa)
        assert min_dfa is not None

    def test_min_dfa_fewer_states(self):
        re = RegularExpression()
        re.parse("(a|b)*abb")
        nfa = re.to_nfa()
        dfa = RegularExpression.to_dfa(nfa)
        min_dfa = RegularExpression.to_min_dfa(dfa)

        dfa_graph = RegularExpression.to_dfa_graph(dfa)
        min_dfa_graph = RegularExpression.to_dfa_graph(min_dfa)

        assert min_dfa_graph.size() <= dfa_graph.size()

    def test_min_dfa_graph_to_svg(self):
        re = RegularExpression()
        re.parse("(a|b)*abb")
        nfa = re.to_nfa()
        dfa = RegularExpression.to_dfa(nfa)
        min_dfa = RegularExpression.to_min_dfa(dfa)
        min_dfa_graph = RegularExpression.to_dfa_graph(min_dfa)

        svg = min_dfa_graph.to_svg()
        assert "<svg" in svg


class TestNFAState:
    def test_nfa_state_attributes(self):
        re = RegularExpression()
        re.parse("a")
        nfa = re.to_nfa()
        assert hasattr(nfa, "id")
        assert hasattr(nfa, "type")


class TestDFAState:
    def test_dfa_state_attributes(self):
        re = RegularExpression()
        re.parse("a")
        nfa = re.to_nfa()
        dfa = RegularExpression.to_dfa(nfa)
        assert hasattr(dfa, "id")
        assert hasattr(dfa, "key")
        assert hasattr(dfa, "type")

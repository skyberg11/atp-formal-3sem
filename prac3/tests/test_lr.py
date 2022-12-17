import pytest
from lib import grammar
from lib import lr as LR
from lib import exceptions


@pytest.mark.correctness_lr_grammar
def test_grammar_dyck_word():
    """Dyck word ab in Chomsky form grammar
        not in lr(0)"""
    A = grammar.Symbol('S')
    S = grammar.Symbol('A')
    a = grammar.Symbol('a')
    B = grammar.Symbol('B')
    b = grammar.Symbol('b')
    C = grammar.Symbol('C')
    D = grammar.Symbol('D')
    StartRule = grammar.ProductionRule([A], [])
    Rule1 = grammar.ProductionRule([A], [S, S])
    Rule2 = grammar.ProductionRule([A], [C, D])
    Rule3 = grammar.ProductionRule([A], [C, B])
    Rule4 = grammar.ProductionRule([S], [S, S])
    Rule5 = grammar.ProductionRule([S], [C, D])
    Rule6 = grammar.ProductionRule([S], [C, B])
    Rule7 = grammar.ProductionRule([B], [S, D])
    Rule8 = grammar.ProductionRule([C], [a])
    Rule9 = grammar.ProductionRule([D], [b])

    g = grammar.Grammar([StartRule, Rule1, Rule2, Rule3,
                        Rule4, Rule5, Rule6, Rule7, Rule8, Rule9])

    try:
        LR.LR0Parser(g)
    except exceptions.GrammarErrorLR0:
        return

    assert False


@pytest.mark.correctness_lr_grammar
def test_correctness_random_cf():
    """Some context-free grammar from seminar
        not in lr(0)"""
    S = grammar.Symbol('S')
    a = grammar.Symbol('a')
    F = grammar.Symbol('F')
    b = grammar.Symbol('b')
    StartRule = grammar.ProductionRule([S], [a, F, b, F])
    Rule1 = grammar.ProductionRule([F], [a, F, b])
    Rule2 = grammar.ProductionRule([F], [])

    g = grammar.Grammar([StartRule, Rule1, Rule2])

    try:
        LR.LR0Parser(g)
    except exceptions.GrammarErrorLR0:
        return

    assert False


@pytest.mark.correctness_lr_parser
def test_left_recursive():
    """Some left-recursive grammar"""
    S = grammar.Symbol('S')
    T = grammar.Symbol('T')
    p = grammar.Symbol('+')
    n = grammar.Symbol('n')
    l = grammar.Symbol('(')
    r = grammar.Symbol(')')
    StartRule = grammar.ProductionRule([S], [S, p, T])
    Rule1 = grammar.ProductionRule([S], [T])
    Rule2 = grammar.ProductionRule([T], [n])
    Rule3 = grammar.ProductionRule([T], [l, S, r])

    g = grammar.Grammar([StartRule, Rule1, Rule2, Rule3])

    testcases = [[l, n, p, n, r, p, n], [n, p, p, n],
                 [n, p, n], [n], [p], [l, l, n, p, n, r, p, n, r, p, n]]
    # |(n + n) + n| n + + n| n + n| n| +| ((n + n) + n) + n|
    answers = [True, False, True, True, False, True]

    parser = LR.LR0Parser(g)

    for test, answer in zip(testcases, answers):
        assert parser.does_generate(test) == answer


@pytest.mark.correctness_lr_parser
def test_cf_grammar():
    """Some CF grammar"""
    S = grammar.Symbol('S')
    A = grammar.Symbol('A')
    a = grammar.Symbol('a')
    b = grammar.Symbol('b')
    StartRule = grammar.ProductionRule([S], [A])
    Rule1 = grammar.ProductionRule([A], [a, A, A])
    Rule2 = grammar.ProductionRule([A], [b])

    g = grammar.Grammar([StartRule, Rule1, Rule2])

    testcases = [[a, b, b], [b], [a], [a, a, b], [a, a, a, b, b, b, b]]
    answers = [True, True, False, False, True]

    parser = LR.LR0Parser(g)

    for test, answer in zip(testcases, answers):
        assert parser.does_generate(test) == answer

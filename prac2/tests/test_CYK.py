from lib import grammar
from lib import CYK
from lib import exceptions


def test_correctness_CYK():
    """Dyck word ab in Chomsky form grammar"""
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

    testcases = [[a, b], [], [a, b, a], [a, b, a, b], [a, a, b, b], [a, a, b, a, b, a], [a, a, b, a, b, b], [b, a], [a], [b],
                 [a, b, a, b, a, a, b, b, a, a, b, a, b, b], [b, b, a], [a, a, b]]
    answers = [True, True, False, True, True, False,
               True, False, False, False, True, False, False]

    parser = CYK.CYKParser()

    for test, answer in zip(testcases, answers):
        if parser.does_generate(g, test) != answer:
            assert False


def test_correctness_Earley_CF():
    """Some context-free grammar from seminar"""
    S = grammar.Symbol('S')
    a = grammar.Symbol('a')
    F = grammar.Symbol('F')
    b = grammar.Symbol('b')
    StartRule = grammar.ProductionRule([S], [a, F, b, F])
    Rule1 = grammar.ProductionRule([F], [a, F, b])
    Rule2 = grammar.ProductionRule([F], [])

    g = grammar.Grammar([StartRule, Rule1, Rule2])

    testcases = [[a, a, b, b], [a, b], [a, a, b, b, a, b],
                 [a, a, b, b, b], [b], [], [a, b, a, b], [a, b, a, b, a, b]]
    answers = [True, True, True, False, False, False, True, False]

    parser = CYK.CYKParser()

    for test, answer in zip(testcases, answers):
        try:
            if parser.does_generate(g, test) != answer:
                assert False
            assert False
        except exceptions.BadGrammarFormError:
            pass

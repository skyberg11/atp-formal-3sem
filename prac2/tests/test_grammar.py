from lib.grammar import *
from lib import exceptions
import copy


def test_symbol():
    upper = Symbol('T')
    lower = Symbol('i')
    start = Symbol('S')

    try:
        bad = Symbol('!')
    except exceptions.BadLetterError:
        pass

    try:
        bad = Symbol('TK')
    except exceptions.SymbolLenError:
        pass

    assert repr(start) == 'S'
    assert upper.is_nonterm()
    assert lower.is_term()
    assert start.is_nonterm()


def test_production_rule():
    upper = Symbol('A')
    lower = Symbol('a')
    start = Symbol('S')

    context_free_rule = ProductionRule([upper], [lower])
    context_sens_rule = ProductionRule([upper, lower], [lower, lower, lower])
    unrestricted_rule = ProductionRule([upper, start], [lower])
    another_sens_rule = ProductionRule([upper, lower], [lower, lower, lower])
    imposter_yes_rule = another_sens_rule
    imposter_yet_yesr = copy.deepcopy(imposter_yes_rule)

    s = set()
    s.add(upper)
    s.add(context_free_rule)

    assert upper in s
    assert context_free_rule in s
    assert not imposter_yes_rule in s
    assert repr(imposter_yet_yesr) == 'Aa -> aaa'
    assert context_free_rule.get_type() == ProductionRuleType.CONTEXTFREE
    assert context_sens_rule.get_type() == ProductionRuleType.CONTEXTSENSIVITE
    assert unrestricted_rule.get_type() == ProductionRuleType.UNRESTRICTED
    assert context_sens_rule == another_sens_rule
    assert context_sens_rule == imposter_yes_rule
    assert context_sens_rule == imposter_yet_yesr


def test_grammar():
    upper = Symbol('A')
    lower = Symbol('a')
    start = Symbol('S')

    start_rule_eps = ProductionRule([start], [])
    rule = ProductionRule([upper], [lower])
    rule_BC = ProductionRule([upper], [upper, upper])
    start_rule = ProductionRule([start], [upper])
    bad_Chomsky = ProductionRule([upper], [start, start])
    context_sens_rule = ProductionRule([upper, lower], [lower, lower, lower])
    unrestricted_rule = ProductionRule([upper, start], [lower])
    copy_rule = rule
    real_copy_rule = copy.deepcopy(rule)

    gram_Chomsky_1 = Grammar([start_rule_eps])
    gram_Chomsky_2 = Grammar([rule, start_rule_eps, rule_BC])
    gram_context_sens = Grammar([start_rule_eps, rule, context_sens_rule])
    gram_1 = Grammar([rule, rule_BC])
    gram_2 = Grammar([bad_Chomsky, start_rule_eps, start_rule])
    generative_gram = Grammar([unrestricted_rule])

    assert gram_Chomsky_1.is_in_Chomsky()
    assert gram_Chomsky_2.is_in_Chomsky()
    assert gram_Chomsky_2.is_in_Chomsky()
    assert not gram_1.is_in_Chomsky()
    assert not gram_2.is_in_Chomsky()
    assert not generative_gram.is_in_Chomsky()

    assert gram_1.has_rule(rule)
    assert gram_1.has_rule(copy_rule)
    assert gram_1.has_rule(real_copy_rule)
    assert not gram_Chomsky_2.has_rule(start_rule)

    assert gram_2.grammar_type() == GrammarType.CONTEXTFREE
    assert gram_context_sens.grammar_type() == GrammarType.CONTEXTSENSIVITE
    assert generative_gram.grammar_type() == GrammarType.GENERATIVE

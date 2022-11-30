from abc import ABC, abstractmethod
from enum import Enum
from lib import exceptions
from typing import List


class ProductionRuleType(Enum):
    UNRESTRICTED = 0
    CONTEXTSENSIVITE = 1
    CONTEXTFREE = 2


class GrammarType(Enum):
    GENERATIVE = 0
    CONTEXTSENSIVITE = 1
    CONTEXTFREE = 2


class SymbolType(Enum):
    TERM = 0
    NONTERM = 1


class Symbol:
    def __init__(self, word: str):
        if len(word) != 1:
            raise exceptions.SymbolLenError
        if word[0] >= 'A' and word[0] <= 'Z':
            self.type = SymbolType.NONTERM
        elif word[0] >= 'a' and word[0] <= 'z':
            self.type = SymbolType.TERM
        else:
            raise exceptions.BadLetterError
        self.word = word

    def is_term(self) -> bool:
        return self.type == SymbolType.TERM

    def is_nonterm(self) -> bool:
        return self.type == SymbolType.NONTERM

    def __repr__(self):
        return self.word

    def __eq__(self, other):
        return self.word == other.word

    def __hash__(self):
        return hash(self.word)


class ProductionRule:
    def __init__(self, left: List[Symbol], right: List[Symbol]):
        self.type = ProductionRuleType.UNRESTRICTED
        if len(left) == 1 and left[0].type == SymbolType.NONTERM:
            self.type = ProductionRuleType.CONTEXTFREE
        else:
            pref = 0
            suff = 0
            for l, r in zip(left, right):
                if l == r:
                    pref += 1
                else:
                    break
            for l, r in zip(reversed(left), reversed(right)):
                if l == r:
                    suff += 1
                else:
                    break
            for i in range(pref + 1):
                if i + 1 >= len(left) - suff and left[i].type == SymbolType.NONTERM:
                    self.type = ProductionRuleType.CONTEXTSENSIVITE
                    break
        self.left = left
        self.right = right

    def get_type(self):
        return self.type

    def __repr__(self):
        to_print = []
        for sym in self.left:
            to_print.append(sym.word)
        to_print.append(" -> ")
        for sym in self.right:
            to_print.append(sym.word)
        return ''.join(to_print)

    def __eq__(self, other):
        return self.left == other.left and self.right == other.right

    def __hash__(self):
        return hash((tuple(self.left), tuple(self.right)))


class Grammar:
    def __init__(self, rules: List[ProductionRule]):
        self.rules = rules
        self.__is_chomsky = False
        rules_types_set = set()
        for rule in rules:
            rules_types_set.add(rule.get_type())
        if ProductionRuleType.UNRESTRICTED in rules_types_set:
            self.type = GrammarType.GENERATIVE
        elif ProductionRuleType.CONTEXTSENSIVITE in rules_types_set:
            self.type = GrammarType.CONTEXTSENSIVITE
        else:
            self.type = GrammarType.CONTEXTFREE

    def grammar_type(self) -> bool:
        return self.type

    def is_in_chomsky(self) -> bool:
        if self.__is_chomsky == True:
            return True
        if self.type != GrammarType.CONTEXTFREE:
            return False
        has_start_eps = False
        for rule in self.rules:
            if rule.left[0].word == 'S' and len(rule.right) == 0:
                has_start_eps = True
            elif rule.left[0].type == SymbolType.TERM:
                return False
            else:
                if len(rule.right) == 2:
                    if rule.right[0].type != SymbolType.NONTERM or rule.right[1].type != SymbolType.NONTERM:
                        return False
                    if rule.right[0].word == 'S' or rule.right[1].word == 'S':
                        return False
                elif len(rule.right) == 1:
                    if rule.right[0].type != SymbolType.TERM:
                        return False
                else:
                    return False
        self.__is_chomsky = has_start_eps
        return has_start_eps

    def has_rule(self, rule: ProductionRule) -> bool:
        return rule in self.rules


class GrammarParser(ABC):
    @abstractmethod
    def does_generate(self, word: str):
        raise exceptions.ParserError

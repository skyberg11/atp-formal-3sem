from lib.grammar import *
import copy


class NodeEarley:
    def __init__(self, rule: ProductionRule, index: int):
        self.rule = copy.deepcopy(rule)
        self.index = index
        self.pivot = 0

    def __eq__(self, other):
        return self.rule == other.rule and self.pivot == other.pivot and self.index == other.index

    def __hash__(self):
        return hash((self.rule, self.index, self.pivot))

    def can_complete(self) -> bool:
        return self.pivot == len(self.rule.right)

    def get_scan_state(self) -> SymbolType:
        if self.pivot >= len(self.rule.right):
            return False
        else:
            return self.rule.right[self.pivot].type

    def can_scan(self, letter: Symbol) -> bool:
        if self.pivot < len(self.rule.right):
            return self.rule.right[self.pivot] == letter
        else:
            return False

    def push_pivot(self):
        self.pivot += 1


class EarleyParser(GrammarParser):
    def __scan(self, D, j: int, word: List[Symbol]):
        if j == 0:
            return D
        for node in D[j - 1]:
            if node.can_scan(word[j - 1]):
                new_node = copy.deepcopy(node)
                new_node.push_pivot()
                D[j].add(new_node)
        return D

    def __complete(self, D, j: int):
        complement = set()
        for node in D[j]:
            if node.can_complete():
                for parent_node in D[node.index]:
                    if parent_node.can_scan(node.rule.left[0]):
                        new_node = copy.deepcopy(parent_node)
                        new_node.push_pivot()
                        complement.add(new_node)
        D[j].update(complement)
        return D

    def __predict(self, D, j: int, gram: Grammar):
        complement = set()
        for rule in D[j]:
            if rule.get_scan_state() == SymbolType.NONTERM:
                for gram_rule in gram.rules:
                    if rule.can_scan(gram_rule.left[0]):
                        new_node = NodeEarley(gram_rule, j)
                        complement.add(new_node)
        D[j].update(complement)
        return D

    def does_generate(self, gram: Grammar, word: List[Symbol]) -> bool:
        if gram.grammar_type() != GrammarType.CONTEXTFREE:
            raise exceptions.BadGrammarFormError

        D = [set() for _ in range(len(word) + 1)]

        start_rule = NodeEarley(ProductionRule([Symbol('X')],
                                               [Symbol('S')]), 0)

        D[0].add(start_rule)

        for j in range(len(word) + 1):
            D = self.__scan(D, j, word)
            szl = len(D[j])
            while True:
                D = self.__complete(D, j)
                D = self.__predict(D, j, gram)
                if szl == len(D[j]):
                    break
                else:
                    szl = len(D[j])

        end_node = NodeEarley(ProductionRule([Symbol('X')],
                                             [Symbol('S')]), 0)
        end_node.push_pivot()

        return end_node in D[len(word)]

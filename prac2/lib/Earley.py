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
    def __scan(self, nodes_set, j: int, word: List[Symbol]):
        if j == 0:
            return nodes_set
        for node in nodes_set[j - 1]:
            if node.can_scan(word[j - 1]):
                new_node = copy.deepcopy(node)
                new_node.push_pivot()
                nodes_set[j].add(new_node)
        return nodes_set

    def __complete(self, nodes_set, j: int):
        complement = set()
        for node in nodes_set[j]:
            if node.can_complete():
                for parent_node in nodes_set[node.index]:
                    if parent_node.can_scan(node.rule.left[0]):
                        new_node = copy.deepcopy(parent_node)
                        new_node.push_pivot()
                        complement.add(new_node)
        nodes_set[j].update(complement)
        return nodes_set

    def __predict(self, nodes_set, j: int, cf_grammar: Grammar):
        complement = set()
        for rule in nodes_set[j]:
            if rule.get_scan_state() == SymbolType.NONTERM:
                for cf_grammar_rule in cf_grammar.rules:
                    if rule.can_scan(cf_grammar_rule.left[0]):
                        new_node = NodeEarley(cf_grammar_rule, j)
                        complement.add(new_node)
        nodes_set[j].update(complement)
        return nodes_set

    def does_generate(self, cf_grammar: Grammar, word: List[Symbol]) -> bool:
        if cf_grammar.grammar_type() != GrammarType.CONTEXTFREE:
            raise exceptions.BadGrammarFormError

        nodes_set = [set() for _ in range(len(word) + 1)]

        start_rule = NodeEarley(ProductionRule([Symbol('X')],
                                               [Symbol('S')]), 0)

        nodes_set[0].add(start_rule)

        for j in range(len(word) + 1):
            nodes_set = self.__scan(nodes_set, j, word)
            szl = len(nodes_set[j])
            while True:
                nodes_set = self.__complete(nodes_set, j)
                nodes_set = self.__predict(nodes_set, j, cf_grammar)
                if szl == len(nodes_set[j]):
                    break
                else:
                    szl = len(nodes_set[j])

        end_node = NodeEarley(ProductionRule([Symbol('X')],
                                             [Symbol('S')]), 0)
        end_node.push_pivot()

        return end_node in nodes_set[len(word)]

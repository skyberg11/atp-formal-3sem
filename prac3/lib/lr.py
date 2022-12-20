from lib.grammar import *
from lib.exceptions import *
import copy


class Item:
    def __init__(self, rule: ProductionRule):
        self.rule = copy.deepcopy(rule)
        self.pivot = 0

    def __eq__(self, other):
        return self.rule == other.rule and self.pivot == other.pivot

    def __hash__(self):
        return hash((self.rule, self.pivot))

    def can_scan(self, letter: Symbol) -> bool:
        if self.pivot < len(self.rule.right):
            return self.rule.right[self.pivot] == letter
        else:
            return False

    def push_pivot(self):
        self.pivot += 1


class NodeDFA:
    def __init__(self):
        self.items = set()
        self.transitions = {}

    def __eq__(self, other):
        return self.items == other.items

    def __hash__(self):
        return hash((self.items, self.transitions))

    def append_item(self, item: Item):
        self.items.add(item)

    def closure(self, grammar: Grammar):
        complement = set()
        last_sz = len(self.items)
        cur_sz = last_sz - 1

        while(last_sz != cur_sz):
            for item in self.items:
                for rule in grammar.rules:
                    if item.can_scan(rule.left[0]):
                        complement.add(Item(rule))
            self.items.update(complement)
            last_sz = cur_sz
            cur_sz = len(self.items)


class LR0Parser(GrammarParser):
    def __init__(self, grammar: Grammar):
        self.grammar = grammar
        self.DFA = []
        self.alphabet = set()

        self.alphabet.add(Symbol('$'))

        for rule in grammar.rules:
            for left_symbol in rule.left:
                self.alphabet.add(left_symbol)
            for right_symbol in rule.right:
                self.alphabet.add(right_symbol)

        if grammar.grammar_type() != GrammarType.CONTEXTFREE:
            raise BadGrammarFormError

        start_node = NodeDFA()

        # augmenting grammar
        X = Symbol('X')
        S = Symbol('S')
        self.alphabet.add(X)
        self.alphabet.add(S)
        rule = ProductionRule([X], [S])
        start_node.append_item(Item(rule))

        # appending start node
        self.DFA.append(start_node)
        self.DFA[0].closure(self.grammar)

        self.__build_step(0)

        self.__build_control_table()

    def __goto(self, cur_index: int, symbol: Symbol) -> int:
        new_node = NodeDFA()

        for item in self.DFA[cur_index].items:
            if item.can_scan(symbol):
                new_item = copy.deepcopy(item)
                new_item.push_pivot()
                new_node.append_item(new_item)

        if len(new_node.items) == 0:
            return -1

        new_node.closure(self.grammar)

        if new_node not in self.DFA:
            self.DFA.append(new_node)
            node_index = len(self.DFA) - 1
            self.__build_step(node_index)
        else:
            node_index = self.DFA.index(new_node)

        return node_index

    def __build_step(self, cur_index: int):
        for symbol in self.alphabet:
            count = self.__goto(cur_index, symbol)
            if count > 0:
                self.DFA[cur_index].transitions[symbol] = count

    def __shift(self, index: int):
        for symbol in self.alphabet:  # handling shift
            if symbol in self.DFA[index].transitions.keys():
                if self.table[index][self.symbol_map[symbol]] != '--':
                    raise GrammarErrorLR0  # grammar is not lr
                if symbol.is_nonterm():  # filling table with sx or just number for nonterm
                    instruction = self.DFA[index].transitions[symbol]
                else:
                    instruction = "s" + \
                        str(self.DFA[index].transitions[symbol])
                self.table[index][self.symbol_map[symbol]] = instruction

    def __reduce(self, index: int):
        for item in self.DFA[index].items:  # handling reduce
            if item.pivot >= len(item.rule.right) and item.rule in self.grammar.rules:
                # for everyone because of lr(0) (forward looking is k = 0)
                for symbol in self.alphabet:
                    if symbol.is_term():  # reduce instruction only for term
                        self.table[index][self.symbol_map[symbol]
                                          ] = "r" + str(self.rule_map[item.rule])

    def __build_control_table(self):
        self.symbol_map = {}
        self.rule_map = {}
        self.rule_map_reverse = {}
        i = 0
        for symbol in self.alphabet:
            self.symbol_map[symbol] = i
            i += 1

        i = 1  # without start node that is why start from 1
        for rule in self.grammar.rules:
            self.rule_map[rule] = i
            self.rule_map_reverse[i] = rule
            i += 1

        self.table = [['--' for i in range(len(self.alphabet))]
                      for j in range(len(self.DFA))]

        for index in range(len(self.DFA)):
            self.__reduce(index)
            self.__shift(index)

        X = Symbol('X')
        S = Symbol('S')
        rule = ProductionRule([X], [S])
        acceptance_item = Item(rule)

        acceptance_item.push_pivot()

        end_symbol = Symbol('$')
        for index in range(len(self.DFA)):
            if acceptance_item in self.DFA[index].items:
                self.table[index][self.symbol_map[end_symbol]] = 'r0'

    def does_generate(self, word: List[Symbol]):
        word.append(Symbol('$'))
        stack = [0]
        pivot = 0
        cur_state = 0
        cur_token = Symbol(str(word[pivot]))

        instruction = self.table[cur_state][self.symbol_map[cur_token]]

        # r := reduce; s := shift;
        while instruction != 'r0':
            if instruction == '--':
                return False

            if instruction[0] == 's':
                cur_state = int(instruction[1])
                stack.append(cur_token)
                stack.append(cur_state)
                pivot += 1
                cur_token = Symbol(str(word[pivot]))
            elif instruction[0] == 'r':
                rule = self.rule_map_reverse[int(instruction[1])]
                for _ in range(len(rule.right) * 2):
                    stack.pop()
                stack.append(rule.left[0])
                stack.append(self.table[stack[len(stack) - 2]]
                             [self.symbol_map[stack[len(stack) - 1]]])
                cur_state = stack[len(stack) - 1]
            else:
                return False

            instruction = self.table[cur_state][self.symbol_map[cur_token]]

        return True

from lib.grammar import *
from lib.exceptions import *
import termtables as tt
import copy
import time


class Item:
    def __init__(self, rule: ProductionRule):
        self.rule = copy.deepcopy(rule)
        self.pivot = 0

    def __eq__(self, other):
        return self.rule == other.rule and self.pivot == other.pivot

    def __hash__(self):
        return hash((self.rule, self.pivot))

    def __repr__(self) -> str:
        out = ""
        for letter in self.rule.left:
            out += repr(letter)

        out += "->"
        i = 0
        for letter in self.rule.right:
            if i == self.pivot:
                out += "."
            i += 1
            out += repr(letter)
        if i == self.pivot:
            out += "."
        return out

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
        cur_sz = last_sz + 1
        while(last_sz != cur_sz):
            for item in self.items:
                for rule in grammar.rules:
                    if item.can_scan(rule.left[0]):
                        new_item = Item(rule)
                        complement.add(new_item)
            self.items.update(complement)
            last_sz = cur_sz
            cur_sz = len(self.items)


class LRParser(GrammarParser):
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

        X = Symbol('X')
        S = Symbol('S')
        self.alphabet.add(X)
        self.alphabet.add(S)

        rule = ProductionRule([X], [S])
        start_node.append_item(Item(rule))

        self.DFA.append(start_node)
        self.DFA[0].closure(self.grammar)

        self.build_step(0)

        ind = 0
        print("BEGIN:")
        for entry in self.DFA:
            print("==================")
            print("Index: ", ind)
            ind += 1
            print("items:")
            for item in entry.items:
                print(item)
            print("transitions:")
            for a, b in entry.transitions.items():
                print(a, ":", b)
            print("==================")

        self.fill_table()
        for i in range(len(self.DFA)):
            for j in range(len(self.alphabet)):
                print(self.table[i][j], end=' ')
            print('')

    def goto(self, cur_index: int, symbol: Symbol) -> int:
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
            self.build_step(node_index)
        else:
            node_index = self.DFA.index(new_node)

        return node_index

    def build_step(self, cur_index: int):
        for symbol in self.alphabet:
            count = self.goto(cur_index, symbol)
            if count > 0:
                self.DFA[cur_index].transitions[symbol] = count

    def fill_table(self):
        self.symbol_map = {}
        self.rule_map = {}
        self.rule_map_reverse = {}
        i = 0
        for symbol in self.alphabet:
            print(symbol, end='  ')
            self.symbol_map[symbol] = i
            i += 1
        print()
        # self.symbol_map[Symbol('$')] = i

        i = 1
        for rule in self.grammar.rules:
            self.rule_map[rule] = i
            self.rule_map_reverse[i] = rule
            i += 1

        self.table = [['--' for i in range(len(self.alphabet))]
                      for j in range(len(self.DFA))]

        for index in range(len(self.DFA)):
            for item in self.DFA[index].items:  # handling reduce
                if item.pivot >= len(item.rule.right) and item.rule in self.grammar.rules:
                    for symbol in self.alphabet:
                        if symbol.is_term():
                            self.table[index][self.symbol_map[symbol]
                                              ] = "r" + str(self.rule_map[item.rule])
            for symbol in self.alphabet:  # handling shift
                if symbol in self.DFA[index].transitions.keys():
                    if self.table[index][self.symbol_map[symbol]] != '--':
                        raise GrammarErrorLR0
                    if symbol.is_nonterm():
                        self.table[index][self.symbol_map[symbol]
                                          ] = self.DFA[index].transitions[symbol]
                    else:
                        self.table[index][self.symbol_map[symbol]
                                          ] = "s" + str(self.DFA[index].transitions[symbol])

        X = Symbol('X')
        S = Symbol('S')
        rule = ProductionRule([X], [S])
        acceptance_item = Item(rule)

        acceptance_item.push_pivot()

        for index in range(len(self.DFA)):
            if acceptance_item in self.DFA[index].items:
                self.table[index][self.symbol_map[Symbol('$')]] = 'r0'

    def does_generate(self, word: str):
        word += '$'
        stack = [0]
        pivot = 0
        cur_state = 0
        cur_token = Symbol(str(word[pivot]))

        instruction = self.table[cur_state][self.symbol_map[cur_token]]

        while instruction != 'r0':
            print(stack, cur_state, cur_token)
            time.sleep(0.1)
            if instruction == '--':
                return False
            if instruction[0] == 's':
                cur_state = int(instruction[1])
                stack.append(cur_token)
                stack.append(cur_state)
                pivot += 1
                cur_token = Symbol(str(word[pivot]))

            elif instruction[0] == 'r':
                for _ in range(len(self.rule_map_reverse[int(instruction[1])].right) * 2):
                    stack.pop()
                stack.append(
                    self.rule_map_reverse[int(instruction[1])].left[0])
                stack.append(self.table[stack[len(stack) - 2]]
                             [self.symbol_map[stack[len(stack) - 1]]])
                cur_state = stack[len(stack) - 1]
            else:
                return False

            instruction = self.table[cur_state][self.symbol_map[cur_token]]

        return True

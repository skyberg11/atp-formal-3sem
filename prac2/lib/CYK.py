from lib.grammar import *


class CYKParser(GrammarParser):
    def __init_one_letters(self, dynamic, gram: Grammar, word: List[Symbol]):
        for index, symbol in enumerate(word):
            for rule in gram.rules:
                if len(rule.right) == 1:
                    if rule.right[0] == symbol:
                        A = ord(rule.left[0].word[0]) - ord('A')
                        dynamic[A][index][index + 1] = True
        return dynamic

    def __process_words(self, dynamic, gram: Grammar, word_length: int, word: List[Symbol]):
        for start in range(len(word)):
            end = start + word_length
            for rule in gram.rules:
                if len(rule.right) == 2:
                    for mid_position in range(start + 1, end):
                        A = ord(rule.left[0].word[0]) - ord('A')
                        B = ord(rule.right[0].word[0]) - ord('A')
                        C = ord(rule.right[1].word[0]) - ord('A')
                        dynamic[A][start][end] |= dynamic[B][start][mid_position] & dynamic[C][mid_position][end]
        return dynamic

    def does_generate(self, gram: Grammar, word: List[Symbol]) -> bool:
        if gram.is_in_Chomsky() == False:
            raise exceptions.BadGrammarFormError
        for sym in word:
            if sym.type == SymbolType.NONTERM:
                raise exceptions.BadLetterError

        dynamic = [[[False for i in range(len(word) * 2)]
                    for j in range(len(word) * 2)] for k in range(26)]

        if len(word) == 0:
            return True

        dynamic = self.__init_one_letters(dynamic, gram, word)

        for word_length in range(2, len(word) + 1):
            dynamic = self.__process_words(dynamic, gram, word_length, word)

        return dynamic[ord('S') - ord('A')][0][len(word)]

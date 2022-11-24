class SymbolLenError(Exception):
    """Raised when symbol is not a char"""
    pass


class BadTypeError(Exception):
    """Raised when symbol is not a char"""
    pass


class BadLetterError(Exception):
    """Raised when symbol is not in alphabet"""
    pass


class BadGrammarFormError(Exception):
    """Raised when grammar is not in required form"""
    pass


class BadAlgoError(Exception):
    """Raised when algorithm is not implemented"""
    pass


class ParserError(Exception):
    """Raised when parser is not set"""
    pass

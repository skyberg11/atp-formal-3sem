## Обзор
Библиотека, где частично реализована формальная грамматика или просто грамматика в теории формальных языков.
Созданы типы символа, правила и грамматики.
```Python
from lib import grammar

letter = grammar.Symbol('A')
rule = grammar.ProductionRule([letter], [letter])
gram = grammar.Grammar([rule])
```

Также реализованы простейшие алгоритмы синтаксического анализа [CYK](https://neerc.ifmo.ru/wiki/index.php?title=%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%9A%D0%BE%D0%BA%D0%B0-%D0%AF%D0%BD%D0%B3%D0%B5%D1%80%D0%B0-%D0%9A%D0%B0%D1%81%D0%B0%D0%BC%D0%B8_%D1%80%D0%B0%D0%B7%D0%B1%D0%BE%D1%80%D0%B0_%D0%B3%D1%80%D0%B0%D0%BC%D0%BC%D0%B0%D1%82%D0%B8%D0%BA%D0%B8_%D0%B2_%D0%9D%D0%A4%D0%A5) и [Эрли](https://neerc.ifmo.ru/wiki/index.php?title=%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%AD%D1%80%D0%BB%D0%B8) (Наследники абстрактого класса GrammarParser), с методом does_generate(self, Grammar, Word), который проверяет выводимость слова Word в грамматике Grammar. Для CYK требуется грамматика в [НФ Хомского](https://neerc.ifmo.ru/wiki/index.php?title=%D0%9D%D0%BE%D1%80%D0%BC%D0%B0%D0%BB%D1%8C%D0%BD%D0%B0%D1%8F_%D1%84%D0%BE%D1%80%D0%BC%D0%B0_%D0%A5%D0%BE%D0%BC%D1%81%D0%BA%D0%BE%D0%B3%D0%BE), а для Эрли работает для произвольной КС.


## Сборка
```bash
sudo apt install python3-venv
python3 -m venv venv
source venv/bin/activate
python -m pip install -r requirements.txt
```
Выйти с виртуалки
```bash
deactivate
```
Покрытие тестов. 
```bash
pytest --cov=lib --cov-report term-missing
```
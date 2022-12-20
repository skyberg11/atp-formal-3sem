## Обзор
Библиотека, где частично реализована формальная грамматика или просто грамматика в теории формальных языков.
Созданы типы символа, правила и грамматики.
```Python
from lib import grammar

letter = grammar.Symbol('A')
rule = grammar.ProductionRule([letter], [letter])
gram = grammar.Grammar([rule])
```

Также реализован LR(0)-разбор — частный случай LR(k)-разборщикa.  [LR0](https://neerc.ifmo.ru/wiki/index.php?title=LR(0)-%D1%80%D0%B0%D0%B7%D0%B1%D0%BE%D1%80), с методом does_generate(self, Word), который проверяет выводимость слова Word в грамматике. В данном случае k=0, то есть решение о своих действиях принимается только на основании содержимого стека, символы входной цепочки не учитываются. Грамматика должна удовлетворять некоторым требованиям [LR(k)-грамматики](https://neerc.ifmo.ru/wiki/index.php?title=LR(k)-%D0%B3%D1%80%D0%B0%D0%BC%D0%BC%D0%B0%D1%82%D0%B8%D0%BA%D0%B8).


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
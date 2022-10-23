## Обзор
Библиотека, которая может НКА переводить в ДКА и МПДКА по известных алгоритмам. 
Чтобы создать НКА, нужно подать на вход файл с определенной грамматикой. Функция с сигнатурой
```C++ 
virtual AutomatonT<kNondeterministic> StreamBuildAutomaton(std::fstream& in)
```
возвращает НКА, который прочтет из fstream по след. правилам:
```
automaton ::= header "--BEGIN--" body "--END--"

header ::= alphabet header-start header-acceptance
alphabet ::= "Sigma:" STRARRAY
header-start ::= "Start:" STRING
header-acceptance ::= "Acceptance:" ARRAY
             
body             ::= (state-name edge*)+
state-name       ::= "State:" identifier
edge             ::= "->" word identifier

word ::= STRING
identifier ::= INTEGER

INTEGER ::= [0-1000000]
STRING ::= [a-zA-Z_0-9]*
ARRAY ::= INTEGER " " ARRAY | #
STRARRAY ::= STRING " " STRARRAY | # 

Пример:
Sigma: a b #
Start: 1
Acceptance: 2 3 #
--BEGIN--
State: 1
    -> a 2
State: 2
    -> b 2
    -> EPS 3
State: 3
    -> aba 3
    -> a 4
State: 4
    -> ab 4
    -> a 2
--END--
```
Есть возможность получения множества слов, которые читает автомат, до некоторой длины. 
Также присутствуют тесты. Реализованы промежуточные преобразования автоматов.

## Сборка
```bash
mkdir build
cd build
cmake ..
make
```
Бинарник тестов появиться в папке bin. Далее с папки build/
```bash
./../bin/test
```
Покрытие тестов. В /build/tests/CMakeFiles/test.dir
```bash
gcov test.cpp.gcno
lcov --capture --directory . --output-file gtest_coverage.info
genhtml gtest_coverage.info --output-directory ../../../CODE_COVERAGE
```
Результаты в /build/CODE_COVERAGE/index.html

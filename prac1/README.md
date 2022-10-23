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
```html
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">

<html lang="en">

<head>
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
  <title>LCOV - gtest_coverage.info</title>
  <link rel="stylesheet" type="text/css" href="gcov.css">
</head>

<body>

  <table width="100%" border=0 cellspacing=0 cellpadding=0>
    <tr><td class="title">LCOV - code coverage report</td></tr>
    <tr><td class="ruler"><img src="glass.png" width=3 height=3 alt=""></td></tr>

    <tr>
      <td width="100%">
        <table cellpadding=1 border=0 width="100%">
          <tr>
            <td width="10%" class="headerItem">Current view:</td>
            <td width="35%" class="headerValue">top level</td>
            <td width="5%"></td>
            <td width="15%"></td>
            <td width="10%" class="headerCovTableHead">Hit</td>
            <td width="10%" class="headerCovTableHead">Total</td>
            <td width="15%" class="headerCovTableHead">Coverage</td>
          </tr>
          <tr>
            <td class="headerItem">Test:</td>
            <td class="headerValue">gtest_coverage.info</td>
            <td></td>
            <td class="headerItem">Lines:</td>
            <td class="headerCovTableEntry">1777</td>
            <td class="headerCovTableEntry">2289</td>
            <td class="headerCovTableEntryMed">77.6 %</td>
          </tr>
          <tr>
            <td class="headerItem">Date:</td>
            <td class="headerValue">2022-10-23 05:50:55</td>
            <td></td>
            <td class="headerItem">Functions:</td>
            <td class="headerCovTableEntry">1447</td>
            <td class="headerCovTableEntry">1683</td>
            <td class="headerCovTableEntryMed">86.0 %</td>
          </tr>
          <tr><td><img src="glass.png" width=3 height=3 alt=""></td></tr>
        </table>
      </td>
    </tr>

    <tr><td class="ruler"><img src="glass.png" width=3 height=3 alt=""></td></tr>
  </table>

  <center>
  <table width="80%" cellpadding=1 cellspacing=1 border=0>

    <tr>
      <td width="50%"><br></td>
      <td width="10%"></td>
      <td width="10%"></td>
      <td width="10%"></td>
      <td width="10%"></td>
      <td width="10%"></td>
    </tr>

    <tr>
      <td class="tableHead">Directory <span class="tableHeadSort"><img src="glass.png" width=10 height=14 alt="Sort by name" title="Sort by name" border=0></span></td>
      <td class="tableHead" colspan=3>Line Coverage <span class="tableHeadSort"><a href="index-sort-l.html"><img src="updown.png" width=10 height=14 alt="Sort by line coverage" title="Sort by line coverage" border=0></a></span></td>
      <td class="tableHead" colspan=2>Functions <span class="tableHeadSort"><a href="index-sort-f.html"><img src="updown.png" width=10 height=14 alt="Sort by function coverage" title="Sort by function coverage" border=0></a></span></td>
    </tr>
    <tr>
      <td class="coverFile"><a href="home/skyberg/vscode/mipt/proga/miptedu/formal/atp-formal-3sem/prac1/lib/index.html">/home/skyberg/vscode/mipt/proga/miptedu/formal/atp-formal-3sem/prac1/lib</a></td>
      <td class="coverBar" align="center">
        <table border=0 cellspacing=0 cellpadding=1><tr><td class="coverBarOutline"><img src="emerald.png" width=96 height=10 alt="96.4%"><img src="snow.png" width=4 height=10 alt="96.4%"></td></tr></table>
      </td>
      <td class="coverPerHi">96.4&nbsp;%</td>
      <td class="coverNumHi">353 / 366</td>
      <td class="coverPerHi">100.0&nbsp;%</td>
      <td class="coverNumHi">49 / 49</td>
    </tr>
    <tr>
      <td class="coverFile"><a href="home/skyberg/vscode/mipt/proga/miptedu/formal/atp-formal-3sem/prac1/tests/index.html">/home/skyberg/vscode/mipt/proga/miptedu/formal/atp-formal-3sem/prac1/tests</a></td>
      <td class="coverBar" align="center">
        <table border=0 cellspacing=0 cellpadding=1><tr><td class="coverBarOutline"><img src="emerald.png" width=94 height=10 alt="94.4%"><img src="snow.png" width=6 height=10 alt="94.4%"></td></tr></table>
      </td>
      <td class="coverPerHi">94.4&nbsp;%</td>
      <td class="coverNumHi">68 / 72</td>
      <td class="coverPerHi">100.0&nbsp;%</td>
      <td class="coverNumHi">22 / 22</td>
    </tr>
    <tr>
      <td class="coverFile"><a href="usr/local/include/gtest/index.html">/usr/local/include/gtest</a></td>
      <td class="coverBar" align="center">
        <table border=0 cellspacing=0 cellpadding=1><tr><td class="coverBarOutline"><img src="ruby.png" width=11 height=10 alt="10.8%"><img src="snow.png" width=89 height=10 alt="10.8%"></td></tr></table>
      </td>
      <td class="coverPerLo">10.8&nbsp;%</td>
      <td class="coverNumLo">7 / 65</td>
      <td class="coverPerLo">17.6&nbsp;%</td>
      <td class="coverNumLo">6 / 34</td>
    </tr>
    <tr>
      <td class="coverFile"><a href="usr/local/include/gtest/internal/index.html">/usr/local/include/gtest/internal</a></td>
      <td class="coverBar" align="center">
        <table border=0 cellspacing=0 cellpadding=1><tr><td class="coverBarOutline"><img src="amber.png" width=85 height=10 alt="85.0%"><img src="snow.png" width=15 height=10 alt="85.0%"></td></tr></table>
      </td>
      <td class="coverPerMed">85.0&nbsp;%</td>
      <td class="coverNumMed">17 / 20</td>
      <td class="coverPerMed">84.6&nbsp;%</td>
      <td class="coverNumMed">11 / 13</td>
    </tr>
    <tr>
      <td class="coverFile"><a href="11/index.html">11</a></td>
      <td class="coverBar" align="center">
        <table border=0 cellspacing=0 cellpadding=1><tr><td class="coverBarOutline"><img src="amber.png" width=88 height=10 alt="87.5%"><img src="snow.png" width=12 height=10 alt="87.5%"></td></tr></table>
      </td>
      <td class="coverPerMed">87.5&nbsp;%</td>
      <td class="coverNumMed">49 / 56</td>
      <td class="coverPerLo">74.1&nbsp;%</td>
      <td class="coverNumLo">60 / 81</td>
    </tr>
    <tr>
      <td class="coverFile"><a href="11/bits/index.html">11/bits</a></td>
      <td class="coverBar" align="center">
        <table border=0 cellspacing=0 cellpadding=1><tr><td class="coverBarOutline"><img src="ruby.png" width=75 height=10 alt="74.9%"><img src="snow.png" width=25 height=10 alt="74.9%"></td></tr></table>
      </td>
      <td class="coverPerLo">74.9&nbsp;%</td>
      <td class="coverNumLo">1260 / 1682</td>
      <td class="coverPerMed">87.1&nbsp;%</td>
      <td class="coverNumMed">1191 / 1368</td>
    </tr>
    <tr>
      <td class="coverFile"><a href="11/ext/index.html">11/ext</a></td>
      <td class="coverBar" align="center">
        <table border=0 cellspacing=0 cellpadding=1><tr><td class="coverBarOutline"><img src="amber.png" width=82 height=10 alt="82.1%"><img src="snow.png" width=18 height=10 alt="82.1%"></td></tr></table>
      </td>
      <td class="coverPerMed">82.1&nbsp;%</td>
      <td class="coverNumMed">23 / 28</td>
      <td class="coverPerHi">93.1&nbsp;%</td>
      <td class="coverNumHi">108 / 116</td>
    </tr>
  </table>
  </center>
  <br>

  <table width="100%" border=0 cellspacing=0 cellpadding=0>
    <tr><td class="ruler"><img src="glass.png" width=3 height=3 alt=""></td></tr>
    <tr><td class="versionInfo">Generated by: <a href="http://ltp.sourceforge.net/coverage/lcov.php">LCOV version 1.14</a></td></tr>
  </table>
  <br>

</body>
</html>
```

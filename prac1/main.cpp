#include <iostream>

#include "automaton.h"

int main() {
  std::set<int> a = {1, 2, 4};
  std::set<int> b = {4, 1, 2};
  std::set<int> c = {1, 2};
  std::cout << (b == c);
}
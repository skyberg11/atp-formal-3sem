#include <gtest/gtest.h>

#include "automaton.h"

bool IsSameFiles(const std::string& name1, const std::string& name2) {
  std::fstream fin1(name1, std::fstream::in);
  std::fstream fin2(name2, std::fstream::in);
  std::string ch1, ch2;
  bool result = true;
  while (fin1 >> ch1 && fin2 >> ch2) {
    if (ch1 != ch2) {
      result = false;
      break;
    }
  }
  if (fin1 >> ch1 || fin2 >> ch2) {
    if (ch1 != ch2) return false;
  }
  return result;
}

TEST(BasicTests, OutIn) {
  std::fstream f(TESTDIR + std::string("/in.txt"));
  AutomatonBuilder* builder = new AutomatonBuilder;
  AutomatonT<kNondeterministic> a = builder->StreamBuildAutomaton(f);
  f.close();
  a.SaveFA(TESTDIR + std::string("/out.txt"));
  ASSERT_EQ(IsSameFiles(TESTDIR + std::string("/out.txt"),
                        TESTDIR + std::string("/expected.txt")),
            true);
}

TEST(TransformTests, Make01Symbols) {
  std::fstream f(TESTDIR + std::string("/in.txt"));
  AutomatonBuilder* builder = new AutomatonBuilder;
  AutomatonTransform* transformer = new AutomatonTransform;
  AutomatonT<kNondeterministic> source = builder->StreamBuildAutomaton(f);
  f.close();
  source = transformer->Make01Symbols(source);
  source.SaveFA(TESTDIR + std::string("/out.txt"));
  ASSERT_EQ(IsSameFiles(TESTDIR + std::string("/out.txt"),
                        TESTDIR + std::string("/expected01.txt")),
            true);
}

TEST(TransformTests, DFA) {
  std::fstream f(TESTDIR + std::string("/in.txt"));
  AutomatonBuilder* builder = new AutomatonBuilder;
  AutomatonTransform* transformer = new AutomatonTransform;
  AutomatonT<kNondeterministic> source = builder->StreamBuildAutomaton(f);
  f.close();
  source = transformer->Make01Symbols(source);
  source = transformer->EPSClosure(source);
  transformer->ExpandAcceptables(source);
  transformer->ExpandEdges(source);
  transformer->DestroyEPS(source);
  AutomatonT<kDeterministic> s = transformer->GetDFAThompson(source);
  s.SaveFA(TESTDIR + std::string("/out.txt"));
  ASSERT_EQ(IsSameFiles(TESTDIR + std::string("/out.txt"),
                        TESTDIR + std::string("/expectedDFA.txt")),
            true);
}

TEST(Correctness, DFA) {
  std::fstream f(TESTDIR + std::string("/in.txt"));
  AutomatonBuilder* builder = new AutomatonBuilder;
  AutomatonTransform* transformer = new AutomatonTransform;
  AutomatonT<kNondeterministic> source = builder->StreamBuildAutomaton(f);
  f.close();
  AutomatonT<kDeterministic> cmp = builder->BuildDFAFromNFA(source);
  ASSERT_EQ(source.GetWords(10), cmp.GetWords(10));
}

TEST(Correctness, MDFA) {
  std::fstream f(TESTDIR + std::string("/in.txt"));
  AutomatonBuilder* builder = new AutomatonBuilder;
  AutomatonTransform* transformer = new AutomatonTransform;
  AutomatonT<kNondeterministic> source = builder->StreamBuildAutomaton(f);
  f.close();
  AutomatonT<kDeterministic> cmp = builder->BuildDFAFromNFA(source);
  AutomatonT<kComplete> mn = builder->BuildMinCompleteFromNFA(source);
  ASSERT_EQ(mn.GetWords(10), cmp.GetWords(10));
}

int main(int argc, char** argv)

{
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
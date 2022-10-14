#include <string.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum AutoType { kNondeterministic = 0, kDeterministic = 1, kComplete = 2 };
enum NodeType { kStart = 0, kNone = 1, kTerminal = 2 };

template <AutoType T>
class AutomatonT {
 private:
  struct Node {
    Node(size_t id, NodeType type = kNone) : id(id), type(type) {}

    size_t id;
    NodeType type;
    std::unordered_map<std::string, std::unordered_set<size_t>> go;

    void AddEdge(size_t to, const std::string& word) { go[word].push_back(to); }
  };

 public:
  AutomatonT(std::set<std::string>& sigma) : m_sigma_(sigma) {
    m_start_node_id_ = -1;
  }

  size_t Size() const { return m_node_cnt_; }

  std::set<std::string>& GetSigma() const { return m_sigma_; }

  AutomatonT<kDeterministic>& operator=(const AutomatonT<kNondeterministic>&) =
      delete;

  AutomatonT<kComplete>& operator=(const AutomatonT<kNondeterministic>&) =
      delete;

  AutomatonT<kComplete>& operator=(const AutomatonT<kDeterministic>&) = delete;

 protected:
  // bool IsNodeExists(size_t index) {
  //   return m_nodes_set_.find(ptr) != m_nodes_set_.end();
  // }

  // bool IsNodeExists(const std::string& node_id) {
  //   return m_map_.find(node_id) != m_map_.end();
  // }

  size_T CreateNode() {
    size_t id = m_nodes_.size();
    m_nodes_.emplace_back(Node(id));
    ++m_node_cnt_;
    return id;
  }

  void AddEdge(size_t from, size_t to, const std::string& word = "#") {
    m_nodes_[from].AddEdge(to, word);
  }

  void SetStartNode(size_t id) {
    if (m_start_node_id_ != -1) {
      m_nodes_[m_start_node_id_].type = kNone;
    }
    m_nodes_[id].type = kStart;
    m_start_node_id_ = id;
  }

  void SetTerminalNode(size_t id) { m_nodes_[id]->type = kTerminal; }

  friend class AutomatonBuilder;
  friend class AutomatonTransform;

  size_t m_node_cnt_ = 0;
  size_t m_start_node_id_;
  std::vector<Node> m_nodes_;
  std::set<std::string> m_sigma_;
};

class AutomatonTransform {
  using Node = AutomatonT<kNondeterministic>::Node;

  void Helper01BFS(size_t cur, std::unordered_map<size_t, size_t>& mp,
                   std::unordered_map<size_t, bool>& visited,
                   AutomatonT<kNondeterministic>& zero_one_letter,
                   const AutomatonT<kNondeterministic>& source) {
    std::queue<size_t> heap;
    heap.push(cur);
    while (!heap.empty()) {
      cur = heap.front();
      heap.pop();
      size_t st = mp[cur];
      zero_one_letter.m_nodes_[st].type = source.m_nodes_[cur].type;
      visited[cur] = true;
      for (auto it : source.m_nodes_[cur].go) {
        std::string word = it.first;
        for (auto it1 : it.second) {
          if (!visited[it1]) {
            mp[it1] = zero_one_letter.CreateNode();
            heap.push(it1);
          }
          size_t fn = mp[it1];
          if (word.length() > 2) {
            for (size_t i = 0; i < word.length() - 1; ++i) {
              size_t temp = zero_one_letter.CreateNode();
              zero_one_letter.m_nodes_[st].AddEdge(temp,
                                                   std::string(1, word[i]));
              st = temp;
            }
            zero_one_letter.m_nodes_[st].AddEdge(
                fn, std::string(1, word[word.length() - 1]));
          } else {
            zero_one_letter.m_nodes_[st].AddEdge(fn, word);
          }
        }
      }
    }
  }

  std::unordered_set<size_t> HelperClosureDFS(
      size_t cur, std::unordered_map<size_t, bool>& visited,
      AutomatonT<kNondeterministic>& closure) {
    visited[cur] = true;
    std::unordered_set<size_t> eps;
    for (auto it : closure.m_nodes_[cur].go) {
      for (auto it1 : it.second) {
        if (it.first == "#") {
          eps.insert(it1);
          if (!visited[it1]) {
            eps.merge(HelperClosureDFS(it1, visited, closure));
          } else {
            if (closure.m_nodes_[it1].go.find("#") !=
                closure.m_nodes_[it1].go.end()) {
              eps.insert(closure.m_nodes_[cur].go["#"].begin(),
                         closure.m_nodes_[cur].go["#"].end());
            }
          }
        } else {
          if (!visited[it1]) {
            HelperClosureDFS(it1, visited, closure);
          }
        }
      }
    }
    closure.m_nodes_[cur].go.merge(eps);
    if (closure.m_nodes_[cur].go.find("#") != closure.m_nodes_[cur].go.end()) {
      eps = closure.m_nodes_[cur].go["#"];
    }
    return eps;
  }

 public:
  AutomatonT<kNondeterministic> Make01Symbols(
      const AutomatonT<kNondeterministic>& source) {
    AutomatonT<kNondeterministic> zero_one_letter(source.GetSigma());

    size_t cur_index = source.m_start_node_id_;
    std::unordered_map<size_t, size_t> mp;
    std::unordered_map<size_t, bool> visited;

    mp[cur_index] = zero_one_letter.CreateNode();
    Helper01BFS(cur_index, mp, visited, zero_one_letter, source);
    return zero_one_letter;
  }

  AutomatonT<kNondeterministic> EPSClosure(
      const AutomatonT<kNondeterministic>& source) {
    AutomatonT<kNondeterministic> closure = source;
    std::unordered_map<size_t, bool> visited;

    size_t cur = closure.m_start_node_id_;
    HelperClosureDFS(cur, visited, closure);
    return closure;
  }
};

class AutomatonBuilder {
  using Node = AutomatonT<kNondeterministic>::Node;

 public:
  virtual AutomatonT<kNondeterministic> StreamBuildAutomaton(
      std::ifstream& in) {
    std::set<std::string> sigma;
    std::string s;
    in >> s;
    if (s == "Sigma:") {
      in >> s;
      while (s != "#") {
        sigma.insert(s);
        in >> s;
      }
    } else {
      throw std::logic_error("Alphabet is not defined");
    }

    AutomatonT<kNondeterministic> automaton(sigma);
    in >> s;
    if (s == "Start:") {
      size_t number;
      in >> number;
      automaton.SetStartNode(number);
    } else {
      throw std::logic_error("Start is not defined");
    }
    in >> s;
    if (s == "Acceptance:") {
      size_t number;
      in >> s;
      while (s != "#") {
        std::stringstream sstream(s);
        size_t result;
        sstream >> result;
        automaton.SetTerminalNode(result);
      }
    } else {
      throw std::logic_error("Acceptance nodes are not defined");
    }
    in >> s;
    if (s == "--BEGIN--") {
      in >> s;
      while (s != "--END--") {
        size_t from;
        if (s == "State:") {
          in >> from;
        } else if (s == "->") {
          std::string word;
          size_t to;
          in >> word >> to;
          if (word == "EPS") {
            word = "#";
          }
          automaton.AddEdge(from, to, word);
        } else {
          throw std::exception();
        }
        in >> s;
      }
    }
    return automaton;
  }
  // std::unordered_map<std::string, std::vector<Node*>> map_n;

  /*std::vector<Node*> m_nodes_;
  std::unordered_map<std::string, size_t> m_map_;
  std::set<std::string> m_sigma_;*/
};

/*
Sigma: a b c d e #
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
*/
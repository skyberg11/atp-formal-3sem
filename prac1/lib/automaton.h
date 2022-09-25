#include <string.h>

#include <fstream>
#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>

enum auto_type { nondeterministic, deterministic, complete };
enum node_type { start, none, terminal };

template <auto_type T>
class automaton_t {
 public:
  struct Node {
    Node(std::string& name, size_t index) : name_n(name) {}

    std::string name_n;
    size_t index_n;
    std::unordered_map<std::string, std::vector<Node*>> map_n;
    node_type type_n = none;
  };

  automaton_t(std::set<std::string> sigma) : m_sigma_(sigma) {}

  ~automaton_t() {
    for (size_t i = 0; i < m_nodes_.size(); ++i) {
      delete m_nodes_[i];
    }
  }

  automaton_t& operator=(automaton_t&) = delete;

  size_t Size() const { return m_node_cnt_; }

  std::set<std::string>& GetSigma() const { return m_sigma_; }

 protected:
  bool IsNodeExists(std::string& node_id) {
    return m_map_.find(node_id) != m_map_.end();
  }

  bool CreateNode(std::string& node_id) {
    if (IsNodeExists(node_id)) {
      return false;
    }
    m_map_[node_id] = m_nodes_.size();
    m_nodes_.push_back(new Node(node_id, m_nodes_.size()));
    ++m_node_cnt_;
    return true;
  }

  void AddEdge(std::string& from_id, std::string& to_id,
               std::string& word = "#") {
    if (!IsNodeExists(from_id)) {
      CreateNode(from_id);
    }
    if (!IsNodeExists(to_id)) {
      CreateNode(to_id);
    }
    m_nodes_[m_map_[from_id]]->map_n[word].push_back(m_nodes_[m_map_[to_id]]);
  }

  void SetStartNode(std::string& node_id) {
    if (!IsNodeExists(node_id)) {
      CreateNode(node_id);
    }
    m_start_node_ = m_nodes_[m_map_[node_id]];
    m_start_node_->type_n = start;
  }

  void SetTerminalNode(std::string& node_id) {
    if (!IsNodeExists(node_id)) {
      CreateNode(node_id);
    }
    Node* temp = m_nodes_[m_map_[node_id]];
    temp->type_n = terminal;
  }

  friend class AutomatonBuilder;

  size_t m_node_cnt_;
  Node* m_start_node_ = nullptr;
  std::vector<Node*> m_nodes_;
  std::unordered_map<std::string, size_t> m_map_;
  std::set<std::string> m_sigma_;
};

class AutomatonBuilder {
 public:
  using Node_t = automaton_t<nondeterministic>::Node;

  virtual automaton_t<nondeterministic> StreamBuildAutomaton(
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
    }

    automaton_t<nondeterministic> automaton(sigma);
    in >> s;
    if (s == "Start:") {
      in >> s;
      automaton.SetStartNode(s);
    }
    in >> s;
    if (s == "Acceptance:") {
      in >> s;
      while (s != "#") {
        automaton.SetTerminalNode(s);
        in >> s;
      }
    }
    in >> s;
    if (s == "--BEGIN--") {
      in >> s;
      while (s != "--END--") {
        std::string from;
        if (s == "State:") {
          in >> from;
        } else if (s == "->") {
          std::string word, to;
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

  virtual automaton_t<deterministic> BuildDeterministic(
      automaton_t<nondeterministic>& input) {
    automaton_t<nondeterministic> zero_one_letter(input.GetSigma());
    Node_t* start = input.m_start_node_;
    std::vector<Node_t*> m_nodes_;
    }
  virtual automaton_t<complete> BuildComplete(automaton_t<deterministic>) = 0;

 private:
  void HelperBuildDeterministic(Node_t* cur) {
    for (auto it = cur->map_n.begin(); it != cur->map_n.end(); ++it) {
      if (it->first.length() > 2) {
      }
    }
  }
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
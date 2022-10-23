#include <string.h>

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
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
        std::map<std::string, std::set<size_t>> go;

        void AddEdge(size_t to, const std::string& word) {
            go[word].insert(to);
        }
    };

  public:
    AutomatonT(const std::set<std::string>& sigma) : m_sigma_(sigma) {
        m_start_node_id_ = -1;
    }

    size_t Size() const { return m_node_cnt_; }

    std::set<std::string> GetSigma() const { return m_sigma_; }

    template <AutoType U>
    AutomatonT<T>& operator=(const AutomatonT<U>&) = delete;

    std::set<std::string> GetWords(size_t depth) {
        std::set<std::string> words = {""};
        std::set<std::string> ans;
        words = GetWordsCycle(words, ans, m_start_node_id_, 0, depth);
        return ans;
    }

    void SaveFA(const std::string& file_name) const {
        std::fstream fs(file_name, std::fstream::out | std::fstream::trunc);
        fs << "DOA: v1\n";
        fs << "Start: " << m_start_node_id_ << '\n';
        fs << "Acceptance: ";
        bool f = true;
        for (int i = 0; i < m_node_cnt_; ++i) {
            if (m_nodes_[i].type == kTerminal) {
                if (f) {
                    fs << m_nodes_[i].id << ' ';
                    f = false;
                } else {
                    fs << "& " << m_nodes_[i].id << ' ';
                }
            }
        }
        //fs << "#\n";
        fs << '\n';
        fs << "--BEGIN--\n";
        for (int i = 0; i < m_node_cnt_; ++i) {
            fs << "State: " << m_nodes_[i].id << '\n';
            for (auto it : m_nodes_[i].go) {
                if (!it.second.empty()) {

                    for (auto it1 : it.second) {
                        fs << "    -> " << (it.first == "#" ? "EPS" : it.first)
                           << ' ' << it1 << '\n';
                    }
                }
            }
        }
        fs << "--END--\n";
        fs.close();
    }

  protected:
    std::set<std::string> GetWordsCycle(std::set<std::string> from,
                                        std::set<std::string>& read,
                                        size_t index, int cur_depth,
                                        int depth) {
        if (cur_depth >= depth)
            return from;
        std::set<std::string> to_ret;
        std::set<std::string> cp;
        if (m_nodes_[index].type == kTerminal) {
            read.insert(from.begin(), from.end());
        }
        for (auto it : m_nodes_[index].go) {
            for (auto word : from) {
                word += (it.first == "#" ? "" : it.first);
                if (word.length() > depth)
                    continue;
                cp.insert(word);
            }
            for (auto it1 : it.second) {
                std::set<std::string> cp1 = GetWordsCycle(
                    cp, read, it1,
                    cur_depth + (it.first == "#" ? 0 : it.first.length()),
                    depth);
                to_ret.insert(cp1.begin(), cp1.end());
            }
            cp.clear();
        }

        return to_ret;
    }

    size_t CreateNode() {
        size_t id = m_nodes_.size();
        m_nodes_.emplace_back(Node(id));
        ++m_node_cnt_;
        return id;
    }

    void AddEdge(size_t from, size_t to, const std::string& word = "#") {
        while (std::max(from, to) >= m_node_cnt_) {
            CreateNode();
        }
        m_nodes_[from].AddEdge(to, word);
    }

    void SetStartNode(size_t id) {
        while (id >= m_node_cnt_) {
            CreateNode();
        }
        if (m_start_node_id_ != -1) {
            m_nodes_[m_start_node_id_].type = kNone;
        }
        m_nodes_[id].type = kStart;
        m_start_node_id_ = id;
    }

    void SetTerminalNode(size_t id) {
        while (id >= m_node_cnt_) {
            CreateNode();
        }
        m_nodes_[id].type = kTerminal;
    }

    friend class AutomatonBuilder;
    friend class AutomatonTransform;

    size_t m_node_cnt_ = 0;
    size_t m_start_node_id_;
    std::vector<Node> m_nodes_;
    std::set<std::string> m_sigma_;
};

class AutomatonTransform {
    using Node = AutomatonT<kNondeterministic>::Node;

    void Helper01BFS(size_t cur, std::map<size_t, size_t>& mp,
                     std::map<size_t, bool>& visited,
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
                    if (word.length() >= 2) {
                        for (size_t i = 0; i < word.length() - 1; ++i) {
                            size_t temp = zero_one_letter.CreateNode();
                            zero_one_letter.m_nodes_[st].AddEdge(
                                temp, std::string(1, word[i]));
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

    std::set<size_t> HelperClosureDFS(size_t cur, std::set<size_t>& visited,
                                      AutomatonT<kNondeterministic>& closure) {
        visited.insert(cur);
        std::set<size_t> eps;
        for (auto it : closure.m_nodes_[cur].go) {
            for (auto it1 : it.second) {
                if (it.first == "#") {
                    eps.insert(it1);
                    if (visited.find(it1) == visited.end()) {
                        std::set<size_t> temp =
                            HelperClosureDFS(it1, visited, closure);
                        eps.insert(temp.begin(), temp.end());
                    } else {
                        if (closure.m_nodes_[it1].go.find("#") !=
                            closure.m_nodes_[it1].go.end()) {
                            eps.insert(closure.m_nodes_[cur].go["#"].begin(),
                                       closure.m_nodes_[cur].go["#"].end());
                        }
                    }
                } else {
                    if (visited.find(it1) == visited.end()) {
                        HelperClosureDFS(it1, visited, closure);
                    }
                }
            }
        }
        closure.m_nodes_[cur].go["#"].insert(eps.begin(), eps.end());
        return closure.m_nodes_[cur].go["#"];
    }

    void ExpandAccDFS(size_t cur, std::set<size_t>& visited,
                      AutomatonT<kNondeterministic>& source) {
        visited.insert(cur);
        for (auto it : source.m_nodes_[cur].go) {
            for (auto it1 : it.second) {
                if (source.m_nodes_[it1].type == kTerminal && it.first == "#") {
                    source.m_nodes_[cur].type = kTerminal;
                }
                if (visited.find(it1) == visited.end()) {
                    ExpandAccDFS(it1, visited, source);
                }
            }
        }
    }

    void DestroyEPSDFS(size_t cur, std::set<size_t>& visited,
                       AutomatonT<kNondeterministic>& source) {
        visited.insert(cur);
        for (auto it : source.m_nodes_[cur].go) {
            for (auto it1 : it.second) {
                if (visited.find(it1) == visited.end()) {
                    DestroyEPSDFS(it1, visited, source);
                }
            }
        }
        source.m_nodes_[cur].go.erase("#");
    }

    std::map<std::string, std::set<size_t>> ExpandEdgesDFS(
        size_t cur, std::set<size_t>& visited,
        AutomatonT<kNondeterministic>& source) {
        visited.insert(cur);
        std::map<std::string, std::set<size_t>> complement;
        for (auto it : source.m_nodes_[cur].go) {
            for (auto it1 : it.second) {
                if (visited.find(it1) == visited.end()) {
                    if (it.first == "#") {
                        std::map<std::string, std::set<size_t>> tmp;
                        tmp = ExpandEdgesDFS(it1, visited, source);
                        complement.insert(tmp.begin(), tmp.end());
                    } else {
                        ExpandEdgesDFS(it1, visited, source);
                    }
                } else {
                    if (it.first == "#") {
                        std::map<std::string, std::set<size_t>> tmp;
                        tmp = source.m_nodes_[it1].go;
                        complement.insert(tmp.begin(), tmp.end());
                    }
                }
            }
        }
        source.m_nodes_[cur].go.insert(complement.begin(), complement.end());
        return source.m_nodes_[cur].go;
    }

  public:
    AutomatonT<kNondeterministic> Make01Symbols(
        const AutomatonT<kNondeterministic>& source) {
        AutomatonT<kNondeterministic> zero_one_letter(source.GetSigma());

        size_t cur_index = source.m_start_node_id_;
        std::map<size_t, size_t> mp;
        std::map<size_t, bool> visited;

        mp[cur_index] = zero_one_letter.CreateNode();
        zero_one_letter.SetStartNode(mp[cur_index]);

        Helper01BFS(cur_index, mp, visited, zero_one_letter, source);
        return zero_one_letter;
    }

    AutomatonT<kNondeterministic> EPSClosure(
        const AutomatonT<kNondeterministic>& source) {
        AutomatonT<kNondeterministic> closure = source;
        std::set<size_t> visited;

        size_t cur = closure.m_start_node_id_;
        HelperClosureDFS(cur, visited, closure);
        return closure;
    }

    void ExpandAcceptables(AutomatonT<kNondeterministic>& source) {
        std::set<size_t> visited;
        size_t cur_index = source.m_start_node_id_;

        ExpandAccDFS(cur_index, visited, source);
    }

    void ExpandEdges(AutomatonT<kNondeterministic>& source) {
        std::set<size_t> visited;
        size_t cur_index = source.m_start_node_id_;

        ExpandEdgesDFS(cur_index, visited, source);
    }

    void DestroyEPS(AutomatonT<kNondeterministic>& source) {
        std::set<size_t> visited;
        size_t cur_index = source.m_start_node_id_;

        DestroyEPSDFS(cur_index, visited, source);
    }

    AutomatonT<kDeterministic> Thompson(AutomatonT<kNondeterministic>& source) {
        std::map<std::set<size_t>, size_t> mp;
        std::queue<std::set<size_t>> q;
        AutomatonT<kDeterministic> ans(source.GetSigma());
        size_t cur = source.m_start_node_id_;
        mp.insert({{cur}, ans.CreateNode()});
        q.push({cur});
        ans.SetStartNode(0);

        size_t dummy = ans.CreateNode();

        while (!q.empty()) {
            std::set<size_t> cur = q.front();

            q.pop();
            for (auto letter : source.m_sigma_) {
                std::set<size_t> temp;
                bool isTerm = false;
                for (auto it : cur) {
                    if (source.m_nodes_[it].type == kTerminal) {
                        isTerm = true;
                    }
                    if (source.m_nodes_[it].go.find(letter) !=
                        source.m_nodes_[it].go.end())
                        temp.insert(source.m_nodes_[it].go[letter].begin(),
                                    source.m_nodes_[it].go[letter].end());
                }
                if (temp.empty()) {
                    ans.AddEdge(mp[cur], dummy, letter);
                    continue;
                }
                if (mp.find(temp) == mp.end()) {
                    mp.insert({temp, ans.CreateNode()});
                    q.push(temp);
                }

                if (isTerm) {
                    ans.SetTerminalNode(mp[cur]);
                }
                ans.AddEdge(mp[cur], mp[temp], letter);
            }
        }
        for (auto letter : source.m_sigma_) {
            ans.AddEdge(dummy, dummy, letter);
        }

        return ans;
    }

    AutomatonT<kComplete> Minimal(AutomatonT<kDeterministic>& source) {
        using Node = AutomatonT<kDeterministic>::Node;
        struct Element {
            size_t index;
            size_t q;
            size_t letters_sz;

            std::vector<size_t> letters_q;
            std::vector<std::string> letters;
            bool operator==(const Element& other) const {
                return q == other.q && letters_q == other.letters_q;
            }
            bool operator!=(const Element& other) const {
                return !(*this == other);
            }
            bool operator<(const Element& other) const {
                if (q == other.q) {
                    size_t index = 0;
                    while (index < letters_sz) {
                        if (letters_q[index] != other.letters_q[index]) {
                            return letters_q[index] < other.letters_q[index];
                        }
                        ++index;
                    }
                    return false;
                } else {
                    return q < other.q;
                }
            }
            bool operator<=(const Element& other) const {
                return *this < other || *this == other;
            }
            bool operator>(const Element& other) const {
                return !(*this <= other);
            }
            bool operator>=(const Element& other) const {
                return *this > other || *this == other;
            }
        };
        std::map<size_t, size_t> node_cl;

        std::vector<Element> equiv;
        for (size_t i = 0; i < source.m_nodes_.size(); ++i) {
            Node* temp = &(source.m_nodes_[i]);
            Element cur;
            cur.index = temp->id;
            cur.letters_sz = 0;
            if (temp->type == kTerminal) {
                cur.q = 0;
            } else {
                cur.q = 1;
            }
            node_cl[cur.index] = cur.q;
            for (auto it : temp->go) {
                for (size_t ind : it.second) {
                    cur.letters.push_back(it.first);
                    cur.letters_q.push_back(
                        (source.m_nodes_[ind].type == kTerminal) ? 0 : 1);
                    ++cur.letters_sz;
                }
            }
            equiv.push_back(cur);
        }
        std::sort(equiv.begin(), equiv.end());

        std::vector<Element> cur = equiv;

        do {
            equiv = cur;
            cur.clear();
            size_t cl = 0;
            for (size_t i = 0; i < equiv.size(); ++i) {
                Element temp;
                if (i > 0) {
                    if (equiv[i] != equiv[i - 1]) {
                        ++cl;
                    }
                }
                temp.q = cl;
                temp.letters = equiv[i].letters;
                temp.letters_sz = 0;
                temp.index = equiv[i].index;
                node_cl[temp.index] = cl;
                cur.push_back(temp);
            }
            for (size_t i = 0; i < cur.size(); ++i) {
                for (auto it : source.m_nodes_[cur[i].index].go) {
                    for (size_t ind : it.second) {
                        cur[i].letters.push_back(it.first);
                        cur[i].letters_q.push_back(node_cl[ind]);
                        ++cur[i].letters_sz;
                    }
                }
            }
            std::sort(cur.begin(), cur.end());
        } while (cur != equiv);
        AutomatonT<kComplete> minim(source.GetSigma());
        for (size_t i = 0; i < equiv.size(); ++i) {
            if (source.m_nodes_[equiv[i].index].type == kStart) {
                minim.SetStartNode(equiv[i].q);
            } else if (source.m_nodes_[equiv[i].index].type == kTerminal) {
                minim.SetTerminalNode(equiv[i].q);
            }
            for (size_t j = 0; j < equiv[i].letters_q.size(); ++j) {
                minim.AddEdge(equiv[i].q, equiv[i].letters_q[j],
                              equiv[i].letters[j]);
            }
        }
        return minim;
    }
};

class AutomatonBuilder {
    using Node = AutomatonT<kNondeterministic>::Node;

  public:
    AutomatonT<kDeterministic> BuildDFAFromNFA(
        AutomatonT<kNondeterministic> source) {
        AutomatonTransform abstract_transformer;
        source = abstract_transformer.Make01Symbols(source);
        source = abstract_transformer.EPSClosure(source);
        abstract_transformer.ExpandAcceptables(source);
        abstract_transformer.ExpandEdges(source);
        abstract_transformer.DestroyEPS(source);
        AutomatonT<kDeterministic> b = abstract_transformer.Thompson(source);
        return b;
    }

    AutomatonT<kComplete> BuildMinCompleteFromNFA(
        AutomatonT<kNondeterministic> source) {

        AutomatonTransform abstract_transformer;
        AutomatonT<kDeterministic> b = BuildDFAFromNFA(source);
        AutomatonT<kComplete> c = abstract_transformer.Minimal(b);
        return c;
    }
    virtual AutomatonT<kNondeterministic> StreamBuildAutomaton(
        std::fstream& in) {
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
            in >> s;
            while (s != "#") {
                std::stringstream sstream(s);
                size_t result;
                sstream >> result;
                automaton.SetTerminalNode(result);
                in >> s;
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
};
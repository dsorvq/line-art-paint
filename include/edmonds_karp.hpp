#pragma once

#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include <stack>
#include <cassert>

template <class flow_t>
class EdmondsKarp {
public:
    EdmondsKarp() = delete;
    EdmondsKarp(int V);
    ~EdmondsKarp() = default;

    void add_directional_edge(int u, int v, flow_t capacity);
    void add_bidirectional_edge(int u, int v, flow_t capacity);

    auto max_flow(int source, int sink) -> flow_t;
    // returns edges in minimum cut in form <capacity, <node_from, node_to>>
    auto min_cut(int source) -> std::vector<std::pair<int, int>>;
    // reachable=1 unreachable=0
    auto partition(int source) -> std::vector<int>;

    int V() const {return V_;}

private:
    auto bfs(int source, int sink) -> flow_t;

    // TODO: capacity can possible be stored in adj with struct::edges
private: 
    int V_ {};
    bool flow_called_ {false};

    std::vector<std::vector<int>> adj_;
    std::vector<std::vector<flow_t>> capacity_;
    std::vector<int> parent_;

    static constexpr flow_t flow_infty {std::numeric_limits<flow_t>::max()}; // /10;
    static constexpr int unvisited = -1;
    static constexpr int noparent = -2;
};

template <class flow_t>
EdmondsKarp<flow_t>::EdmondsKarp(int V) 
    : V_ {V}
    , flow_called_ {false}
    , adj_ (V_)
    , capacity_ (V_, std::vector<flow_t>(V_))
    , parent_ (V_)
{ }

template <class flow_t>
void EdmondsKarp<flow_t>::add_directional_edge(int u, int v, flow_t capacity) {
    assert(0 <= std::min(u, v) && std::max(u, v) < V_);
    assert(capacity >= 0);
    if (capacity_[u][v]) {
        capacity_[u][v] += capacity;
        return;
    }
    adj_[u].push_back(v);
    adj_[v].push_back(u);
    capacity_[u][v] = capacity;
}

template <class flow_t>
void EdmondsKarp<flow_t>::add_bidirectional_edge(int u, int v, flow_t capacity) {
    assert(0 <= std::min(u, v) && std::max(u, v) < V_);
    assert(capacity >= 0);
    adj_[u].push_back(v);
    adj_[v].push_back(u);
    capacity_[u][v] = capacity;
    capacity_[v][u] = capacity;
}

template <class flow_t>
auto EdmondsKarp<flow_t>::max_flow(int source, int sink) -> flow_t {
    flow_t flow = 0;
 
    flow_t augm_flow = 0;
    while ((augm_flow = bfs(source, sink))) {
        flow += augm_flow;
        auto to = sink;
        while (to != source) {
            auto from = parent_[to];
            capacity_[from][to] -= augm_flow;
            capacity_[to][from] += augm_flow;
            to = from;
        }
    }
    
    flow_called_ = true;
    return flow;    
}

template <class flow_t>
auto EdmondsKarp<flow_t>::min_cut(int source) -> std::vector<std::pair<int, int>> {
    assert(flow_called_);
    std::vector<std::pair<int, int>> cut;

    bfs(source, -3); // -3 is never reachable
                     
    for (int u = 0; u < V_; ++u) {
        for (auto v : adj_[u]) {
            if (parent_[u] != unvisited and parent_[v] == unvisited) {
                cut.push_back({u, v});
            }
        }
    }

    return cut;
}

template <class flow_t>
auto EdmondsKarp<flow_t>::partition(int source) -> std::vector<int> {
    assert(flow_called_);

    bfs(source, -3);
    std::vector<int> partition(V_);
    for (int i = 0; i != V_; ++i) {
        if (parent_[i] == unvisited) {
            partition[i] = 0;
        }
        else {
            partition[i] = 255;
        }
    }

    return partition;
}
template <class flow_t>
auto EdmondsKarp<flow_t>::bfs(int source, int sink) -> flow_t {
    std::fill(begin(parent_), end(parent_), unvisited);
    parent_[source] = noparent;

    std::queue<std::pair<int, flow_t>> q;
    q.push({source, flow_infty});
 
    while (!q.empty()) {
        auto [from, from_flow] = q.front();
        q.pop();

        for (auto to : adj_[from]) {
            if (parent_[to] != unvisited or capacity_[from][to] == 0)
                continue;

            parent_[to] = from;
            auto to_flow = std::min(from_flow, capacity_[from][to]);
            if (to == sink) {
                return to_flow;
            }
            q.push({to, to_flow});
        }
    }
    
    return 0;
}


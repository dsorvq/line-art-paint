#pragma once

#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include <stack>
#include <cassert>

enum EdgeType {
    DIRECTIONAL, DIRECTIONAL_REVERSE, BIDIRECTIONAL   
};

template <class flow_t>
class Dinic {
public:
    Dinic() = delete;
    Dinic(int V);
    ~Dinic() = default;

    void add_directional_edge(int u, int v, flow_t capacity);
    void add_bidirectional_edge(int u, int v, flow_t capacity);

    auto V() const -> int { return V_; }

    auto max_flow(int source, int sink) -> flow_t;
    // returns edges in minimum cut in form <capacity, <node_from, node_to>>
    auto min_cut(int source) -> std::vector<std::pair<int, int>>;
    // reachable=1 unreachable=0
    auto partition(int source) -> std::vector<bool>;

private:
    struct Edge {
        int node{};
        int rev{};
        flow_t capacity{};
        EdgeType type{};

        Edge(int _node, int _rev, flow_t _capacity, EdgeType _type)
            : node{_node}, rev{_rev}, capacity{_capacity}, type{_type} 
        {}
    };

    auto reverse_edge(const Edge& e) -> Edge& {
        return adj_[e.node][e.rev];
    }

    auto bfs(int source, int sink) -> bool;
    auto dfs(int node, flow_t path_cap, int sink) -> flow_t;

private: 
    int V_ {};
    bool flow_called_ {false};
    std::vector<std::vector<Edge>> adj_;
    std::vector<int> level_;
    std::vector<int> edge_id_;

    static constexpr flow_t flow_infty {std::numeric_limits<flow_t>::max()}; // /10;
    static constexpr int id_infty {std::numeric_limits<int>::max()};
    static constexpr int unvisited = -1;
    static constexpr int noparent = -2;
};

template <class flow_t>
Dinic<flow_t>::Dinic(int V) 
    : V_ {V}
    , flow_called_ {false}
    , adj_(V)
    , level_(V)
{ }

template <class flow_t>
void Dinic<flow_t>::add_directional_edge(int u, int v, flow_t capacity) {
    assert(0 <= std::min(u, v) && std::max(u, v) < V_);
    assert(capacity >= 0);

    Edge uv {v, static_cast<int>(adj_[v].size()), capacity, DIRECTIONAL};
    Edge vu {u, static_cast<int>(adj_[u].size()), 0, DIRECTIONAL_REVERSE};

    adj_[u].push_back(uv);
    adj_[v].push_back(vu);
}

template <class flow_t>
void Dinic<flow_t>::add_bidirectional_edge(int u, int v, flow_t capacity) {
    assert(0 <= std::min(u, v) && std::max(u, v) < V_);
    assert(capacity >= 0);
    Edge uv {v, static_cast<int>(adj_[v].size()), capacity, BIDIRECTIONAL};
    Edge vu {u, static_cast<int>(adj_[u].size()), capacity, BIDIRECTIONAL};

    adj_[u].push_back(uv);
    adj_[v].push_back(vu);
}

template <class flow_t>
auto Dinic<flow_t>::max_flow(int source, int sink) -> flow_t {
    flow_t flow = 0;
 
    flow_t flow_cap = std::numeric_limits<flow_t>::max();
    while (flow_cap > 0 and bfs(source, sink)) {
        edge_id_.assign(V_, 0);
        flow_t increment = dfs(source, flow_cap, sink);
        assert(increment > 0);
        flow += increment;
        flow_cap -= increment;
    }
    
    flow_called_ = true;
    return flow;    
}

template <class flow_t>
auto Dinic<flow_t>::min_cut(int source) -> std::vector<std::pair<int, int>> {
    assert(flow_called_);

    auto reachable = partition(source);

    std::vector<std::pair<int, int>> cut;

    for (int node = 0; node < V_; node++)
        for (auto& e : adj_[node])
            if (reachable[node] && !reachable[e.node] && e.type != DIRECTIONAL_REVERSE) {
                cut.push_back({node, e.node});
            }

    return cut;
}

template <class flow_t>
auto Dinic<flow_t>::partition(int source) -> std::vector<bool> {
    assert(flow_called_);

    std::vector<bool> partition(V_, false); 
    partition[source] = true;

    std::stack<int> st;
    st.push(source);
    while (!st.empty()) {
        auto cur = st.top();
        st.pop();
        for (auto& e : adj_[cur]) {
            if (e.capacity > 0 and !partition[e.node]) {
                partition[e.node] = true;
                st.push(e.node);
            }
        }
    }
    
    return partition;
}

template <class flow_t>
auto Dinic<flow_t>::bfs(int source, int sink) -> bool {
    std::vector<int> q(V_); 
    int q_start = 0, q_end = 0;
    level_.assign(V_, id_infty);

    auto bfs_check = [&](int node, int new_dist) -> void {
        if (new_dist < level_[node]) {
            level_[node] = new_dist;
            q[q_end++] = node;
        }
    };

    bfs_check(source, 0);

    while (q_start < q_end) {
        int top = q[q_start++];

        for (Edge &e : adj_[top])
            if (e.capacity > 0)
                bfs_check(e.node, level_[top] + 1);
    }

    return level_[sink] < id_infty;
}

template <class flow_t>
auto Dinic<flow_t>::dfs(int node, flow_t path_cap, int sink) -> flow_t {
    if (node == sink)
        return path_cap;
    if (level_[node] >= level_[sink])
        return 0;

    flow_t flow{};

    while (edge_id_[node] < int(adj_[node].size())) {
        auto& e = adj_[node][edge_id_[node]];

        if (e.capacity > 0 && level_[node] + 1 == level_[e.node]) {
            flow_t path = dfs(e.node, std::min(path_cap, e.capacity), sink);
            path_cap -= path;
            e.capacity -= path;
            reverse_edge(e).capacity += path;
            flow += path;
        }

        if (path_cap == 0)
            break;

        edge_id_[node]++;
    }

    return flow;
}

#include <bits/stdc++.h>
using namespace std;

// --- Optimized Tarjan's algorithm for biconnected components (BCC) ---
int n, m_unique;
vector<vector<int>> g;
vector<int> entry, low, vis, stamp;
vector<pair<int,int>> st;
int timer = 0, bcc_cnt = 0, cc_count = 0;
int max_bcc = 0, max_bcc_edges = 0;
int cur_stamp = 1;

void dfs(int u, int parent) {
    entry[u] = low[u] = ++timer;
    vis[u] = 1;
    for (int v : g[u]) {
        if (!entry[v]) {
            st.emplace_back(u, v);
            dfs(v, u);
            low[u] = min(low[u], low[v]);
            if (low[v] >= entry[u]) {
                // extract one BCC
                int edge_count = 0;
                vector<int> bcc_vertices;
                bcc_vertices.reserve(16);
                while (true) {
                    auto [p, q] = st.back();
                    st.pop_back();
                    edge_count++;
                    if (stamp[p] != cur_stamp) {
                        stamp[p] = cur_stamp;
                        bcc_vertices.push_back(p);
                    }
                    if (stamp[q] != cur_stamp) {
                        stamp[q] = cur_stamp;
                        bcc_vertices.push_back(q);
                    }
                    if (p == u && q == v) break;
                }
                cur_stamp++;
                int vertex_count = (int)bcc_vertices.size();
                max_bcc = max(max_bcc, vertex_count);
                max_bcc_edges = max(max_bcc_edges, edge_count);
                bcc_cnt++;
            }
        } else if (v != parent && entry[v] < entry[u]) {
            st.emplace_back(u, v);
            low[u] = min(low[u], entry[v]);
        }
    }
}

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <graph.mtx>\n";
        return 1;
    }
    ifstream fin(argv[1]);
    if (!fin) {
        cerr << "Cannot open file " << argv[1] << "\n";
        return 1;
    }

    string line;
    // Skip header/comments
    while (getline(fin, line)) {
        if (!line.empty() && line[0] != '%') break;
    }
    istringstream iss(line);
    int rows, cols, nnz;
    iss >> rows >> cols >> nnz;
    n = rows;

    if (nnz >= 20000000) {
        cout << argv[1] << "," << n << "," << nnz << ",Graph too large\n";
        return 0;
    }
    if (nnz <= 500000) {
        cout << argv[1] << "," << n << "," << nnz << ",Graph too small\n";
        return 0;
    }

    // Read edges (handle possible weights) and deduplicate
    vector<pair<int,int>> edges;
    edges.reserve(nnz);
    int count = 0;
    while (count < nnz && getline(fin, line)) {
        if (line.empty() || line[0] == '%') continue;
        istringstream ls(line);
        int u, v;
        if (!(ls >> u >> v)) continue;
        count++;
        // ignore any additional tokens (e.g., weights)
        if (u == v) continue;
        if (u > v) swap(u, v);
        edges.emplace_back(u, v);
    }
    sort(edges.begin(), edges.end());
    edges.erase(unique(edges.begin(), edges.end()), edges.end());
    m_unique = (int)edges.size();

    // Build adjacency lists
    g.assign(n + 1, {});
    for (auto &e : edges) {
        g[e.first].push_back(e.second);
        g[e.second].push_back(e.first);
    }
    st.reserve(m_unique);

    // Allocate Tarjan arrays
    entry.assign(n + 1, 0);
    low.assign(n + 1, 0);
    vis.assign(n + 1, 0);
    stamp.assign(n + 1, 0);

    // Run DFS for CC + BCC detection
    for (int i = 1; i <= n; i++) {
        if (!entry[i]) {
            cc_count++;
            dfs(i, 0);
        }
    }

    double sbcc = double(max_bcc + max_bcc_edges) / double(n + 2LL * m_unique);
    cout << argv[1] << "," << n << "," << m_unique << ","
         << cc_count << "," << bcc_cnt << ","
         << max_bcc << "," << max_bcc_edges << ","
         << fixed << setprecision(6) << sbcc << "\n";

    return 0;
}

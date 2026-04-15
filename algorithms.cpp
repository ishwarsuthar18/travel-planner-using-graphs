#include "algorithms.h"
#include "ui.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <climits>
#include <string>

const double INF = 1e18;

// ─────────────────────────────────────────────
//  Helper: edge weight selector
// ─────────────────────────────────────────────
static double weight(const Edge& e, OptCriteria c) {
    if (c == BY_TIME) return e.time;
    if (c == BY_COST) return e.cost;
    return e.distance;
}

// ─────────────────────────────────────────────
//  MinHeap
// ─────────────────────────────────────────────
MinHeap::MinHeap(int cap) : size(0), capacity(cap) {
    data = new HeapNode[cap];
}
MinHeap::~MinHeap() { delete[] data; }

void MinHeap::heapifyUp(int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (data[p].key > data[i].key) {
            HeapNode tmp = data[p]; data[p] = data[i]; data[i] = tmp;
            i = p;
        } else break;
    }
}
void MinHeap::heapifyDown(int i) {
    while (true) {
        int smallest = i, l = 2*i+1, r = 2*i+2;
        if (l < size && data[l].key < data[smallest].key) smallest = l;
        if (r < size && data[r].key < data[smallest].key) smallest = r;
        if (smallest == i) break;
        HeapNode tmp = data[i]; data[i] = data[smallest]; data[smallest] = tmp;
        i = smallest;
    }
}
void MinHeap::push(double key, int city) {
    data[size++] = {key, city};
    heapifyUp(size - 1);
}
HeapNode MinHeap::pop() {
    HeapNode top = data[0];
    data[0] = data[--size];
    heapifyDown(0);
    return top;
}

// ─────────────────────────────────────────────
//  Stack
// ─────────────────────────────────────────────
Stack::Stack(int cap) : top(-1), capacity(cap) { data = new StackNode[cap]; }
Stack::~Stack() { delete[] data; }
void Stack::push(StackNode n) { data[++top] = n; }
StackNode Stack::pop() { return data[top--]; }

// ─────────────────────────────────────────────
//  Queue
// ─────────────────────────────────────────────
Queue::Queue(int cap) : head(0), tail(0), cap(cap) { data = new QueueNode[cap]; }
Queue::~Queue() { delete[] data; }
void Queue::push(QueueNode n) { data[tail++ % cap] = n; }
QueueNode Queue::pop() { return data[head++ % cap]; }

// ─────────────────────────────────────────────
//  Helper: reconstruct path result
// ─────────────────────────────────────────────
static PathResult buildResult(const Graph& g, const std::vector<int>& path,
                               int steps, const std::string& name) {
    PathResult r;
    r.path          = path;
    r.found         = !path.empty();
    r.stepsExplored = steps;
    r.algoName      = name;
    r.totalDist = r.totalTime = r.totalCost = 0;

    for (int i = 0; i + 1 < (int)path.size(); i++) {
        int u = path[i], v = path[i+1];
        // find best direct edge
        for (const Edge& e : g.getEdges(u)) {
            if (e.to == v) {
                r.totalDist += e.distance;
                r.totalTime += e.time;
                r.totalCost += e.cost;
                break;
            }
        }
    }
    return r;
}

// ─────────────────────────────────────────────
//  DIJKSTRA
//  Time: O((V + E) log V)  Space: O(V)
// ─────────────────────────────────────────────
PathResult dijkstra(const Graph& g, int src, int dst, OptCriteria crit) {
    int n = g.numCities;
    double dist[20]; int prev[20]; bool visited[20];
    for (int i = 0; i < n; i++) { dist[i] = INF; prev[i] = -1; visited[i] = false; }
    dist[src] = 0;

    MinHeap heap(n * n);
    heap.push(0, src);
    int steps = 0;

    while (!heap.empty()) {
        HeapNode cur = heap.pop();
        int u = cur.cityIdx;
        if (visited[u]) continue;
        visited[u] = true;
        steps++;
        if (u == dst) break;

        for (const Edge& e : g.getEdges(u)) {
            double w = weight(e, crit);
            if (!visited[e.to] && dist[u] + w < dist[e.to]) {
                dist[e.to] = dist[u] + w;
                prev[e.to] = u;
                heap.push(dist[e.to], e.to);
            }
        }
    }

    // Reconstruct path
    std::vector<int> path;
    if (dist[dst] < INF) {
        for (int v = dst; v != -1; v = prev[v]) path.insert(path.begin(), v);
    }
    return buildResult(g, path, steps, "Dijkstra");
}

// ─────────────────────────────────────────────
//  BFS
//  Time: O(V + E)  Space: O(V)
// ─────────────────────────────────────────────
PathResult bfs(const Graph& g, int src, int dst) {
    int n = g.numCities;
    bool visited[20] = {};
    Queue q(n * n * 4);
    q.push({src, {src}});
    visited[src] = true;
    int steps = 0;

    while (!q.empty()) {
        QueueNode cur = q.pop();
        steps++;
        if (cur.cityIdx == dst)
            return buildResult(g, cur.path, steps, "BFS");

        for (const Edge& e : g.getEdges(cur.cityIdx)) {
            if (!visited[e.to]) {
                visited[e.to] = true;
                std::vector<int> newPath = cur.path;
                newPath.push_back(e.to);
                q.push({e.to, newPath});
            }
        }
    }
    return buildResult(g, {}, steps, "BFS");
}

// ─────────────────────────────────────────────
//  DFS (iterative with explicit Stack)
//  Time: O(V + E)  Space: O(V)
// ─────────────────────────────────────────────
PathResult dfs(const Graph& g, int src, int dst) {
    int n = g.numCities;
    Stack stk(n * n * 4);
    stk.push({src, {src}});
    int steps = 0;

    while (!stk.empty()) {
        StackNode cur = stk.pop();
        steps++;
        if (cur.cityIdx == dst)
            return buildResult(g, cur.path, steps, "DFS");

        for (const Edge& e : g.getEdges(cur.cityIdx)) {
            // Check if already in path
            bool inPath = false;
            for (int p : cur.path) if (p == e.to) { inPath = true; break; }
            if (!inPath) {
                std::vector<int> newPath = cur.path;
                newPath.push_back(e.to);
                stk.push({e.to, newPath});
            }
        }
    }
    return buildResult(g, {}, steps, "DFS");
}

// ─────────────────────────────────────────────
//  Haversine heuristic for A*
// ─────────────────────────────────────────────
static double haversine(double lat1, double lng1, double lat2, double lng2) {
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLng = (lng2 - lng1) * M_PI / 180.0;
    double a = sin(dLat/2)*sin(dLat/2)
             + cos(lat1*M_PI/180)*cos(lat2*M_PI/180)*sin(dLng/2)*sin(dLng/2);
    return 6371.0 * 2 * atan2(sqrt(a), sqrt(1-a));
}

// ─────────────────────────────────────────────
//  A* ALGORITHM
//  Time: O((V + E) log V)  Space: O(V)
// ─────────────────────────────────────────────
PathResult astar(const Graph& g, int src, int dst, OptCriteria crit) {
    int n = g.numCities;
    double gCost[20], fCost[20]; int prev[20]; bool closed[20];
    for (int i = 0; i < n; i++) {
        gCost[i] = INF; fCost[i] = INF;
        prev[i]  = -1;  closed[i] = false;
    }
    gCost[src] = 0;
    double h = haversine(g.cities[src].lat, g.cities[src].lng,
                         g.cities[dst].lat, g.cities[dst].lng);
    fCost[src] = h;

    MinHeap heap(n * n);
    heap.push(fCost[src], src);
    int steps = 0;

    while (!heap.empty()) {
        HeapNode cur = heap.pop();
        int u = cur.cityIdx;
        if (closed[u]) continue;
        closed[u] = true;
        steps++;
        if (u == dst) break;

        for (const Edge& e : g.getEdges(u)) {
            if (closed[e.to]) continue;
            double w = weight(e, crit);
            double ng = gCost[u] + w;
            if (ng < gCost[e.to]) {
                gCost[e.to] = ng;
                prev[e.to]  = u;
                double hv   = haversine(g.cities[e.to].lat, g.cities[e.to].lng,
                                         g.cities[dst].lat,  g.cities[dst].lng);
                fCost[e.to] = ng + hv;
                heap.push(fCost[e.to], e.to);
            }
        }
    }

    std::vector<int> path;
    if (gCost[dst] < INF)
        for (int v = dst; v != -1; v = prev[v]) path.insert(path.begin(), v);
    return buildResult(g, path, steps, "A*");
}

// ─────────────────────────────────────────────
//  BELLMAN-FORD
//  Time: O(V * E)  Space: O(V)
// ─────────────────────────────────────────────
PathResult bellmanFord(const Graph& g, int src, int dst, OptCriteria crit) {
    int n = g.numCities;
    double dist[20]; int prev[20];
    for (int i = 0; i < n; i++) { dist[i] = INF; prev[i] = -1; }
    dist[src] = 0;

    // Collect all edges
    struct RawEdge { int u, v; double w; };
    std::vector<RawEdge> edges;
    for (int u = 0; u < n; u++)
        for (const Edge& e : g.getEdges(u))
            edges.push_back({u, e.to, weight(e, crit)});

    int steps = 0;
    // Relax V-1 times
    for (int iter = 0; iter < n - 1; iter++) {
        for (auto& e : edges) {
            steps++;
            if (dist[e.u] < INF && dist[e.u] + e.w < dist[e.v]) {
                dist[e.v] = dist[e.u] + e.w;
                prev[e.v] = e.u;
            }
        }
    }

    // Check negative cycles
    bool negCycle = false;
    for (auto& e : edges)
        if (dist[e.u] < INF && dist[e.u] + e.w < dist[e.v]) { negCycle = true; break; }

    if (negCycle) {
        std::cout << RED << "  [Bellman-Ford] Negative cycle detected!\n" << RESET;
        return buildResult(g, {}, steps, "Bellman-Ford");
    }

    std::vector<int> path;
    if (dist[dst] < INF)
        for (int v = dst; v != -1; v = prev[v]) path.insert(path.begin(), v);
    return buildResult(g, path, steps, "Bellman-Ford");
}

// ─────────────────────────────────────────────
//  FLOYD-WARSHALL  (all-pairs shortest path)
//  Time: O(V^3)  Space: O(V^2)
// ─────────────────────────────────────────────
void floydWarshall(const Graph& g, OptCriteria crit,
                   double dist[][20], int next[][20]) {
    int n = g.numCities;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            dist[i][j] = (i == j) ? 0 : INF;
            next[i][j] = -1;
        }

    for (int u = 0; u < n; u++)
        for (const Edge& e : g.getEdges(u)) {
            double w = weight(e, crit);
            if (w < dist[u][e.to]) {
                dist[u][e.to] = w;
                next[u][e.to] = e.to;
            }
        }

    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (dist[i][k] < INF && dist[k][j] < INF)
                    if (dist[i][k] + dist[k][j] < dist[i][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        next[i][j] = next[i][k];
                    }
}

// ─────────────────────────────────────────────
//  BUDGET-CONSTRAINED DIJKSTRA
//  Time: O((V + E) log V)  Space: O(V)
// ─────────────────────────────────────────────
PathResult budgetDijkstra(const Graph& g, int src, int dst,
                          double maxBudget, OptCriteria crit) {
    int n = g.numCities;
    double dist[20]; int prev[20]; bool visited[20];
    double spent[20]; // track cost separately
    for (int i = 0; i < n; i++) {
        dist[i] = INF; prev[i] = -1;
        visited[i] = false; spent[i] = INF;
    }
    dist[src]  = 0;
    spent[src] = 0;

    MinHeap heap(n * n);
    heap.push(0, src);
    int steps = 0;

    while (!heap.empty()) {
        HeapNode cur = heap.pop();
        int u = cur.cityIdx;
        if (visited[u]) continue;
        visited[u] = true;
        steps++;
        if (u == dst) break;

        for (const Edge& e : g.getEdges(u)) {
            double w   = weight(e, crit);
            double newSpent = spent[u] + e.cost;
            if (newSpent > maxBudget) continue;   // budget constraint
            if (!visited[e.to] && dist[u] + w < dist[e.to]) {
                dist[e.to]  = dist[u] + w;
                prev[e.to]  = u;
                spent[e.to] = newSpent;
                heap.push(dist[e.to], e.to);
            }
        }
    }

    std::vector<int> path;
    if (dist[dst] < INF)
        for (int v = dst; v != -1; v = prev[v]) path.insert(path.begin(), v);
    auto res = buildResult(g, path, steps, "Budget-Dijkstra");
    return res;
}

// ─────────────────────────────────────────────
//  Multi-stop: Nearest Neighbour TSP heuristic
// ─────────────────────────────────────────────
std::vector<int> multiStopPlan(const Graph& g, int start,
                               std::vector<int>& stops, OptCriteria crit) {
    std::vector<int> order;
    order.push_back(start);
    std::vector<bool> visited(stops.size(), false);

    int cur = start;
    for (int iter = 0; iter < (int)stops.size(); iter++) {
        double best = INF;
        int    bestIdx = -1;
        PathResult bestPath;
        for (int i = 0; i < (int)stops.size(); i++) {
            if (visited[i] || stops[i] == cur) continue;
            PathResult r = dijkstra(g, cur, stops[i], crit);
            if (r.found) {
                double w = (crit == BY_TIME) ? r.totalTime :
                           (crit == BY_COST) ? r.totalCost : r.totalDist;
                if (w < best) { best = w; bestIdx = i; bestPath = r; }
            }
        }
        if (bestIdx == -1) break;
        visited[bestIdx] = true;
        // append sub-path (skip first element to avoid duplicates)
        for (int i = 1; i < (int)bestPath.path.size(); i++)
            order.push_back(bestPath.path[i]);
        cur = stops[bestIdx];
    }
    return order;
}

// ─────────────────────────────────────────────
//  Print Path Result
// ─────────────────────────────────────────────
void printPath(const Graph& g, const PathResult& r) {
    using namespace std;
    if (!r.found) {
        cout << RED << "  No path found!\n" << RESET;
        return;
    }
    cout << "\n  " << CYAN << "Route: " << RESET;
    for (int i = 0; i < (int)r.path.size(); i++) {
        cout << GREEN << g.cities[r.path[i]].name << RESET;
        if (i + 1 < (int)r.path.size()) cout << YELLOW << " → " << RESET;
    }
    cout << "\n\n";
    cout << "  ┌────────────────────────────────┐\n";
    cout << "  │  " << CYAN << "Total Distance : " << RESET
         << setw(8) << fixed << setprecision(0) << r.totalDist << " km        │\n";
    cout << "  │  " << CYAN << "Total Time     : " << RESET
         << setw(8) << fixed << setprecision(0) << r.totalTime / 60.0 << " hrs       │\n";
    cout << "  │  " << CYAN << "Total Cost     : " << RESET
         << "₹" << setw(7) << fixed << setprecision(0) << r.totalCost << "        │\n";
    cout << "  │  " << YELLOW << "Steps Explored : " << RESET
         << setw(8) << r.stepsExplored << "           │\n";
    cout << "  └────────────────────────────────┘\n";
}

// ─────────────────────────────────────────────
//  Comparison Table
// ─────────────────────────────────────────────
void printComparisonTable(const Graph& g, int src, int dst,
                          const std::vector<PathResult>& results) {
    using namespace std;
    cout << "\n  " << CYAN << "Algorithm Comparison: "
         << g.cities[src].name << " → " << g.cities[dst].name << "\n" << RESET;
    cout << "  ╔══════════════╦══════════╦══════════╦══════════╦════════╦═══════════╗\n";
    cout << "  ║ " << YELLOW << left << setw(12) << "Algorithm" << RESET
         << " ║ " << setw(8) << "Dist(km)" 
         << " ║ " << setw(8) << "Time(hr)"
         << " ║ " << setw(8) << "Cost(₹)"
         << " ║ " << setw(6) << "Steps"
         << " ║ " << setw(9) << "Path Len"
         << " ║\n";
    cout << "  ╠══════════════╬══════════╬══════════╬══════════╬════════╬═══════════╣\n";

    for (auto& r : results) {
        string found = r.found ? "" : "(No path)";
        cout << "  ║ " << GREEN << left << setw(12) << r.algoName << RESET
             << " ║ " << setw(8) << (r.found ? to_string((int)r.totalDist) : "-")
             << " ║ " << setw(8) << (r.found ? to_string((int)(r.totalTime/60)) + "h" : "-")
             << " ║ " << setw(8) << (r.found ? to_string((int)r.totalCost) : "-")
             << " ║ " << setw(6) << r.stepsExplored
             << " ║ " << setw(9) << (r.found ? (int)r.path.size() : 0)
             << " ║\n";
    }
    cout << "  ╚══════════════╩══════════╩══════════╩══════════╩════════╩═══════════╝\n";
}

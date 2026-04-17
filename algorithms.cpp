#include "algorithms.h"
#include "ui.h"
#include <iostream>
#include <iomanip>
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <climits>
#include <string>

static const double INF = 1e18;

// ─────────────────────────────────────────────
//  Helper: extract edge weight by criteria
// ─────────────────────────────────────────────
static double weight(const Edge& e, OptCriteria c) {
    if (c == BY_TIME) return e.time;
    if (c == BY_COST) return e.cost;
    return e.distance;
}

// ═════════════════════════════════════════════
//  CUSTOM DATA STRUCTURES
// ═════════════════════════════════════════════

// ─────────────────────────────────────────────
//  MinHeap — binary min-heap
// ─────────────────────────────────────────────
MinHeap::MinHeap(int cap) : sz(0), capacity(cap) {
    data = new HeapNode[cap];
}
MinHeap::~MinHeap() { delete[] data; }

void MinHeap::heapifyUp(int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (data[parent].key > data[i].key) {
            HeapNode tmp   = data[parent];
            data[parent]   = data[i];
            data[i]        = tmp;
            i = parent;
        } else break;
    }
}

void MinHeap::heapifyDown(int i) {
    while (true) {
        int smallest = i;
        int left     = 2 * i + 1;
        int right    = 2 * i + 2;
        if (left  < sz && data[left].key  < data[smallest].key) smallest = left;
        if (right < sz && data[right].key < data[smallest].key) smallest = right;
        if (smallest == i) break;
        HeapNode tmp      = data[i];
        data[i]           = data[smallest];
        data[smallest]    = tmp;
        i = smallest;
    }
}

void MinHeap::push(double key, int city) {
    data[sz++] = {key, city};
    heapifyUp(sz - 1);
}

HeapNode MinHeap::pop() {
    HeapNode top = data[0];
    data[0] = data[--sz];
    heapifyDown(0);
    return top;
}

// ─────────────────────────────────────────────
//  Stack — for iterative DFS
// ─────────────────────────────────────────────
Stack::Stack(int cap) : top(-1), capacity(cap) {
    data = new StackNode[cap];
}
Stack::~Stack() { delete[] data; }

void Stack::push(StackNode n) { data[++top] = n; }
StackNode Stack::pop()        { return data[top--]; }

// ─────────────────────────────────────────────
//  Queue — circular buffer for BFS
// ─────────────────────────────────────────────
Queue::Queue(int cap) : head(0), tail(0), cap(cap) {
    data = new QueueNode[cap];
}
Queue::~Queue() { delete[] data; }

void Queue::push(QueueNode n) { data[tail % cap] = n; tail++; }
QueueNode Queue::pop()        { return data[head++ % cap]; }

// ═════════════════════════════════════════════
//  HELPER: Build PathResult from path + actual edges used
//
//  Each algorithm now tracks the exact Edge it chose for each hop.
//  This avoids the ambiguity when parallel edges (road + rail + air)
//  exist between the same pair of cities.
// ═════════════════════════════════════════════
static PathResult buildResultFromEdges(const std::vector<int>& path,
                                        const std::vector<Edge>& usedEdges,
                                        int steps, const std::string& name) {
    PathResult r;
    r.path          = path;
    r.found         = !path.empty();
    r.stepsExplored = steps;
    r.algoName      = name;
    r.totalDist = r.totalTime = r.totalCost = 0.0;

    for (const Edge& e : usedEdges) {
        r.totalDist += e.distance;
        r.totalTime += e.time;
        r.totalCost += e.cost;
    }
    return r;
}

// Fallback: when we only have a path vector (BFS/DFS), pick the edge
// whose weight-by-criteria is smallest among parallel edges.
static PathResult buildResult(const Graph& g, const std::vector<int>& path,
                               int steps, const std::string& name,
                               OptCriteria crit = BY_DISTANCE) {
    PathResult r;
    r.path          = path;
    r.found         = !path.empty();
    r.stepsExplored = steps;
    r.algoName      = name;
    r.totalDist = r.totalTime = r.totalCost = 0.0;

    for (int i = 0; i + 1 < (int)path.size(); i++) {
        int u = path[i];
        int v = path[i + 1];

        std::vector<Edge> edges = g.getEdges(u);
        bool   edgeFound = false;
        double bestW     = INF;
        double bestDist  = 0, bestTime = 0, bestCost = 0;

        for (const Edge& e : edges) {
            if (e.to != v) continue;
            double w = weight(e, crit);
            if (w < bestW) {
                bestW    = w;
                bestDist = e.distance;
                bestTime = e.time;
                bestCost = e.cost;
            }
            edgeFound = true;
        }
        if (!edgeFound) {
            r.found     = false;
            r.totalDist = r.totalTime = r.totalCost = 0.0;
            return r;
        }
        r.totalDist += bestDist;
        r.totalTime += bestTime;
        r.totalCost += bestCost;
    }
    return r;
}

// ═════════════════════════════════════════════
//  DIJKSTRA  —  O((V+E) log V)
// ═════════════════════════════════════════════
PathResult dijkstra(const Graph& g, int src, int dst, OptCriteria crit) {
    int n = g.numCities;
    std::vector<double> dist(n, INF);
    std::vector<int>    prev(n, -1);
    std::vector<bool>   visited(n, false);
    // Store the actual edge used to reach each node
    std::vector<Edge>   prevEdge(n, {-1, 0, 0, 0, ROAD});

    dist[src] = 0.0;
    MinHeap heap(n * n + 10);
    heap.push(0.0, src);
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
                dist[e.to]     = dist[u] + w;
                prev[e.to]     = u;
                prevEdge[e.to] = e;
                heap.push(dist[e.to], e.to);
            }
        }
    }

    // Reconstruct path and collect used edges
    std::vector<int>  path;
    std::vector<Edge> usedEdges;
    if (dist[dst] < INF) {
        for (int v = dst; v != -1; v = prev[v])
            path.insert(path.begin(), v);
        for (int i = 1; i < (int)path.size(); i++)
            usedEdges.push_back(prevEdge[path[i]]);
    }
    return buildResultFromEdges(path, usedEdges, steps, "Dijkstra");
}

// ═════════════════════════════════════════════
//  DIJKSTRA FILTERED  —  only uses a given travel mode
//  modeFilter = -1 means any mode
// ═════════════════════════════════════════════
PathResult dijkstraFiltered(const Graph& g, int src, int dst,
                            OptCriteria crit, int modeFilter) {
    if (modeFilter == -1)
        return dijkstra(g, src, dst, crit);

    // Validate: source and destination must have airport for AIR filter
    if (modeFilter == AIR) {
        if (!g.cities[src].hasAirport || !g.cities[dst].hasAirport) {
            PathResult r;
            r.found         = false;
            r.stepsExplored = 0;
            r.totalDist = r.totalTime = r.totalCost = 0.0;
            r.algoName = "Air-Dijkstra";
            return r;
        }
    }

    int n = g.numCities;
    std::vector<double> dist(n, INF);
    std::vector<int>    prev(n, -1);
    std::vector<bool>   visited(n, false);
    std::vector<Edge>   prevEdge(n, {-1, 0, 0, 0, ROAD});

    dist[src] = 0.0;
    MinHeap heap(n * n + 10);
    heap.push(0.0, src);
    int steps = 0;

    while (!heap.empty()) {
        HeapNode cur = heap.pop();
        int u = cur.cityIdx;
        if (visited[u]) continue;
        visited[u] = true;
        steps++;
        if (u == dst) break;

        for (const Edge& e : g.getEdges(u)) {
            // Only traverse edges of the chosen mode
            if ((int)e.mode != modeFilter) continue;
            double w = weight(e, crit);
            if (!visited[e.to] && dist[u] + w < dist[e.to]) {
                dist[e.to]     = dist[u] + w;
                prev[e.to]     = u;
                prevEdge[e.to] = e;
                heap.push(dist[e.to], e.to);
            }
        }
    }

    std::vector<int>  path;
    std::vector<Edge> usedEdges;
    if (dist[dst] < INF) {
        for (int v = dst; v != -1; v = prev[v])
            path.insert(path.begin(), v);
        for (int i = 1; i < (int)path.size(); i++)
            usedEdges.push_back(prevEdge[path[i]]);
    }

    std::string name = (modeFilter == ROAD) ? "Road-Dijkstra" :
                       (modeFilter == RAIL) ? "Rail-Dijkstra" : "Air-Dijkstra";
    return buildResultFromEdges(path, usedEdges, steps, name);
}

// ═════════════════════════════════════════════
//  BFS  —  O(V+E), minimises number of hops
// ═════════════════════════════════════════════
PathResult bfs(const Graph& g, int src, int dst) {
    int n = g.numCities;
    std::vector<bool> visited(n, false);
    // Queue large enough for all paths (n cities × n path length worst case)
    Queue q(n * n * 4 + 100);
    q.push({src, {src}});
    visited[src] = true;
    int steps = 0;

    while (!q.empty()) {
        QueueNode cur = q.pop();
        steps++;
        if (cur.cityIdx == dst)
            return buildResult(g, cur.path, steps, "BFS", BY_DISTANCE);

        for (const Edge& e : g.getEdges(cur.cityIdx)) {
            if (!visited[e.to]) {
                visited[e.to] = true;
                std::vector<int> newPath = cur.path;
                newPath.push_back(e.to);
                q.push({e.to, newPath});
            }
        }
    }
    return buildResult(g, {}, steps, "BFS", BY_DISTANCE);
}

// ═════════════════════════════════════════════
//  DFS  —  iterative with explicit stack
//  Finds a path (not necessarily optimal)
// ═════════════════════════════════════════════
PathResult dfs(const Graph& g, int src, int dst) {
    int n = g.numCities;
    Stack stk(n * n * 4 + 100);
    stk.push({src, {src}});
    int steps = 0;

    while (!stk.empty()) {
        StackNode cur = stk.pop();
        steps++;
        if (cur.cityIdx == dst)
            return buildResult(g, cur.path, steps, "DFS", BY_DISTANCE);

        for (const Edge& e : g.getEdges(cur.cityIdx)) {
            // Avoid revisiting nodes already in the current path (prevents cycles)
            bool inPath = false;
            for (int p : cur.path)
                if (p == e.to) { inPath = true; break; }
            if (!inPath) {
                std::vector<int> newPath = cur.path;
                newPath.push_back(e.to);
                stk.push({e.to, newPath});
            }
        }
    }
    return buildResult(g, {}, steps, "DFS", BY_DISTANCE);
}

// ─────────────────────────────────────────────
//  Haversine formula: great-circle distance (km)
//  Used as the admissible heuristic in A*
// ─────────────────────────────────────────────
static double haversine(double lat1, double lng1, double lat2, double lng2) {
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLng = (lng2 - lng1) * M_PI / 180.0;
    double a = sin(dLat / 2) * sin(dLat / 2)
             + cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0)
             * sin(dLng / 2) * sin(dLng / 2);
    return 6371.0 * 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
}

// ═════════════════════════════════════════════
//  A*  —  O((V+E) log V), heuristic-guided
//  Uses Haversine distance as heuristic (admissible for BY_DISTANCE)
// ═════════════════════════════════════════════
PathResult astar(const Graph& g, int src, int dst, OptCriteria crit) {
    int n = g.numCities;
    std::vector<double> gCost(n, INF);
    std::vector<double> fCost(n, INF);
    std::vector<int>    prev(n, -1);
    std::vector<bool>   visited(n, false);
    std::vector<Edge>   prevEdge(n, {-1, 0, 0, 0, ROAD});

    gCost[src] = 0.0;
    fCost[src] = haversine(g.cities[src].lat, g.cities[src].lng,
                           g.cities[dst].lat,  g.cities[dst].lng);

    MinHeap heap(n * n + 10);
    heap.push(fCost[src], src);
    int steps = 0;

    while (!heap.empty()) {
        HeapNode cur = heap.pop();
        int u = cur.cityIdx;
        if (visited[u]) continue;
        visited[u] = true;
        steps++;
        if (u == dst) break;

        for (const Edge& e : g.getEdges(u)) {
            if (visited[e.to]) continue;
            double w  = weight(e, crit);
            double ng = gCost[u] + w;
            if (ng < gCost[e.to]) {
                gCost[e.to]    = ng;
                prev[e.to]     = u;
                prevEdge[e.to] = e;
                double hv = (crit == BY_DISTANCE)
                    ? haversine(g.cities[e.to].lat, g.cities[e.to].lng,
                                g.cities[dst].lat,  g.cities[dst].lng)
                    : 0.0;
                fCost[e.to] = ng + hv;
                heap.push(fCost[e.to], e.to);
            }
        }
    }

    std::vector<int>  path;
    std::vector<Edge> usedEdges;
    if (gCost[dst] < INF) {
        for (int v = dst; v != -1; v = prev[v])
            path.insert(path.begin(), v);
        for (int i = 1; i < (int)path.size(); i++)
            usedEdges.push_back(prevEdge[path[i]]);
    }
    return buildResultFromEdges(path, usedEdges, steps, "A*");
}

// ═════════════════════════════════════════════
//  BELLMAN-FORD  —  O(V × E)
//  Handles negative weights; detects negative cycles
// ═════════════════════════════════════════════
PathResult bellmanFord(const Graph& g, int src, int dst, OptCriteria crit) {
    int n = g.numCities;
    std::vector<double> dist(n, INF);
    std::vector<int>    prev(n, -1);
    dist[src] = 0.0;

    // Collect all directed edges (with original Edge reference for prevEdge tracking)
    struct RawEdge { int u, v; double w; Edge orig; };
    std::vector<RawEdge> edges;
    for (int u = 0; u < n; u++)
        for (const Edge& e : g.getEdges(u))
            edges.push_back({u, e.to, weight(e, crit), e});

    std::vector<Edge> prevEdge(n, {-1, 0, 0, 0, ROAD});
    int steps = 0;

    // Relax V-1 times
    for (int iter = 0; iter < n - 1; iter++) {
        bool updated = false;
        for (auto& e : edges) {
            steps++;
            if (dist[e.u] < INF && dist[e.u] + e.w < dist[e.v]) {
                dist[e.v]     = dist[e.u] + e.w;
                prev[e.v]     = e.u;
                prevEdge[e.v] = e.orig;
                updated       = true;
            }
        }
        if (!updated) break;  // Early exit if no updates
    }

    // Check for negative cycles
    for (auto& e : edges) {
        if (dist[e.u] < INF && dist[e.u] + e.w < dist[e.v]) {
            std::cout << RED << "  [Bellman-Ford] Negative cycle detected!\n" << RESET;
            return buildResultFromEdges({}, {}, steps, "Bellman-Ford");
        }
    }

    std::vector<int>  path;
    std::vector<Edge> usedEdges;
    if (dist[dst] < INF) {
        for (int v = dst; v != -1; v = prev[v])
            path.insert(path.begin(), v);
        for (int i = 1; i < (int)path.size(); i++)
            usedEdges.push_back(prevEdge[path[i]]);
    }
    return buildResultFromEdges(path, usedEdges, steps, "Bellman-Ford");
}

// ═════════════════════════════════════════════
//  FLOYD-WARSHALL  —  O(V³), all-pairs shortest paths
// ═════════════════════════════════════════════
void floydWarshall(const Graph& g, OptCriteria crit,
                   double dist[][Graph::MAXN], int next[][Graph::MAXN]) {
    int n = g.numCities;

    // Initialize
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            dist[i][j] = (i == j) ? 0.0 : INF;
            next[i][j] = -1;
        }

    // Seed with direct edges
    for (int u = 0; u < n; u++) {
        for (const Edge& e : g.getEdges(u)) {
            double w = weight(e, crit);
            if (w < dist[u][e.to]) {
                dist[u][e.to] = w;
                next[u][e.to] = e.to;
            }
        }
    }

    // Relax through all intermediate vertices
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (dist[i][k] < INF && dist[k][j] < INF)
                    if (dist[i][k] + dist[k][j] < dist[i][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        next[i][j] = next[i][k];
                    }
}

// ═════════════════════════════════════════════
//  BUDGET-CONSTRAINED DIJKSTRA
//  Finds best path (by crit) while keeping total cost ≤ maxBudget
// ═════════════════════════════════════════════
PathResult budgetDijkstra(const Graph& g, int src, int dst,
                          double maxBudget, OptCriteria crit) {
    int n = g.numCities;
    std::vector<double> dist(n, INF);
    std::vector<int>    prev(n, -1);
    std::vector<bool>   visited(n, false);
    std::vector<double> spent(n, INF);      // cumulative INR cost on best path
    std::vector<Edge>   prevEdge(n, {-1, 0, 0, 0, ROAD});

    dist[src]  = 0.0;
    spent[src] = 0.0;

    MinHeap heap(n * n + 10);
    heap.push(0.0, src);
    int steps = 0;

    while (!heap.empty()) {
        HeapNode cur = heap.pop();
        int u = cur.cityIdx;
        if (visited[u]) continue;
        visited[u] = true;
        steps++;
        if (u == dst) break;

        for (const Edge& e : g.getEdges(u)) {
            double newSpent = spent[u] + e.cost;
            if (newSpent > maxBudget) continue;        // budget exceeded — skip

            double w = weight(e, crit);
            if (!visited[e.to] && dist[u] + w < dist[e.to]) {
                dist[e.to]     = dist[u] + w;
                prev[e.to]     = u;
                spent[e.to]    = newSpent;
                prevEdge[e.to] = e;
                heap.push(dist[e.to], e.to);
            }
        }
    }

    std::vector<int>  path;
    std::vector<Edge> usedEdges;
    if (dist[dst] < INF) {
        for (int v = dst; v != -1; v = prev[v])
            path.insert(path.begin(), v);
        for (int i = 1; i < (int)path.size(); i++)
            usedEdges.push_back(prevEdge[path[i]]);
    }
    return buildResultFromEdges(path, usedEdges, steps, "Budget-Dijkstra");
}

// ═════════════════════════════════════════════
//  MULTI-STOP PLANNER  —  Nearest-Neighbour TSP heuristic
// ═════════════════════════════════════════════
std::vector<int> multiStopPlan(const Graph& g, int start,
                               std::vector<int>& stops, OptCriteria crit) {
    std::vector<int>  order;
    order.push_back(start);
    std::vector<bool> visited(stops.size(), false);

    int cur = start;
    for (int iter = 0; iter < (int)stops.size(); iter++) {
        double     best     = INF;
        int        bestIdx  = -1;
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
        // Append sub-path (skip first city to avoid duplicating the junction)
        for (int i = 1; i < (int)bestPath.path.size(); i++)
            order.push_back(bestPath.path[i]);
        cur = stops[bestIdx];
    }
    return order;
}

// ═════════════════════════════════════════════
//  PRINT PATH RESULT
// ═════════════════════════════════════════════
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
    cout << "  │  " << CYAN  << "Total Distance : " << RESET
         << setw(8) << fixed << setprecision(0) << r.totalDist << " km       │\n";
    cout << "  │  " << CYAN  << "Total Time     : " << RESET
         << setw(8) << fixed << setprecision(1) << r.totalTime / 60.0 << " hrs      │\n";
    cout << "  │  " << CYAN  << "Total Cost     : " << RESET
         << "₹" << setw(7) << fixed << setprecision(0) << r.totalCost << "        │\n";
    cout << "  │  " << YELLOW << "Steps Explored : " << RESET
         << setw(8) << r.stepsExplored << "           │\n";
    cout << "  └────────────────────────────────┘\n";
}

// ═════════════════════════════════════════════
//  COMPARISON TABLE
// ═════════════════════════════════════════════
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

    for (const auto& r : results) {
        cout << "  ║ " << GREEN << left << setw(12) << r.algoName << RESET
             << " ║ " << setw(8) << (r.found ? to_string((int)r.totalDist) : "-")
             << " ║ " << setw(8) << (r.found ? to_string((int)(r.totalTime / 60)) + "h" : "-")
             << " ║ " << setw(8) << (r.found ? to_string((int)r.totalCost) : "-")
             << " ║ " << setw(6) << r.stepsExplored
             << " ║ " << setw(9) << (r.found ? (int)r.path.size() : 0)
             << " ║\n";
    }
    cout << "  ╚══════════════╩══════════╩══════════╩══════════╩════════╩═══════════╝\n";
}

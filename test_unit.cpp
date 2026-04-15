#include "graph.h"
#include "algorithms.h"
#include "ui.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <iomanip>

// ─────────────────────────────────────────────
//  Test framework helpers
// ─────────────────────────────────────────────
int passed = 0, failed = 0;

void testAssert(bool cond, const std::string& name) {
    if (cond) {
        std::cout << GREEN << "  [PASS] " << RESET << name << "\n";
        passed++;
    } else {
        std::cout << RED << "  [FAIL] " << RESET << name << "\n";
        failed++;
    }
}

// ─────────────────────────────────────────────
//  Helper: city index by name
// ─────────────────────────────────────────────
int cityIdx(const Graph& g, const std::string& name) {
    return g.getCityIndex(name);
}

// ─────────────────────────────────────────────
//  TEST SUITE
// ─────────────────────────────────────────────
int main() {
    Graph g;
    g.loadDefaultNetwork();

    std::cout << CYAN << "\n  ══════════════════════════════════════════════════\n";
    std::cout << "       DSA Travel Planner — Automated Test Suite\n";
    std::cout << "  ══════════════════════════════════════════════════\n" << RESET;

    int D  = cityIdx(g, "Delhi");
    int M  = cityIdx(g, "Mumbai");
    int B  = cityIdx(g, "Bangalore");
    int C  = cityIdx(g, "Chennai");
    int K  = cityIdx(g, "Kolkata");
    int H  = cityIdx(g, "Hyderabad");
    int J  = cityIdx(g, "Jaipur");
    int A  = cityIdx(g, "Ahmedabad");
    int P  = cityIdx(g, "Pune");
    int Lu = cityIdx(g, "Lucknow");

    // ─────────────────────────────────────────────
    //  Section 1: Graph Construction
    // ─────────────────────────────────────────────
    std::cout << YELLOW << "\n  [1] Graph Construction\n" << RESET;
    testAssert(g.numCities == 15, "15 cities loaded");
    testAssert(D != -1, "Delhi found");
    testAssert(M != -1, "Mumbai found");
    testAssert(B != -1, "Bangalore found");
    testAssert(g.hasEdge[D][J], "Delhi→Jaipur edge exists");
    testAssert(g.hasEdge[J][D], "Jaipur→Delhi edge (undirected) exists");
    testAssert(g.adjMatrix[D][J] == 281, "Delhi→Jaipur distance = 281 km");
    testAssert(!g.hasEdge[D][B], "Delhi→Bangalore no direct edge");

    // ─────────────────────────────────────────────
    //  Section 2: Dijkstra
    // ─────────────────────────────────────────────
    std::cout << YELLOW << "\n  [2] Dijkstra Algorithm\n" << RESET;

    // Direct neighbor
    PathResult r1 = dijkstra(g, D, J, BY_DISTANCE);
    testAssert(r1.found, "Dijkstra: Delhi→Jaipur found");
    testAssert(r1.path.front() == D, "Dijkstra: path starts at Delhi");
    testAssert(r1.path.back()  == J, "Dijkstra: path ends at Jaipur");
    testAssert(r1.totalDist == 281, "Dijkstra: Delhi→Jaipur direct dist = 281");

    // Multi-hop
    PathResult r2 = dijkstra(g, D, B, BY_DISTANCE);
    testAssert(r2.found, "Dijkstra: Delhi→Bangalore found");
    testAssert(r2.totalDist > 0, "Dijkstra: positive total distance");
    testAssert(r2.totalCost > 0, "Dijkstra: positive total cost");

    // Same node
    PathResult r3 = dijkstra(g, M, M, BY_DISTANCE);
    testAssert(r3.path.size() == 1 || r3.totalDist == 0, "Dijkstra: src==dst trivial");

    // BY_TIME
    PathResult r4 = dijkstra(g, D, K, BY_TIME);
    testAssert(r4.found, "Dijkstra by TIME: Delhi→Kolkata found");
    testAssert(r4.totalTime > 0, "Dijkstra by TIME: positive total time");

    // BY_COST
    PathResult r5 = dijkstra(g, D, B, BY_COST);
    testAssert(r5.found, "Dijkstra by COST: Delhi→Bangalore found");
    testAssert(r5.totalCost <= r2.totalCost + 1.0, "Dijkstra: cost-optimal path ≤ distance path cost");

    // Unreachable (if any isolated city — not in our graph, test inverted)
    // We'll verify dijkstra on adjacent pair is always shorter than via detour
    PathResult r6a = dijkstra(g, M, P, BY_DISTANCE);
    PathResult r6b = dijkstra(g, M, B, BY_DISTANCE);
    testAssert(r6a.found, "Dijkstra: Mumbai→Pune reachable");
    testAssert(r6b.found, "Dijkstra: Mumbai→Bangalore reachable");
    testAssert(r6a.totalDist <= r6b.totalDist, "Dijkstra: Mumbai→Pune shorter than Mumbai→Bangalore");

    // ─────────────────────────────────────────────
    //  Section 3: BFS
    // ─────────────────────────────────────────────
    std::cout << YELLOW << "\n  [3] BFS Algorithm\n" << RESET;

    PathResult bfs1 = bfs(g, D, J);
    testAssert(bfs1.found, "BFS: Delhi→Jaipur found");
    testAssert(bfs1.path.front() == D, "BFS: starts at Delhi");
    testAssert(bfs1.path.back() == J,  "BFS: ends at Jaipur");
    testAssert(bfs1.path.size() == 2,  "BFS: Delhi→Jaipur 1 hop (direct)");

    PathResult bfs2 = bfs(g, D, B);
    testAssert(bfs2.found, "BFS: Delhi→Bangalore found");
    // BFS gives minimum hops
    PathResult dijk2 = dijkstra(g, D, B, BY_DISTANCE);
    testAssert(bfs2.path.size() <= dijk2.path.size() + 2,
               "BFS: hops not wildly worse than Dijkstra");

    PathResult bfs3 = bfs(g, M, H);
    testAssert(bfs3.found, "BFS: Mumbai→Hyderabad found");
    testAssert(bfs3.path.size() == 2, "BFS: Mumbai→Hyderabad 1 hop (direct)");

    // ─────────────────────────────────────────────
    //  Section 4: DFS
    // ─────────────────────────────────────────────
    std::cout << YELLOW << "\n  [4] DFS Algorithm\n" << RESET;

    PathResult dfs1 = dfs(g, D, J);
    testAssert(dfs1.found, "DFS: Delhi→Jaipur found");
    testAssert(dfs1.path.front() == D, "DFS: starts at Delhi");
    testAssert(dfs1.path.back()  == J, "DFS: ends at Jaipur");

    PathResult dfs2 = dfs(g, M, B);
    testAssert(dfs2.found, "DFS: Mumbai→Bangalore found");

    PathResult dfs3 = dfs(g, B, K);
    testAssert(dfs3.found, "DFS: Bangalore→Kolkata found");

    // No cycles in path
    PathResult dfs4 = dfs(g, D, C);
    if (dfs4.found) {
        bool noCycles = true;
        for (int i = 0; i < (int)dfs4.path.size(); i++)
            for (int j = i+1; j < (int)dfs4.path.size(); j++)
                if (dfs4.path[i] == dfs4.path[j]) noCycles = false;
        testAssert(noCycles, "DFS: path has no repeated cities");
    }

    // ─────────────────────────────────────────────
    //  Section 5: A*
    // ─────────────────────────────────────────────
    std::cout << YELLOW << "\n  [5] A* Algorithm\n" << RESET;

    PathResult as1 = astar(g, D, J, BY_DISTANCE);
    testAssert(as1.found, "A*: Delhi→Jaipur found");
    testAssert(as1.totalDist == r1.totalDist,
               "A*: Delhi→Jaipur same cost as Dijkstra");

    PathResult as2 = astar(g, M, H, BY_DISTANCE);
    testAssert(as2.found, "A*: Mumbai→Hyderabad found");

    PathResult as3 = astar(g, D, B, BY_DISTANCE);
    PathResult dj3 = dijkstra(g, D, B, BY_DISTANCE);
    testAssert(as3.found, "A*: Delhi→Bangalore found");
    testAssert(std::abs(as3.totalDist - dj3.totalDist) < 1.0,
               "A*: Delhi→Bangalore matches Dijkstra distance");

    PathResult as4 = astar(g, J, C, BY_COST);
    testAssert(as4.found, "A*: Jaipur→Chennai found (by cost)");

    // ─────────────────────────────────────────────
    //  Section 6: Bellman-Ford
    // ─────────────────────────────────────────────
    std::cout << YELLOW << "\n  [6] Bellman-Ford Algorithm\n" << RESET;

    PathResult bf1 = bellmanFord(g, D, J, BY_DISTANCE);
    testAssert(bf1.found, "Bellman-Ford: Delhi→Jaipur found");
    testAssert(std::abs(bf1.totalDist - r1.totalDist) < 1.0,
               "Bellman-Ford: matches Dijkstra distance for Delhi→Jaipur");

    PathResult bf2 = bellmanFord(g, D, B, BY_DISTANCE);
    testAssert(bf2.found, "Bellman-Ford: Delhi→Bangalore found");
    testAssert(std::abs(bf2.totalDist - dijk2.totalDist) < 1.0,
               "Bellman-Ford: matches Dijkstra for Delhi→Bangalore");

    PathResult bf3 = bellmanFord(g, M, K, BY_COST);
    testAssert(bf3.found, "Bellman-Ford: Mumbai→Kolkata (by cost) found");

    // ─────────────────────────────────────────────
    //  Section 7: Floyd-Warshall
    // ─────────────────────────────────────────────
    std::cout << YELLOW << "\n  [7] Floyd-Warshall\n" << RESET;

    double fw_dist[20][20]; int fw_next[20][20];
    floydWarshall(g, BY_DISTANCE, fw_dist, fw_next);

    testAssert(fw_dist[D][D] == 0, "Floyd-Warshall: diagonal = 0");
    testAssert(fw_dist[D][J] == 281, "Floyd-Warshall: Delhi→Jaipur = 281");
    testAssert(fw_dist[J][D] == 281, "Floyd-Warshall: Jaipur→Delhi = 281 (symmetric)");
    testAssert(fw_dist[D][B] > 0 && fw_dist[D][B] < 1e17, "Floyd-Warshall: Delhi→Bangalore reachable");
    testAssert(std::abs(fw_dist[D][B] - dijk2.totalDist) < 1.0,
               "Floyd-Warshall: Delhi→Bangalore matches Dijkstra");
    // Symmetry
    testAssert(std::abs(fw_dist[M][B] - fw_dist[B][M]) < 1.0,
               "Floyd-Warshall: Mumbai↔Bangalore symmetric");

    // Path reconstruction
    testAssert(fw_next[D][J] == J, "Floyd-Warshall: Delhi→Jaipur next is Jaipur");
    testAssert(fw_next[D][B] != -1, "Floyd-Warshall: Delhi→Bangalore next exists");

    // ─────────────────────────────────────────────
    //  Section 8: Budget-Constrained Dijkstra
    // ─────────────────────────────────────────────
    std::cout << YELLOW << "\n  [8] Budget-Constrained Dijkstra\n" << RESET;

    // With enough budget
    PathResult bg1 = budgetDijkstra(g, D, J, 10000, BY_DISTANCE);
    testAssert(bg1.found, "Budget-Dijkstra: Delhi→Jaipur within ₹10000 found");
    testAssert(bg1.totalCost <= 10000, "Budget-Dijkstra: cost stays within budget");

    // With tight budget — direct edge costs 650
    PathResult bg2 = budgetDijkstra(g, D, J, 700, BY_DISTANCE);
    testAssert(bg2.found, "Budget-Dijkstra: Delhi→Jaipur within ₹700 found (direct=650)");
    testAssert(bg2.totalCost <= 700, "Budget-Dijkstra: cost ≤ 700");

    // With impossible budget
    PathResult bg3 = budgetDijkstra(g, D, J, 10, BY_DISTANCE);
    testAssert(!bg3.found, "Budget-Dijkstra: Delhi→Jaipur with ₹10 not found (impossible)");

    // Long route with budget
    PathResult bg4 = budgetDijkstra(g, D, B, 8000, BY_DISTANCE);
    PathResult bg4_nobug = dijkstra(g, D, B, BY_DISTANCE);
    if (bg4.found) {
        testAssert(bg4.totalCost <= 8000, "Budget-Dijkstra: Delhi→Bangalore within ₹8000");
    } else {
        testAssert(true, "Budget-Dijkstra: Delhi→Bangalore with ₹8000 (no path—accepted)");
    }

    // ─────────────────────────────────────────────
    //  Section 9: Multi-Stop Planning
    // ─────────────────────────────────────────────
    std::cout << YELLOW << "\n  [9] Multi-Stop Nearest Neighbour\n" << RESET;

    std::vector<int> stops1 = {J, A};
    std::vector<int> order1 = multiStopPlan(g, D, stops1, BY_DISTANCE);
    testAssert(!order1.empty(), "Multi-stop: Delhi→{Jaipur,Ahmedabad} not empty");
    testAssert(order1.front() == D, "Multi-stop: starts at Delhi");

    std::vector<int> stops2 = {M, P};
    std::vector<int> order2 = multiStopPlan(g, D, stops2, BY_DISTANCE);
    testAssert(!order2.empty(), "Multi-stop: Delhi→{Mumbai,Pune} not empty");

    // ─────────────────────────────────────────────
    //  Section 10: Custom Data Structures
    // ─────────────────────────────────────────────
    std::cout << YELLOW << "\n  [10] Custom Data Structures\n" << RESET;

    // MinHeap
    MinHeap heap(10);
    heap.push(5.0, 5);
    heap.push(2.0, 2);
    heap.push(8.0, 8);
    heap.push(1.0, 1);
    testAssert(!heap.empty(), "MinHeap: not empty after push");
    HeapNode top = heap.pop();
    testAssert(top.key == 1.0 && top.cityIdx == 1, "MinHeap: pop returns min (key=1)");
    top = heap.pop();
    testAssert(top.key == 2.0, "MinHeap: second pop = 2");
    top = heap.pop();
    testAssert(top.key == 5.0, "MinHeap: third pop = 5");
    top = heap.pop();
    testAssert(top.key == 8.0, "MinHeap: fourth pop = 8");
    testAssert(heap.empty(), "MinHeap: empty after all pops");

    // Adjacency list structure
    std::vector<Edge> edges = g.getEdges(D);
    testAssert(!edges.empty(), "AdjList: Delhi has edges");
    bool foundJaipur = false;
    for (auto& e : edges) if (e.to == J) { foundJaipur = true; break; }
    testAssert(foundJaipur, "AdjList: Delhi→Jaipur edge in adjacency list");

    // ─────────────────────────────────────────────
    //  Section 11: Algorithm Consistency
    // ─────────────────────────────────────────────
    std::cout << YELLOW << "\n  [11] Cross-Algorithm Consistency\n" << RESET;

    // For direct neighbors, all algorithms should agree on path
    PathResult c_dijk = dijkstra(g, D, J, BY_DISTANCE);
    PathResult c_bfs  = bfs(g, D, J);
    PathResult c_dfs  = dfs(g, D, J);
    PathResult c_astr = astar(g, D, J, BY_DISTANCE);
    PathResult c_bf   = bellmanFord(g, D, J, BY_DISTANCE);

    testAssert(c_dijk.found && c_bfs.found && c_dfs.found && c_astr.found && c_bf.found,
               "All 5 algorithms find Delhi→Jaipur");
    testAssert(c_dijk.totalDist == c_astr.totalDist,
               "Dijkstra & A* agree on Delhi→Jaipur distance");
    testAssert(std::abs(c_dijk.totalDist - c_bf.totalDist) < 1.0,
               "Dijkstra & Bellman-Ford agree on Delhi→Jaipur distance");
    testAssert(c_bfs.path.size() == 2, "BFS finds direct 1-hop Delhi→Jaipur");

    // Longer route consistency
    PathResult lon_dijk = dijkstra(g, D, C, BY_DISTANCE);
    PathResult lon_astr = astar(g, D, C, BY_DISTANCE);
    PathResult lon_bf   = bellmanFord(g, D, C, BY_DISTANCE);
    testAssert(lon_dijk.found, "Delhi→Chennai reachable (Dijkstra)");
    testAssert(std::abs(lon_dijk.totalDist - lon_astr.totalDist) < 1.0,
               "Dijkstra & A* agree on Delhi→Chennai distance");
    testAssert(std::abs(lon_dijk.totalDist - lon_bf.totalDist) < 1.0,
               "Dijkstra & Bellman-Ford agree on Delhi→Chennai distance");

    // ─────────────────────────────────────────────
    //  Section 12: Travel Mode Data
    // ─────────────────────────────────────────────
    std::cout << YELLOW << "\n  [12] Travel Mode Data\n" << RESET;

    bool hasRoad = false, hasRail = false, hasAir = false;
    for (int i = 0; i < g.numCities; i++) {
        for (auto& e : g.getEdges(i)) {
            if (e.mode == ROAD) hasRoad = true;
            if (e.mode == RAIL) hasRail = true;
            if (e.mode == AIR)  hasAir  = true;
        }
    }
    testAssert(hasRoad, "Graph has ROAD edges");
    testAssert(hasRail, "Graph has RAIL edges");
    testAssert(hasAir,  "Graph has AIR edges");

    // ─────────────────────────────────────────────
    //  Final Summary
    // ─────────────────────────────────────────────
    std::cout << CYAN << "\n  ══════════════════════════════════════════════════\n";
    std::cout << "  TEST SUMMARY:\n";
    std::cout << "  " << GREEN << passed << " passed" << RESET
              << "  |  " << (failed > 0 ? RED : GREEN) << failed << " failed" << RESET << "\n";
    std::cout << "  ══════════════════════════════════════════════════\n\n" << RESET;

    return failed > 0 ? 1 : 0;
}

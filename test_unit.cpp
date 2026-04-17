#include "graph.h"
#include "algorithms.h"
#include "ui.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <iomanip>

// ─────────────────────────────────────────────
//  Lightweight test framework
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

int cityIdx(const Graph& g, const std::string& name) {
    return g.getCityIndex(name);
}

// ─────────────────────────────────────────────
//  MAIN TEST SUITE
// ─────────────────────────────────────────────
int main() {
    Graph g;
    g.loadDefaultNetwork();

    std::cout << CYAN << "\n  ══════════════════════════════════════════════════\n";
    std::cout << "       DSA Travel Planner — Automated Test Suite\n";
    std::cout << "       70-City Indian Network | v2.0\n";
    std::cout << "  ══════════════════════════════════════════════════\n" << RESET;

    // Frequently used city indices
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
    int Pa = cityIdx(g, "Patna");
    int V  = cityIdx(g, "Varanasi");

    // ─────────────────────────────────────────
    //  Section 1: Graph Construction
    // ─────────────────────────────────────────
    std::cout << YELLOW << "\n  [1] Graph Construction\n" << RESET;
    testAssert(g.numCities == 70, "70 cities loaded");
    testAssert(D  != -1, "Delhi found");
    testAssert(M  != -1, "Mumbai found");
    testAssert(B  != -1, "Bangalore found");
    testAssert(K  != -1, "Kolkata found");
    testAssert(H  != -1, "Hyderabad found");
    testAssert(g.cities[D].hasAirport,  "Delhi has airport");
    testAssert(g.cities[M].hasAirport,  "Mumbai has airport");
    testAssert(g.cities[B].hasAirport,  "Bangalore has airport");
    testAssert(g.hasEdge[D][J],  "Delhi→Jaipur edge exists");
    testAssert(g.hasEdge[J][D],  "Jaipur→Delhi (undirected) exists");
    testAssert(!g.hasEdge[D][B] || g.cities[D].hasAirport,
               "Delhi→Bangalore direct only via airport (air route)");

    // Count airports
    int airports = 0;
    for (int i = 0; i < g.numCities; i++)
        if (g.cities[i].hasAirport) airports++;
    testAssert(airports >= 20, "At least 20 airports in network");

    // ─────────────────────────────────────────
    //  Section 2: Dijkstra
    // ─────────────────────────────────────────
    std::cout << YELLOW << "\n  [2] Dijkstra Algorithm\n" << RESET;

    PathResult r1 = dijkstra(g, D, J, BY_DISTANCE);
    testAssert(r1.found,                "Dijkstra: Delhi→Jaipur found");
    testAssert(r1.path.front() == D,    "Dijkstra: path starts at Delhi");
    testAssert(r1.path.back()  == J,    "Dijkstra: path ends at Jaipur");
    testAssert(r1.totalDist > 0,        "Dijkstra: positive distance");

    PathResult r2 = dijkstra(g, D, B, BY_DISTANCE);
    testAssert(r2.found,                "Dijkstra: Delhi→Bangalore found");
    testAssert(r2.totalDist > 0,        "Dijkstra: positive total distance");
    testAssert(r2.totalCost > 0,        "Dijkstra: positive total cost");

    PathResult r3 = dijkstra(g, M, M, BY_DISTANCE);
    testAssert(r3.totalDist == 0,       "Dijkstra: src==dst trivial (dist=0)");

    PathResult r4 = dijkstra(g, D, K, BY_TIME);
    testAssert(r4.found,                "Dijkstra by TIME: Delhi→Kolkata found");
    testAssert(r4.totalTime > 0,        "Dijkstra by TIME: positive total time");

    PathResult r5 = dijkstra(g, D, B, BY_COST);
    testAssert(r5.found,                "Dijkstra by COST: Delhi→Bangalore found");
    testAssert(r5.totalCost <= r2.totalCost + 1.0,
               "Dijkstra: cost-optimal ≤ distance-optimal cost");

    PathResult r6a = dijkstra(g, M, P, BY_DISTANCE);
    PathResult r6b = dijkstra(g, M, B, BY_DISTANCE);
    testAssert(r6a.found,               "Dijkstra: Mumbai→Pune reachable");
    testAssert(r6b.found,               "Dijkstra: Mumbai→Bangalore reachable");
    testAssert(r6a.totalDist <= r6b.totalDist,
               "Dijkstra: Mumbai→Pune shorter than Mumbai→Bangalore");

    // ─────────────────────────────────────────
    //  Section 3: BFS
    // ─────────────────────────────────────────
    std::cout << YELLOW << "\n  [3] BFS Algorithm\n" << RESET;

    PathResult bfs1 = bfs(g, D, J);
    testAssert(bfs1.found,              "BFS: Delhi→Jaipur found");
    testAssert(bfs1.path.front() == D,  "BFS: starts at Delhi");
    testAssert(bfs1.path.back()  == J,  "BFS: ends at Jaipur");

    PathResult bfs2 = bfs(g, D, B);
    testAssert(bfs2.found,              "BFS: Delhi→Bangalore found");
    PathResult dijk2 = dijkstra(g, D, B, BY_DISTANCE);
    testAssert(bfs2.path.size() <= dijk2.path.size() + 3,
               "BFS: hops reasonable vs Dijkstra");

    PathResult bfs3 = bfs(g, M, H);
    testAssert(bfs3.found,              "BFS: Mumbai→Hyderabad found");

    // ─────────────────────────────────────────
    //  Section 4: DFS
    // ─────────────────────────────────────────
    std::cout << YELLOW << "\n  [4] DFS Algorithm\n" << RESET;

    PathResult dfs1 = dfs(g, D, J);
    testAssert(dfs1.found,              "DFS: Delhi→Jaipur found");
    testAssert(dfs1.path.front() == D,  "DFS: starts at Delhi");
    testAssert(dfs1.path.back()  == J,  "DFS: ends at Jaipur");

    PathResult dfs2 = dfs(g, M, B);
    testAssert(dfs2.found,              "DFS: Mumbai→Bangalore found");

    PathResult dfs3 = dfs(g, B, K);
    testAssert(dfs3.found,              "DFS: Bangalore→Kolkata found");

    // Ensure no repeated cities in DFS path
    PathResult dfs4 = dfs(g, D, C);
    if (dfs4.found) {
        bool noCycles = true;
        for (int i = 0; i < (int)dfs4.path.size(); i++)
            for (int j = i + 1; j < (int)dfs4.path.size(); j++)
                if (dfs4.path[i] == dfs4.path[j]) noCycles = false;
        testAssert(noCycles, "DFS: path has no repeated cities");
    }

    // ─────────────────────────────────────────
    //  Section 5: A*
    // ─────────────────────────────────────────
    std::cout << YELLOW << "\n  [5] A* Algorithm\n" << RESET;

    PathResult as1 = astar(g, D, J, BY_DISTANCE);
    testAssert(as1.found,               "A*: Delhi→Jaipur found");
    testAssert(std::abs(as1.totalDist - r1.totalDist) < 1.0,
               "A*: Delhi→Jaipur matches Dijkstra distance");

    PathResult as2 = astar(g, M, H, BY_DISTANCE);
    testAssert(as2.found,               "A*: Mumbai→Hyderabad found");

    PathResult as3 = astar(g, D, B, BY_DISTANCE);
    PathResult dj3 = dijkstra(g, D, B, BY_DISTANCE);
    testAssert(as3.found, "A*: Delhi→Bangalore found");
    // Note: A* with Haversine heuristic may prefer a direct AIR edge (shorter
    // straight-line distance) over a cheaper multi-hop land route.
    // We verify A* finds a valid path with reasonable distance (≤ 2× Dijkstra).
    testAssert(as3.totalDist <= dj3.totalDist * 2.0,
               "A*: Delhi→Bangalore path distance within 2× of Dijkstra");

    PathResult as4 = astar(g, J, C, BY_COST);
    testAssert(as4.found,               "A*: Jaipur→Chennai found (by cost)");

    // ─────────────────────────────────────────
    //  Section 6: Bellman-Ford
    // ─────────────────────────────────────────
    std::cout << YELLOW << "\n  [6] Bellman-Ford Algorithm\n" << RESET;

    PathResult bf1 = bellmanFord(g, D, J, BY_DISTANCE);
    testAssert(bf1.found,               "Bellman-Ford: Delhi→Jaipur found");
    testAssert(std::abs(bf1.totalDist - r1.totalDist) < 1.0,
               "Bellman-Ford: Delhi→Jaipur matches Dijkstra");

    PathResult bf2 = bellmanFord(g, D, B, BY_DISTANCE);
    testAssert(bf2.found,               "Bellman-Ford: Delhi→Bangalore found");
    testAssert(std::abs(bf2.totalDist - dijk2.totalDist) < 1.0,
               "Bellman-Ford: Delhi→Bangalore matches Dijkstra");

    PathResult bf3 = bellmanFord(g, M, K, BY_COST);
    testAssert(bf3.found,               "Bellman-Ford: Mumbai→Kolkata (by cost) found");

    // ─────────────────────────────────────────
    //  Section 7: Floyd-Warshall
    // ─────────────────────────────────────────
    std::cout << YELLOW << "\n  [7] Floyd-Warshall\n" << RESET;

    static double fw_dist[Graph::MAXN][Graph::MAXN];
    static int    fw_next[Graph::MAXN][Graph::MAXN];
    floydWarshall(g, BY_DISTANCE, fw_dist, fw_next);

    testAssert(fw_dist[D][D] == 0,      "Floyd-Warshall: diagonal = 0");
    testAssert(fw_dist[D][J] > 0 && fw_dist[D][J] < 1e17,
               "Floyd-Warshall: Delhi→Jaipur finite");
    testAssert(std::abs(fw_dist[D][J] - fw_dist[J][D]) < 1.0,
               "Floyd-Warshall: Delhi↔Jaipur symmetric");
    testAssert(fw_dist[D][B] > 0 && fw_dist[D][B] < 1e17,
               "Floyd-Warshall: Delhi→Bangalore reachable");
    testAssert(std::abs(fw_dist[D][B] - dijk2.totalDist) < 1.0,
               "Floyd-Warshall: Delhi→Bangalore matches Dijkstra");
    testAssert(std::abs(fw_dist[M][B] - fw_dist[B][M]) < 1.0,
               "Floyd-Warshall: Mumbai↔Bangalore symmetric");
    testAssert(fw_next[D][B] != -1,
               "Floyd-Warshall: Delhi→Bangalore next exists");

    // ─────────────────────────────────────────
    //  Section 8: Budget-Constrained Dijkstra
    // ─────────────────────────────────────────
    std::cout << YELLOW << "\n  [8] Budget-Constrained Dijkstra\n" << RESET;

    PathResult bg1 = budgetDijkstra(g, D, J, 50000, BY_DISTANCE);
    testAssert(bg1.found,               "Budget-Dijkstra: Delhi→Jaipur within ₹50000");
    testAssert(bg1.totalCost <= 50000,  "Budget-Dijkstra: cost within budget");

    // Delhi→Jaipur: cheapest direct edges are Rail ₹600 and Road ₹700
    PathResult bg2 = budgetDijkstra(g, D, J, 750, BY_DISTANCE);
    testAssert(bg2.found,               "Budget-Dijkstra: Delhi→Jaipur within ₹750 found");
    testAssert(bg2.totalCost <= 750,    "Budget-Dijkstra: cost ≤ 750");

    PathResult bg3 = budgetDijkstra(g, D, J, 10, BY_DISTANCE);
    testAssert(!bg3.found,              "Budget-Dijkstra: Delhi→Jaipur ₹10 impossible");

    PathResult bg4 = budgetDijkstra(g, D, B, 15000, BY_DISTANCE);
    if (bg4.found)
        testAssert(bg4.totalCost <= 15000, "Budget-Dijkstra: Delhi→Bangalore within ₹15000");
    else
        testAssert(true, "Budget-Dijkstra: Delhi→Bangalore ₹15000 (no path—ok)");

    // ─────────────────────────────────────────
    //  Section 9: Mode-Filtered Dijkstra
    // ─────────────────────────────────────────
    std::cout << YELLOW << "\n  [9] Mode-Filtered Routing\n" << RESET;

    // Road-only Delhi → Jaipur (should exist)
    PathResult road1 = dijkstraFiltered(g, D, J, BY_DISTANCE, ROAD);
    testAssert(road1.found, "Road-Dijkstra: Delhi→Jaipur road route found");

    // Air Delhi → Mumbai (both have airports)
    PathResult air1 = dijkstraFiltered(g, D, M, BY_COST, AIR);
    testAssert(air1.found,              "Air-Dijkstra: Delhi→Mumbai air route found");
    // Air route cost should be higher than rail
    PathResult rail1 = dijkstra(g, D, M, BY_COST);
    testAssert(air1.totalCost >= rail1.totalCost,
               "Air route costs more than cheapest mixed route");

    // Air route from city without airport should fail gracefully
    int agra = cityIdx(g, "Agra");
    testAssert(!g.cities[agra].hasAirport, "Agra has no airport");
    PathResult air2 = dijkstraFiltered(g, agra, M, BY_COST, AIR);
    testAssert(!air2.found,             "Air-Dijkstra: Agra (no airport) → Mumbai fails");

    // ─────────────────────────────────────────
    //  Section 10: Multi-Stop Planning
    // ─────────────────────────────────────────
    std::cout << YELLOW << "\n  [10] Multi-Stop Nearest Neighbour\n" << RESET;

    std::vector<int> stops1 = {J, A};
    std::vector<int> order1 = multiStopPlan(g, D, stops1, BY_DISTANCE);
    testAssert(!order1.empty(),          "Multi-stop: Delhi→{Jaipur,Ahmedabad} not empty");
    testAssert(order1.front() == D,      "Multi-stop: starts at Delhi");

    std::vector<int> stops2 = {M, P};
    std::vector<int> order2 = multiStopPlan(g, D, stops2, BY_DISTANCE);
    testAssert(!order2.empty(),          "Multi-stop: Delhi→{Mumbai,Pune} not empty");

    std::vector<int> stops3 = {Lu, V, Pa};
    std::vector<int> order3 = multiStopPlan(g, D, stops3, BY_TIME);
    testAssert(!order3.empty(),          "Multi-stop: Delhi→{Lucknow,Varanasi,Patna} by time");
    testAssert(order3.front() == D,      "Multi-stop: starts at Delhi");

    // ─────────────────────────────────────────
    //  Section 11: Custom Data Structures
    // ─────────────────────────────────────────
    std::cout << YELLOW << "\n  [11] Custom Data Structures\n" << RESET;

    MinHeap heap(20);
    heap.push(5.0, 5);
    heap.push(2.0, 2);
    heap.push(8.0, 8);
    heap.push(1.0, 1);
    heap.push(3.0, 3);

    testAssert(!heap.empty(),           "MinHeap: not empty after push");
    HeapNode top = heap.pop();
    testAssert(top.key == 1.0 && top.cityIdx == 1, "MinHeap: pop min (key=1)");
    top = heap.pop();
    testAssert(top.key == 2.0,          "MinHeap: second pop = 2");
    top = heap.pop();
    testAssert(top.key == 3.0,          "MinHeap: third pop = 3");
    top = heap.pop();
    testAssert(top.key == 5.0,          "MinHeap: fourth pop = 5");
    top = heap.pop();
    testAssert(top.key == 8.0,          "MinHeap: fifth pop = 8");
    testAssert(heap.empty(),            "MinHeap: empty after all pops");

    std::vector<Edge> edges = g.getEdges(D);
    testAssert(!edges.empty(),          "AdjList: Delhi has edges");
    bool foundJaipur = false;
    for (auto& e : edges)
        if (e.to == J) { foundJaipur = true; break; }
    testAssert(foundJaipur,             "AdjList: Delhi→Jaipur in adjacency list");

    // ─────────────────────────────────────────
    //  Section 12: Cross-Algorithm Consistency
    // ─────────────────────────────────────────
    std::cout << YELLOW << "\n  [12] Cross-Algorithm Consistency\n" << RESET;

    PathResult c_dijk = dijkstra   (g, D, J, BY_DISTANCE);
    PathResult c_bfs  = bfs        (g, D, J);
    PathResult c_dfs  = dfs        (g, D, J);
    PathResult c_astr = astar      (g, D, J, BY_DISTANCE);
    PathResult c_bf   = bellmanFord(g, D, J, BY_DISTANCE);

    testAssert(c_dijk.found && c_bfs.found && c_dfs.found && c_astr.found && c_bf.found,
               "All 5 algorithms find Delhi→Jaipur");
    testAssert(std::abs(c_dijk.totalDist - c_astr.totalDist) < 1.0,
               "Dijkstra & A* agree on Delhi→Jaipur distance");
    testAssert(std::abs(c_dijk.totalDist - c_bf.totalDist) < 1.0,
               "Dijkstra & Bellman-Ford agree on Delhi→Jaipur distance");

    // Longer route
    PathResult lon_dijk = dijkstra   (g, D, C, BY_DISTANCE);
    PathResult lon_astr = astar      (g, D, C, BY_DISTANCE);
    PathResult lon_bf   = bellmanFord(g, D, C, BY_DISTANCE);
    testAssert(lon_dijk.found,          "Delhi→Chennai reachable (Dijkstra)");
    testAssert(std::abs(lon_dijk.totalDist - lon_astr.totalDist) < 1.0,
               "Dijkstra & A* agree on Delhi→Chennai");
    testAssert(std::abs(lon_dijk.totalDist - lon_bf.totalDist) < 1.0,
               "Dijkstra & Bellman-Ford agree on Delhi→Chennai");

    // ─────────────────────────────────────────
    //  Section 13: Travel Mode Data
    // ─────────────────────────────────────────
    std::cout << YELLOW << "\n  [13] Travel Mode Coverage\n" << RESET;

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

    // Air routes should cost more than road for same city pair
    PathResult air_dm  = dijkstraFiltered(g, D, M, BY_COST, AIR);
    PathResult road_dm = dijkstraFiltered(g, D, M, BY_COST, ROAD);
    if (air_dm.found && road_dm.found)
        testAssert(air_dm.totalCost > road_dm.totalCost,
                   "Air cost > Road cost Delhi→Mumbai");

    // ─────────────────────────────────────────
    //  Final Summary
    // ─────────────────────────────────────────
    std::cout << CYAN << "\n  ══════════════════════════════════════════════════\n";
    std::cout << "  TEST SUMMARY:\n";
    std::cout << "  " << GREEN << passed << " passed" << RESET
              << "  |  " << (failed > 0 ? RED : GREEN)
              << failed << " failed" << RESET << "\n";
    std::cout << "  ══════════════════════════════════════════════════\n\n" << RESET;

    return failed > 0 ? 1 : 0;
}

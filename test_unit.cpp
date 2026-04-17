#include "graph.h"
#include "algorithms.h"
#include "ui.h"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

int passed = 0;
int failed = 0;

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

int main() {
    Graph g;
    g.loadDefaultNetwork();

    std::cout << CYAN << "\n  ================================================\n";
    std::cout << "        DSA Travel Planner Test Suite\n";
    std::cout << "  ================================================\n" << RESET;

    int D   = cityIdx(g, "Delhi");
    int M   = cityIdx(g, "Mumbai");
    int B   = cityIdx(g, "Bangalore");
    int C   = cityIdx(g, "Chennai");
    int K   = cityIdx(g, "Kolkata");
    int H   = cityIdx(g, "Hyderabad");
    int J   = cityIdx(g, "Jaipur");
    int A   = cityIdx(g, "Ahmedabad");
    int P   = cityIdx(g, "Pune");
    int Jo  = cityIdx(g, "Jodhpur");
    int Ud  = cityIdx(g, "Udaipur");
    int Ko  = cityIdx(g, "Kochi");
    int Gu  = cityIdx(g, "Guwahati");
    int Vi  = cityIdx(g, "Visakhapatnam");
    int In  = cityIdx(g, "Indore");
    int Bh  = cityIdx(g, "Bhubaneswar");
    int Au  = cityIdx(g, "Aurangabad");

    std::cout << YELLOW << "\n  [1] Graph Construction\n" << RESET;
    testAssert(g.numCities == 45, "45 cities loaded");
    testAssert(D != -1, "Delhi found");
    testAssert(Jo != -1, "Jodhpur found");
    testAssert(Ko != -1, "Kochi found");
    testAssert(Gu != -1, "Guwahati found");
    testAssert(Vi != -1, "Visakhapatnam found");
    testAssert(In != -1, "Indore found");
    testAssert(g.hasEdge[D][J], "Delhi to Jaipur edge exists");
    testAssert(g.hasEdge[J][Jo], "Jaipur to Jodhpur edge exists");
    testAssert(g.hasEdge[Jo][Ud], "Jodhpur to Udaipur edge exists");
    testAssert(g.hasEdge[K][Gu], "Kolkata to Guwahati edge exists");
    testAssert(g.hasEdge[H][Vi], "Hyderabad to Visakhapatnam edge exists");
    testAssert(g.hasEdge[In][Au], "Indore to Aurangabad edge exists");
    testAssert(!g.hasEdge[D][B], "Delhi to Bangalore is not direct");

    std::cout << YELLOW << "\n  [2] Dijkstra\n" << RESET;
    PathResult d1 = dijkstra(g, D, J, BY_DISTANCE);
    testAssert(d1.found, "Delhi to Jaipur found");
    testAssert(d1.totalDist == 281, "Delhi to Jaipur direct distance is 281");

    PathResult d2 = dijkstra(g, D, B, BY_DISTANCE);
    testAssert(d2.found, "Delhi to Bangalore found");
    testAssert(d2.totalDist > 0, "Delhi to Bangalore has positive distance");

    PathResult d3 = dijkstra(g, D, Jo, BY_DISTANCE);
    testAssert(d3.found, "Delhi to Jodhpur found");
    testAssert(d3.path.front() == D && d3.path.back() == Jo, "Delhi to Jodhpur endpoints correct");

    PathResult d4 = dijkstra(g, M, Ko, BY_DISTANCE);
    testAssert(d4.found, "Mumbai to Kochi found");
    testAssert(d4.totalCost > 0, "Mumbai to Kochi cost is positive");

    std::cout << YELLOW << "\n  [3] BFS / DFS\n" << RESET;
    PathResult bfs1 = bfs(g, D, J);
    testAssert(bfs1.found, "BFS Delhi to Jaipur found");
    testAssert(bfs1.path.size() == 2, "BFS Delhi to Jaipur is one hop");

    PathResult bfs2 = bfs(g, K, Gu);
    testAssert(bfs2.found, "BFS Kolkata to Guwahati found");

    PathResult dfs1 = dfs(g, M, B);
    testAssert(dfs1.found, "DFS Mumbai to Bangalore found");

    PathResult dfs2 = dfs(g, Jo, Bh);
    testAssert(dfs2.found, "DFS Jodhpur to Bhubaneswar found");

    std::cout << YELLOW << "\n  [4] A* / Bellman-Ford\n" << RESET;
    PathResult a1 = astar(g, D, J, BY_DISTANCE);
    testAssert(a1.found, "A* Delhi to Jaipur found");
    testAssert(std::abs(a1.totalDist - d1.totalDist) < 1.0, "A* matches Dijkstra for Delhi to Jaipur");

    PathResult a2 = astar(g, J, Ud, BY_DISTANCE);
    testAssert(a2.found, "A* Jaipur to Udaipur found");

    PathResult b1 = bellmanFord(g, D, B, BY_DISTANCE);
    testAssert(b1.found, "Bellman-Ford Delhi to Bangalore found");
    testAssert(std::abs(b1.totalDist - d2.totalDist) < 1.0, "Bellman-Ford matches Dijkstra for Delhi to Bangalore");

    PathResult b2 = bellmanFord(g, H, Vi, BY_DISTANCE);
    testAssert(b2.found, "Bellman-Ford Hyderabad to Visakhapatnam found");

    std::cout << YELLOW << "\n  [5] Floyd-Warshall\n" << RESET;
    double fwDist[Graph::MAXN][Graph::MAXN];
    int fwNext[Graph::MAXN][Graph::MAXN];
    floydWarshall(g, BY_DISTANCE, fwDist, fwNext);

    testAssert(fwDist[D][D] == 0, "Floyd diagonal is zero");
    testAssert(fwDist[D][J] == 281, "Floyd Delhi to Jaipur is 281");
    testAssert(fwNext[D][J] == J, "Floyd next step Delhi to Jaipur is Jaipur");
    testAssert(fwDist[M][Ko] > 0 && fwDist[M][Ko] < 1e17, "Floyd Mumbai to Kochi reachable");
    testAssert(std::abs(fwDist[D][B] - d2.totalDist) < 1.0, "Floyd matches Dijkstra for Delhi to Bangalore");

    std::cout << YELLOW << "\n  [6] Budget + Multi-stop\n" << RESET;
    PathResult bg1 = budgetDijkstra(g, D, J, 700, BY_DISTANCE);
    testAssert(bg1.found, "Budget Delhi to Jaipur within 700 found");
    testAssert(bg1.totalCost <= 700, "Budget Delhi to Jaipur stays under 700");

    PathResult bg2 = budgetDijkstra(g, M, Ko, 7000, BY_DISTANCE);
    testAssert(bg2.found, "Budget Mumbai to Kochi within 7000 found");
    testAssert(bg2.totalCost <= 7000, "Budget Mumbai to Kochi stays under 7000");

    std::vector<int> stops1 = {J, A};
    std::vector<int> order1 = multiStopPlan(g, D, stops1, BY_DISTANCE);
    testAssert(!order1.empty(), "Multi-stop Delhi Jaipur Ahmedabad not empty");
    testAssert(order1.front() == D, "Multi-stop starts at Delhi");

    std::vector<int> stops2 = {Jo, Ud, Bh};
    std::vector<int> order2 = multiStopPlan(g, D, stops2, BY_DISTANCE);
    testAssert(!order2.empty(), "Multi-stop expanded route not empty");
    testAssert(order2.front() == D, "Expanded multi-stop starts at Delhi");

    std::cout << YELLOW << "\n  [7] Data Structures + Travel Modes\n" << RESET;
    MinHeap heap(10);
    heap.push(5.0, 5);
    heap.push(2.0, 2);
    heap.push(8.0, 8);
    heap.push(1.0, 1);
    testAssert(heap.pop().key == 1.0, "MinHeap pop 1");
    testAssert(heap.pop().key == 2.0, "MinHeap pop 2");
    testAssert(heap.pop().key == 5.0, "MinHeap pop 5");
    testAssert(heap.pop().key == 8.0, "MinHeap pop 8");
    testAssert(heap.empty(), "MinHeap empty after pops");

    bool hasRoad = false, hasRail = false, hasAir = false;
    for (int i = 0; i < g.numCities; i++) {
        for (const Edge& e : g.getEdges(i)) {
            if (e.mode == ROAD) hasRoad = true;
            if (e.mode == RAIL) hasRail = true;
            if (e.mode == AIR) hasAir = true;
        }
    }
    testAssert(hasRoad, "Graph has road edges");
    testAssert(hasRail, "Graph has rail edges");
    testAssert(hasAir, "Graph has air edges");

    std::cout << CYAN << "\n  ================================================\n";
    std::cout << "  TEST SUMMARY: " << passed << " passed, " << failed << " failed\n";
    std::cout << "  ================================================\n\n" << RESET;

    return failed > 0 ? 1 : 0;
}

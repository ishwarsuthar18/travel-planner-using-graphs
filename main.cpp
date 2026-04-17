#include "graph.h"
#include "algorithms.h"
#include "ui.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>

// ─────────────────────────────────────────────
//  Helper: build city name array for UI
// ─────────────────────────────────────────────
static void buildNameArray(const Graph& g, std::string names[]) {
    for (int i = 0; i < g.numCities; i++)
        names[i] = g.cities[i].name;
}

// ─────────────────────────────────────────────
//  Helper: print step-by-step segment table
// ─────────────────────────────────────────────
static void printSegments(const Graph& g, const PathResult& result) {
    if (!result.found || result.path.size() < 2) return;

    std::cout << "\n  Step-by-step breakdown:\n";
    std::cout << "  " << std::string(70, '-') << "\n";
    std::cout << "  " << std::setw(22) << std::left << "Segment"
              << std::setw(10) << "Dist(km)"
              << std::setw(11) << "Time(min)"
              << std::setw(10) << "Cost(₹)"
              << "Mode\n";
    std::cout << "  " << std::string(70, '-') << "\n";

    for (int i = 0; i + 1 < (int)result.path.size(); i++) {
        int u = result.path[i];
        int v = result.path[i + 1];
        for (const Edge& e : g.getEdges(u)) {
            if (e.to == v) {
                std::string seg = g.cities[u].name + "→" + g.cities[v].name;
                std::string modeStr;
                if      (e.mode == ROAD) modeStr = "🚗 Road";
                else if (e.mode == RAIL) modeStr = "🚆 Rail";
                else                     modeStr = "✈  Air ";
                std::cout << "  " << std::setw(22) << std::left << seg
                          << std::setw(10) << (int)e.distance
                          << std::setw(11) << (int)e.time
                          << std::setw(10) << (int)e.cost
                          << modeStr << "\n";
                break;
            }
        }
    }
    std::cout << "  " << std::string(70, '-') << "\n";
    std::cout << "  " << std::setw(22) << std::left << "TOTAL"
              << std::setw(10) << (int)result.totalDist
              << std::setw(11) << (int)result.totalTime
              << std::setw(10) << (int)result.totalCost << "\n";
    std::cout << "  " << std::string(70, '-') << "\n";
}

// ═════════════════════════════════════════════
//  Menu 1 — Plan a Route (with mode filter)
// ═════════════════════════════════════════════
void menuPlanRoute(Graph& g) {
    std::string names[Graph::MAXN];
    buildNameArray(g, names);

    clearScreen();
    std::cout << CYAN << "\n  ═══ PLAN A ROUTE ════════════════════════════\n" << RESET;
    printCityList(g.numCities, names);

    int src = selectCity("Select SOURCE city",      g.numCities, names);
    int dst = selectCity("Select DESTINATION city", g.numCities, names);
    if (src == dst) {
        std::cout << RED << "  Source and destination must differ!\n" << RESET;
        pause();
        return;
    }

    int modeFilter = selectTravelMode();
    int crit       = selectCriteria();
    int algo       = selectAlgorithm();
    printAlgoExplanation(algo);
    animateSearching("  Calculating route");

    PathResult result;

    // For mode-filtered routes we use Dijkstra with mode restriction.
    // BFS and DFS do not support mode filtering (they explore all edges).
    if (modeFilter != -1 && (algo == 1 || algo == 4 || algo == 5)) {
        result = dijkstraFiltered(g, src, dst, (OptCriteria)crit, modeFilter);
        result.algoName += " (" + std::string(modeFilter == 0 ? "Road" :
                                              modeFilter == 1 ? "Rail" : "Air") + " only)";
    } else {
        switch (algo) {
            case 1: result = dijkstra   (g, src, dst, (OptCriteria)crit); break;
            case 2: result = bfs        (g, src, dst);                    break;
            case 3: result = dfs        (g, src, dst);                    break;
            case 4: result = astar      (g, src, dst, (OptCriteria)crit); break;
            case 5: result = bellmanFord(g, src, dst, (OptCriteria)crit); break;
            default: result = dijkstra  (g, src, dst, (OptCriteria)crit);
        }
    }

    std::cout << GREEN << "\n  ─── Result (" << result.algoName << ") ─────────────────────\n" << RESET;
    printPath(g, result);
    printSegments(g, result);
    pause();
}

// ═════════════════════════════════════════════
//  Menu 2 — Multi-Stop Journey
// ═════════════════════════════════════════════
void menuMultiStop(Graph& g) {
    std::string names[Graph::MAXN];
    buildNameArray(g, names);

    clearScreen();
    std::cout << CYAN << "\n  ═══ MULTI-STOP JOURNEY ══════════════════════\n" << RESET;
    printCityList(g.numCities, names);

    int start = selectCity("Select START city", g.numCities, names);

    int numStops = 0;
    std::cout << "  How many stops to add (1-8)? ";
    std::cin >> numStops;
    if (numStops < 1) numStops = 1;
    if (numStops > 8) numStops = 8;

    std::vector<int> stops;
    for (int i = 0; i < numStops; i++) {
        int s = selectCity("  Add stop " + std::to_string(i + 1), g.numCities, names);
        stops.push_back(s);
    }

    int crit = selectCriteria();
    animateSearching("  Planning optimal multi-stop route");

    std::vector<int> order = multiStopPlan(g, start, stops, (OptCriteria)crit);
    

    std::cout << GREEN << "\n  Optimal visit order:\n" << RESET;
    std::cout << "  ";
    for (int i = 0; i < (int)order.size(); i++) {
        std::cout << GREEN << g.cities[order[i]].name << RESET;
        if (i + 1 < (int)order.size()) std::cout << YELLOW << " → " << RESET;
    }
    std::cout << "\n  (Total cities in route: " << order.size() << ")\n";
    pause();
}

// ═════════════════════════════════════════════
//  Menu 3 — Compare All Algorithms
// ═════════════════════════════════════════════
void menuCompare(Graph& g) {
    std::string names[Graph::MAXN];
    buildNameArray(g, names);

    clearScreen();
    std::cout << CYAN << "\n  ═══ ALGORITHM COMPARISON ════════════════════\n" << RESET;
    printCityList(g.numCities, names);

    int src  = selectCity("Select SOURCE city",      g.numCities, names);
    int dst  = selectCity("Select DESTINATION city", g.numCities, names);
    int crit = selectCriteria();

    animateSearching("  Running all algorithms");

    std::vector<PathResult> results;
    results.push_back(dijkstra   (g, src, dst, (OptCriteria)crit));
    results.push_back(bfs        (g, src, dst));
    results.push_back(dfs        (g, src, dst));
    results.push_back(astar      (g, src, dst, (OptCriteria)crit));
    results.push_back(bellmanFord(g, src, dst, (OptCriteria)crit));

    printComparisonTable(g, src, dst, results);

    // Print each found path
    for (const auto& r : results) {
        std::cout << "\n  " << YELLOW << r.algoName << ": " << RESET;
        if (r.found) {
            for (int i = 0; i < (int)r.path.size(); i++) {
                std::cout << g.cities[r.path[i]].name;
                if (i + 1 < (int)r.path.size()) std::cout << " → ";
            }
        } else {
            std::cout << RED << "No path found" << RESET;
        }
        std::cout << "\n";
    }
    pause();
}

// ═════════════════════════════════════════════
//  Menu 4 — View Graph Structure
// ═════════════════════════════════════════════
void menuViewGraph(Graph& g) {
    clearScreen();
    std::cout << CYAN << "\n  ═══ GRAPH STRUCTURE ══════════════════════\n" << RESET;
    g.printStats();

    std::cout << "\n  " << YELLOW << "1." << RESET << " View Adjacency List\n";
    std::cout << "  " << YELLOW << "2." << RESET << " View Adjacency Matrix (first 20 cities)\n";
    std::cout << "  Choice: ";
    int c; std::cin >> c;

    if (c == 1) g.printAdjList();
    else        g.printAdjMatrix();
    pause();
}

// ═════════════════════════════════════════════
//  Menu 5 — Budget-Constrained Route
// ═════════════════════════════════════════════
void menuBudget(Graph& g) {
    std::string names[Graph::MAXN];
    buildNameArray(g, names);

    clearScreen();
    std::cout << CYAN << "\n  ═══ BUDGET-CONSTRAINED ROUTE ═══════════════\n" << RESET;
    printCityList(g.numCities, names);

    int src = selectCity("Select SOURCE city",      g.numCities, names);
    int dst = selectCity("Select DESTINATION city", g.numCities, names);

    double budget = 0;
    std::cout << "  Enter max budget (₹): ";
    std::cin >> budget;

    int crit = selectCriteria();
    animateSearching("  Finding best route within budget");

    PathResult result = budgetDijkstra(g, src, dst, budget, (OptCriteria)crit);

    std::cout << GREEN << "\n  ─── Budget Route Result ───────────────────\n" << RESET;
    if (result.found)
        std::cout << "  Budget: ₹" << (int)budget
                  << "   Actual Cost: ₹" << (int)result.totalCost << "\n";
    printPath(g, result);
    printSegments(g, result);
    pause();
}

// ═════════════════════════════════════════════
//  Menu 6 — Floyd-Warshall All-Pairs
// ═════════════════════════════════════════════
void menuFloyd(Graph& g) {
    clearScreen();
    std::cout << CYAN << "\n  ═══ FLOYD-WARSHALL ALL-PAIRS SHORTEST PATH ══\n" << RESET;
    std::cout << "  Time: O(V³)   Space: O(V²)\n\n";

    int crit = selectCriteria();
    animateSearching("  Computing all-pairs matrix");

    // Use MAXN-sized arrays on heap to avoid large stack allocation
    static double dist[Graph::MAXN][Graph::MAXN];
    static int    next[Graph::MAXN][Graph::MAXN];
    floydWarshall(g, (OptCriteria)crit, dist, next);

    std::string label = (crit == BY_TIME) ? "Time(min)" :
                        (crit == BY_COST) ? "Cost(INR)" : "Dist(km)";

    // Print sub-matrix (first 15 cities for readability on terminal)
    int show = (g.numCities > 15) ? 15 : g.numCities;
    std::cout << "\n  All-pairs " << label << " (first " << show << " cities):\n\n";

    std::cout << "              ";
    for (int j = 0; j < show; j++)
        std::cout << CYAN << std::setw(8) << g.cities[j].name.substr(0, 7) << RESET;
    std::cout << "\n";

    for (int i = 0; i < show; i++) {
        std::cout << GREEN << std::setw(14) << std::left
                  << g.cities[i].name.substr(0, 13) << RESET;
        for (int j = 0; j < show; j++) {
            if (dist[i][j] >= 1e17)
                std::cout << std::setw(8) << " INF";
            else
                std::cout << std::setw(8) << (int)dist[i][j];
        }
        std::cout << "\n";
    }

    // Interactive path trace
    std::cout << "\n  Trace a specific path? (y/n): ";
    char ch; std::cin >> ch;
    if (ch == 'y' || ch == 'Y') {
        std::string names[Graph::MAXN];
        buildNameArray(g, names);
        int src = selectCity("Select SOURCE",      g.numCities, names);
        int dst = selectCity("Select DESTINATION", g.numCities, names);
        if (dist[src][dst] >= 1e17) {
            std::cout << RED << "  No path between these cities!\n" << RESET;
        } else {
            std::cout << "\n  Path: ";
            int cur = src;
            while (cur != dst) {
                std::cout << GREEN << g.cities[cur].name << RESET << " → ";
                cur = next[cur][dst];
                if (cur == -1) break;
            }
            std::cout << GREEN << g.cities[dst].name << RESET << "\n";
            std::cout << "  Total " << label << ": "
                      << CYAN << (int)dist[src][dst] << RESET << "\n";
        }
    }
    pause();
}

// ═════════════════════════════════════════════
//  Menu 7 — City Information
// ═════════════════════════════════════════════
void menuCityInfo(Graph& g) {
    std::string names[Graph::MAXN];
    buildNameArray(g, names);

    clearScreen();
    std::cout << CYAN << "\n  ═══ CITY INFORMATION ════════════════════════\n" << RESET;
    printCityList(g.numCities, names);

    int idx = selectCity("Select city", g.numCities, names);
    const City& c = g.cities[idx];

    std::cout << "\n  ┌────────────────────────────────────────┐\n";
    std::cout << "  │  " << GREEN << std::setw(22) << std::left << c.name << RESET
              << "            │\n";
    std::cout << "  │  Latitude : " << CYAN << c.lat << RESET << "                      │\n";
    std::cout << "  │  Longitude: " << CYAN << c.lng << RESET << "                      │\n";
    std::cout << "  │  Airport  : "
              << (c.hasAirport ? GREEN "Yes ✈" : RED "No ") << RESET
              << "                     │\n";
    std::cout << "  └────────────────────────────────────────┘\n";

    // Direct connections grouped by mode
    std::vector<Edge> edges = g.getEdges(idx);
    std::cout << "\n  " << YELLOW << "Direct connections (" << edges.size() << "):\n" << RESET;
    if (edges.empty()) {
        std::cout << "  (None)\n";
    } else {
        std::cout << "  " << std::setw(18) << std::left << "City"
                  << std::setw(10) << "Dist(km)"
                  << std::setw(11) << "Time(min)"
                  << std::setw(10) << "Cost(₹)"
                  << "Mode\n";
        std::cout << "  " << std::string(58, '-') << "\n";
        for (const auto& e : edges) {
            std::string modeStr;
            if      (e.mode == ROAD) modeStr = "🚗 Road";
            else if (e.mode == RAIL) modeStr = "🚆 Rail";
            else                     modeStr = "✈  Air ";
            std::cout << "  " << std::setw(18) << std::left << g.cities[e.to].name
                      << std::setw(10) << (int)e.distance
                      << std::setw(11) << (int)e.time
                      << std::setw(10) << (int)e.cost
                      << modeStr << "\n";
        }
    }
    pause();
}

// ═════════════════════════════════════════════
//  Menu 8 — List All Airports
// ═════════════════════════════════════════════
void menuAirports(Graph& g) {
    clearScreen();
    std::cout << CYAN << "\n  ═══ AIRPORT LISTING ════════════════════════\n" << RESET;
    g.printAirports();

    // Ask if user wants air-only route between two airports
    std::cout << "\n  Plan an air-only route? (y/n): ";
    char ch; std::cin >> ch;
    if (ch == 'y' || ch == 'Y') {
        std::string names[Graph::MAXN];
        buildNameArray(g, names);
        int src  = selectCity("Select SOURCE airport city",      g.numCities, names);
        int dst  = selectCity("Select DESTINATION airport city", g.numCities, names);
        int crit = selectCriteria();
        animateSearching("  Finding air route");

        PathResult result = dijkstraFiltered(g, src, dst, (OptCriteria)crit, AIR);
        result.algoName = "Air-Dijkstra";

        if (!result.found && (!g.cities[src].hasAirport || !g.cities[dst].hasAirport)) {
            std::cout << RED << "\n  ✗ One or both cities do not have airports!\n" << RESET;
        }

        std::cout << GREEN << "\n  ─── Air Route Result ──────────────────────\n" << RESET;
        printPath(g, result);
        printSegments(g, result);
    }
    pause();
}

// ═════════════════════════════════════════════
//  MAIN
// ═════════════════════════════════════════════
int main() {
    Graph g;
    g.loadDefaultNetwork();

    clearScreen();
    printBanner();

    std::cout << GREEN << "  Network loaded: " << g.numCities
              << " cities ready!\n" << RESET;
    pause();

    int choice = 0;
    while (true) {
        clearScreen();
        printBanner();
        printMainMenu();
        std::cin >> choice;
        std::cin.ignore(10000, '\n');

        switch (choice) {
            case 1: menuPlanRoute(g); break;
            case 2: menuMultiStop(g); break;
            case 3: menuCompare(g);   break;
            case 4: menuViewGraph(g); break;
            case 5: menuBudget(g);    break;
            case 6: menuFloyd(g);     break;
            case 7: menuCityInfo(g);  break;
            case 8: menuAirports(g);  break;
            case 9:
                clearScreen();
                std::cout << CYAN << "\n  Thank you for using Travel Planner!\n" << RESET;
                std::cout << YELLOW << "  Safe travels! 🗺\n\n" << RESET;
                return 0;
            default:
                std::cout << RED << "  Invalid choice. Try again.\n" << RESET;
                break;
        }
    }
    return 0;
}

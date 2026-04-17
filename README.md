# 🗺 Travel Planner — DSA Course Project

A complete **terminal-based Indian city travel planner** built in C++ using graph data structures and 6 classic graph algorithms. Built as a DSA course project demonstrating real-world algorithm design, custom data structures, and ANSI-colored terminal UI.

---

## Features

- **45 Indian cities** pre-loaded with realistic road/rail/air connections
- **6 algorithms** all implemented from scratch
- Rich **ANSI-colored terminal UI** with menus, tables, and step-by-step breakdown
- **Budget-constrained routing**, multi-stop planning, graph visualization

---

## How to Compile

```bash
g++ -std=c++17 -o travel main.cpp graph.cpp algorithms.cpp ui.cpp
```

## How to Run

```bash
./travel
```

## How to Run Unit Tests

```bash
g++ -std=c++17 -o test_unit test_unit.cpp graph.cpp algorithms.cpp ui.cpp
./test_unit
```

---

## Project Structure

```
travel-planner/
├── main.cpp          # Menu system and entry point
├── graph.h / .cpp    # Graph data structure (adj list + adj matrix)
├── algorithms.h/.cpp # All 6 algorithms + custom data structures
├── ui.h / .cpp       # ANSI UI helpers
├── test_unit.cpp     # 83 automated unit tests
└── README.md
```

---

## Algorithms Implemented

| Algorithm       | Use Case                        | Time Complexity  | Space  |
|----------------|---------------------------------|------------------|--------|
| Dijkstra       | Shortest/cheapest/fastest path  | O((V+E) log V)   | O(V)   |
| BFS            | Fewest hops (stops)             | O(V + E)         | O(V)   |
| DFS            | Explore all routes              | O(V + E)         | O(V)   |
| A*             | Heuristic-optimal path          | O((V+E) log V)   | O(V)   |
| Bellman-Ford   | Handles negative weights        | O(V × E)         | O(V)   |
| Floyd-Warshall | All-pairs shortest paths        | O(V³)            | O(V²)  |

---

## Custom Data Structures

- **MinHeap** — custom binary min-heap used by Dijkstra and A* (no `std::priority_queue`)
- **Stack** — explicit stack used by iterative DFS
- **Queue** — circular queue used by BFS
- **AdjNode** linked list — raw-pointer adjacency list for the graph
- **Adjacency Matrix** — `double[MAXN][MAXN]` for O(1) edge existence checks

---

## Graph Data

- 45 cities including Delhi, Mumbai, Bangalore, Chennai, Kolkata, Hyderabad, Jaipur, Ahmedabad, Pune, Lucknow, Bhopal, Nagpur, Surat, Kanpur, Patna, Jodhpur, Kochi, Guwahati, Chandigarh, Udaipur, Bhubaneswar, Visakhapatnam, and more
- Each edge stores: **distance (km)**, **time (minutes)**, **cost (₹)**, **travel mode** (Road/Rail/Air)
- Realistic coordinates (lat/lng) used by A* heuristic (Haversine formula)

---

## Sample Output

```
╔══════════════════════════════════════════════════════════╗
║        DSA Course Project — Graph Algorithms v1.0        ║
╚══════════════════════════════════════════════════════════╝

  Route: Delhi → Jaipur → Ahmedabad → Mumbai

  ┌────────────────────────────────┐
  │  Total Distance :      948 km  │
  │  Total Time     :       15 hrs │
  │  Total Cost     :     ₹ 3450   │
  │  Steps Explored :        5     │
  └────────────────────────────────┘

  Algorithm Comparison:
  ╔══════════════╦══════════╦══════════╦══════════╦════════╗
  ║ Algorithm    ║ Dist(km) ║ Time(hr) ║ Cost(₹)  ║ Steps  ║
  ╠══════════════╬══════════╬══════════╬══════════╬════════╣
  ║ Dijkstra     ║ 1511     ║ 22h      ║ 4600     ║ 9      ║
  ║ BFS          ║ 1515     ║ 20h      ║ 4500     ║ 10     ║
  ║ DFS          ║ 3345     ║ 42h      ║ 9550     ║ 5      ║
  ╚══════════════╩══════════╩══════════╩══════════╩════════╝
```

---

## Learning Outcomes

- Implementing **graph representations** (adjacency list with raw pointers + adjacency matrix)
- Writing **Dijkstra's algorithm** with a custom MinHeap
- Understanding **BFS/DFS** for unweighted vs weighted traversal
- **A\*** with Haversine heuristic for geographic graphs
- **Bellman-Ford** including negative cycle detection
- **Floyd-Warshall** O(V³) all-pairs shortest path with path reconstruction
- **TSP heuristic** (nearest neighbour) for multi-stop planning
- Modular C++ with separate compilation units

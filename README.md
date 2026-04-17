# 🗺 Travel Planner v2.0 — DSA Course Project

A complete **terminal-based Indian city travel planner** built in C++ using graph data structures and 6 classic graph algorithms. Expanded to **70 cities**, **22 airports**, and **3 travel modes** (Road / Rail / Air).

---

## What's New in v2.0

| Feature | v1.0 | v2.0 |
|---|---|---|
| Cities | 15 | **70** |
| Airports | — | **22** |
| Travel modes | Mixed only | **Road / Rail / Air / Any** |
| Mode-filtered routing | ✗ | **✓** |
| Airport listing menu | ✗ | **✓** |
| Segment table mode icons | ✗ | **✓** (🚗 🚆 ✈) |
| Array sizes (MAXN) | 20 | **75** |
| `prevEdge` tracking | ✗ | **✓** (exact edge used per hop) |
| Test suite | 83 tests | **93 tests** |

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
├── main.cpp           # Menu system and entry point
├── graph.h / .cpp     # Graph data structure (adj list + adj matrix, 70 cities)
├── algorithms.h/.cpp  # All 6 algorithms + custom data structures
├── ui.h / .cpp        # ANSI UI helpers + travel mode selector
├── test_unit.cpp      # 93 automated unit tests
└── README.md
```

---

## City Network

**70 Indian cities** across all regions:

| Region | Cities |
|---|---|
| North | Delhi ★, Jaipur ★, Agra, Lucknow ★, Kanpur, Varanasi ★, Allahabad, Mathura, Amritsar ★, Chandigarh ★, Ludhiana, Dehradun ★, Meerut, Bareilly, Moradabad, Aligarh, Gorakhpur |
| East | Kolkata ★, Patna ★, Bhubaneswar ★, Guwahati ★, Ranchi ★, Dhanbad, Siliguri, Jamshedpur, Cuttack, Sambalpur, Brahmapur, Gaya, Muzaffarpur, Agartala ★, Imphal ★, Shillong, Dimapur |
| West | Mumbai ★, Pune ★, Ahmedabad ★, Surat, Vadodara, Rajkot ★, Nashik, Aurangabad ★, Solapur |
| Central | Bhopal ★, Indore ★, Nagpur ★, Jabalpur, Gwalior ★, Raipur ★ |
| South | Hyderabad ★, Bangalore ★, Chennai ★, Coimbatore ★, Kochi ★, Thiruvanantapuram ★, Visakhapatnam ★, Vijayawada, Madurai ★, Mysuru, Mangaluru ★, Tirupati, Hubli |
| Rajasthan | Jodhpur ★, Udaipur ★, Kota, Ajmer, Bikaner |
| Himachal | Shimla, Haridwar |

★ = Airport city (22 total)

---

## Algorithms Implemented

| Algorithm | Use Case | Time Complexity | Space |
|---|---|---|---|
| Dijkstra | Shortest/cheapest/fastest path | O((V+E) log V) | O(V) |
| BFS | Fewest hops (stops) | O(V + E) | O(V) |
| DFS | Explore one route deeply | O(V + E) | O(V) |
| A\* | Heuristic-guided optimal path | O((V+E) log V) | O(V) |
| Bellman-Ford | Handles negative weights | O(V × E) | O(V) |
| Floyd-Warshall | All-pairs shortest paths | O(V³) | O(V²) |

---

## Custom Data Structures

- **MinHeap** — custom binary min-heap used by Dijkstra, A\*, Budget-Dijkstra (no `std::priority_queue`)
- **Stack** — explicit stack used by iterative DFS
- **Queue** — circular buffer queue used by BFS
- **AdjNode** linked list — raw-pointer adjacency list (demonstrates pointer DSA)
- **Adjacency Matrix** — `double[MAXN][MAXN]` for O(1) edge existence checks
- **prevEdge tracking** — each algorithm stores the actual `Edge` struct used per hop, ensuring accurate cost/time/distance reporting when parallel edges (road + rail + air) exist between cities

---

## Edge Data (Realistic)

Each edge stores: **distance (km)**, **time (minutes)**, **cost (₹)**, **travel mode**

| Mode | Avg Speed | Cost Model |
|---|---|---|
| 🚗 Road | ~50 km/h | ~₹2.5/km |
| 🚆 Rail | ~60 km/h | ~₹2.0/km (express trains) |
| ✈ Air | ~800 km/h | ₹3500 base + ₹5/km (budget airline) |

Air routes connect only airport cities. Air is fastest but most expensive; road is usually cheapest for short distances; rail balances speed and cost for long distances.

---

## Menu Options

```
1. Plan a Route           — choose mode (Any/Road/Rail/Air) + algorithm + criteria
2. Multi-Stop Journey     — nearest-neighbour TSP for up to 8 waypoints
3. Compare All Algorithms — run all 5 algorithms side-by-side
4. View Graph Structure   — adjacency list or matrix
5. Budget-Constrained     — find best path within a ₹ budget
6. Floyd-Warshall         — all-pairs matrix + interactive path trace
7. City Information       — lat/lng, airport status, direct connections
8. List All Airports      — view all 22 airports; option to plan air-only route
9. Exit
```

---

## Key Bug Fixes (v1.0 → v2.0)

1. **Array bounds**: All arrays sized to `MAXN=75` (was 20) — supports 70+ cities without overflow
2. **Parallel edge ambiguity**: Added `prevEdge[]` to all weighted algorithms (Dijkstra, A\*, Bellman-Ford, Budget-Dijkstra). When road + rail + air edges exist between the same two cities, the reported distance/time/cost now reflects the *actual edge traversed*, not a guess based on criteria
3. **Budget filter**: `budgetDijkstra` now correctly blocks edges whose cumulative cost exceeds budget; `buildResult` no longer picks a cheaper-but-wrong parallel edge post-hoc
4. **Mode filter**: `dijkstraFiltered` correctly restricts traversal to a single mode and reports actual costs for that mode
5. **Floyd-Warshall stack overflow**: Moved `dist[MAXN][MAXN]` arrays from stack to `static` storage in `menuFloyd` to prevent stack overflow with 70 cities
6. **DFS/BFS capacity**: Queue/Stack capacities now `n*n*4 + 100` to handle deep paths in 70-city graph

---

## Learning Outcomes

- Implementing **graph representations** (adj list with raw pointers + adj matrix)
- **Dijkstra** with a custom MinHeap; **exact edge tracking** via `prevEdge[]`
- **BFS/DFS** for unweighted vs weighted traversal
- **A\*** with Haversine heuristic; admissibility caveats in mixed-mode graphs
- **Bellman-Ford** with negative cycle detection and early-exit optimisation
- **Floyd-Warshall** O(V³) all-pairs with path reconstruction
- **TSP nearest-neighbour heuristic** for multi-stop planning
- **Budget-constrained Dijkstra** — modified relaxation with cost tracking
- **Mode-filtered routing** — subgraph search via edge predicate
- Modular C++ with separate compilation units, zero compiler warnings

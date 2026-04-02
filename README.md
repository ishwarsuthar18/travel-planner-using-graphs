# Travel Planner Project
Current Approach
This project implements a City Route Planner using a weighted, undirected graph structure to represent a network of Indian cities. The core objective is to provide an interactive interface where users can calculate optimal travel paths based on different constraints like distance or cost.

Key Architectural Decisions:

Adjacency List Representation: The Graph class uses a vector<vector<Edge>> to store connections, ensuring efficient traversal for sparse city networks.

Decoupled Logic: The system is split into specialized modules: Graph (data structure), Algorithms (pathfinding logic), FileIO (persistence), and Menu (UI).

Dual-Weight Dijkstra: Unlike standard implementations, this version allows users to toggle between minimizing distance (km) or cost (INR) using the same underlying graph.

Progress So Far
1. Core Infrastructure (Completed)
Dynamic Graph Building: Implemented functionality to add cities and routes (both uni- and bidirectional) on the fly.

File Persistence: Developed a robust FileIO system that can parse custom text formats and save the current graph state or algorithm results to external files.

Interactive CLI: Created a multi-option menu system with input validation to prevent crashes when a user enters a non-existent city.

2. Algorithms Implemented (Completed)
Dijkstra's Algorithm: Fully functional for both Shortest Distance and Cheapest Cost.

BFS (Breadth-First Search): Implemented to show reachable cities level-by-level.

DFS (Depth-First Search): Implemented to explore and list all possible routes from a source.

Prim's Algorithm: Computes the Minimum Spanning Tree (MST) to find the most cost-effective way to connect the entire network.

3. Current Status (Ongoing)
Sample Data Integration: The system currently pre-loads a network of major Indian cities (Delhi, Mumbai, Bengaluru, etc.) if no external file is provided.

Testing: Basic path existence checks and reachability logic are verified and working.

Upcoming Milestones
Integrate more complex graph constraints (e.g., time-based weights).

Enhance the DFS "All Routes" logic to include filtering (e.g., "only routes under 1000km").

Improve memory management for very large city datasets

#pragma once
#include "graph.h"
#include <vector>
#include <string>

// ─────────────────────────────────────────────
//  Result type returned by path-finding algos
// ─────────────────────────────────────────────
struct PathResult {
    std::vector<int> path;   // city indices along the route
    double totalDist;        // km
    double totalTime;        // minutes
    double totalCost;        // INR
    bool   found;
    int    stepsExplored;    // nodes visited during search
    std::string algoName;
};

// ─────────────────────────────────────────────
//  Custom MinHeap (no std::priority_queue)
// ─────────────────────────────────────────────
struct HeapNode {
    double key;
    int    cityIdx;
};

class MinHeap {
    HeapNode* data;
    int       sz;
    int       capacity;
    void heapifyUp(int i);
    void heapifyDown(int i);
public:
    MinHeap(int cap);
    ~MinHeap();
    void     push(double key, int city);
    HeapNode pop();
    bool     empty() const { return sz == 0; }
};

// ─────────────────────────────────────────────
//  Custom Stack (for DFS)
// ─────────────────────────────────────────────
struct StackNode {
    int cityIdx;
    std::vector<int> path;
};

class Stack {
    StackNode* data;
    int        top;
    int        capacity;
public:
    Stack(int cap);
    ~Stack();
    void      push(StackNode n);
    StackNode pop();
    bool      empty() const { return top < 0; }
};

// ─────────────────────────────────────────────
//  Custom Queue (for BFS)
// ─────────────────────────────────────────────
struct QueueNode {
    int cityIdx;
    std::vector<int> path;
};

class Queue {
    QueueNode* data;
    int        head, tail, cap;
public:
    Queue(int cap);
    ~Queue();
    void      push(QueueNode n);
    QueueNode pop();
    bool      empty() const { return head == tail; }
};

// ─────────────────────────────────────────────
//  Optimization criteria
// ─────────────────────────────────────────────
enum OptCriteria { BY_DISTANCE = 0, BY_TIME = 1, BY_COST = 2 };

// ─────────────────────────────────────────────
//  Algorithm functions
// ─────────────────────────────────────────────

// Standard shortest-path algorithms
PathResult dijkstra   (const Graph& g, int src, int dst, OptCriteria crit);
PathResult bfs        (const Graph& g, int src, int dst);
PathResult dfs        (const Graph& g, int src, int dst);
PathResult astar      (const Graph& g, int src, int dst, OptCriteria crit);
PathResult bellmanFord(const Graph& g, int src, int dst, OptCriteria crit);

// All-pairs shortest paths (Floyd-Warshall)
// Arrays sized to MAXN = 75
void floydWarshall(const Graph& g, OptCriteria crit,
                   double dist[][Graph::MAXN], int next[][Graph::MAXN]);

// Budget-constrained routing
PathResult budgetDijkstra(const Graph& g, int src, int dst,
                          double maxBudget, OptCriteria crit);

// Mode-filtered routing: only use edges of given mode (or ANY)
// modeFilter = -1 means no filter (any mode allowed)
PathResult dijkstraFiltered(const Graph& g, int src, int dst,
                            OptCriteria crit, int modeFilter);

// Multi-stop: nearest-neighbour TSP heuristic
std::vector<int> multiStopPlan(const Graph& g, int start,
                               std::vector<int>& stops, OptCriteria crit);

// Print helpers
void printPath            (const Graph& g, const PathResult& r);
void printComparisonTable (const Graph& g, int src, int dst,
                           const std::vector<PathResult>& results);

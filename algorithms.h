#pragma once
#include "graph.h"
#include <vector>
#include <string>

// ─────────────────────────────────────────────
//  Result type returned by path-finding algos
// ─────────────────────────────────────────────
struct PathResult {
    std::vector<int> path;   // city indices
    double totalDist;
    double totalTime;
    double totalCost;
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
    int       size;
    int       capacity;
    void heapifyUp(int i);
    void heapifyDown(int i);
public:
    MinHeap(int cap);
    ~MinHeap();
    void   push(double key, int city);
    HeapNode pop();
    bool   empty() const { return size == 0; }
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
    void push(StackNode n);
    StackNode pop();
    bool empty() const { return top < 0; }
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
    void push(QueueNode n);
    QueueNode pop();
    bool empty() const { return head == tail; }
};

// ─────────────────────────────────────────────
//  Algorithm functions
// ─────────────────────────────────────────────
enum OptCriteria { BY_DISTANCE = 0, BY_TIME = 1, BY_COST = 2 };

PathResult dijkstra(const Graph& g, int src, int dst, OptCriteria crit);
PathResult bfs(const Graph& g, int src, int dst);
PathResult dfs(const Graph& g, int src, int dst);
PathResult astar(const Graph& g, int src, int dst, OptCriteria crit);
PathResult bellmanFord(const Graph& g, int src, int dst, OptCriteria crit);
void       floydWarshall(const Graph& g, OptCriteria crit,
                         double dist[][Graph::MAXN], int next[][Graph::MAXN]);

PathResult budgetDijkstra(const Graph& g, int src, int dst,
                          double maxBudget, OptCriteria crit);

// Multi-stop: nearest-neighbour TSP heuristic
std::vector<int> multiStopPlan(const Graph& g, int start,
                               std::vector<int>& stops, OptCriteria crit);

// Print helpers
void printPath(const Graph& g, const PathResult& r);
void printComparisonTable(const Graph& g, int src, int dst,
                          const std::vector<PathResult>& results);

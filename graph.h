#pragma once
#include <string>
#include <vector>

// ─────────────────────────────────────────────
//  Data Structures
// ─────────────────────────────────────────────

enum TravelMode { ROAD = 0, RAIL = 1, AIR = 2 };

struct City {
    int    id;
    std::string name;
    double lat;
    double lng;
};

struct Edge {
    int    to;
    double distance;   // km
    double time;       // minutes
    double cost;       // INR
    TravelMode mode;
};

// Adjacency-list node
struct AdjNode {
    Edge   edge;
    AdjNode* next;
    AdjNode(Edge e) : edge(e), next(nullptr) {}
};

// ─────────────────────────────────────────────
//  Graph class
// ─────────────────────────────────────────────
class Graph {
public:
    static const int MAXN = 60;
    int   numCities;
    City  cities[MAXN];

    // Adjacency list (raw pointers to show DSA knowledge)
    AdjNode* adjList[MAXN];

    // Adjacency matrix [from][to] — stores distance (0 = no edge)
    double adjMatrix[MAXN][MAXN];
    bool   hasEdge[MAXN][MAXN];

    Graph();
    ~Graph();

    void addCity(int id, const std::string& name, double lat, double lng);
    void addEdge(int from, int to, double dist, double time, double cost,
                 TravelMode mode, bool undirected = true);

    void loadDefaultNetwork();   // pre-loads 45 Indian cities

    // Utilities
    int  getCityIndex(const std::string& name) const;
    void printAdjList() const;
    void printAdjMatrix() const;
    void printStats() const;

    // Returns edges from a city (helper for algorithms)
    std::vector<Edge> getEdges(int from) const;
};

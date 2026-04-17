#pragma once
#include <string>
#include <vector>

// ─────────────────────────────────────────────
//  Travel Modes
// ─────────────────────────────────────────────
enum TravelMode { ROAD = 0, RAIL = 1, AIR = 2 };

// ─────────────────────────────────────────────
//  City: stores name, coordinates, airport flag
// ─────────────────────────────────────────────
struct City {
    int    id;
    std::string name;
    double lat;
    double lng;
    bool   hasAirport;   // true if this city has a commercial airport
};

// ─────────────────────────────────────────────
//  Edge: a connection between two cities
// ─────────────────────────────────────────────
struct Edge {
    int    to;
    double distance;   // km
    double time;       // minutes
    double cost;       // INR
    TravelMode mode;
};

// ─────────────────────────────────────────────
//  Adjacency-list node (raw pointer — DSA style)
// ─────────────────────────────────────────────
struct AdjNode {
    Edge     edge;
    AdjNode* next;
    AdjNode(Edge e) : edge(e), next(nullptr) {}
};

// ─────────────────────────────────────────────
//  Graph class
// ─────────────────────────────────────────────
class Graph {
public:
    static const int MAXN = 75;   // supports up to 75 cities

    int   numCities;
    City  cities[MAXN];

    // Adjacency list — raw pointers to demonstrate DSA knowledge
    AdjNode* adjList[MAXN];

    // Adjacency matrix — stores distance (0 = no direct edge)
    double adjMatrix[MAXN][MAXN];
    bool   hasEdge[MAXN][MAXN];

    Graph();
    ~Graph();

    // Build the graph
    void addCity(int id, const std::string& name,
                 double lat, double lng, bool hasAirport = false);
    void addEdge(int from, int to,
                 double dist, double time, double cost,
                 TravelMode mode, bool undirected = true);

    // Load the full 70-city Indian network
    void loadDefaultNetwork();

    // Utilities
    int  getCityIndex(const std::string& name) const;
    void printAdjList()   const;
    void printAdjMatrix() const;
    void printStats()     const;
    void printAirports()  const;

    // Returns all edges leaving a city
    std::vector<Edge> getEdges(int from) const;

    // Returns only edges of a specific mode
    std::vector<Edge> getEdgesByMode(int from, TravelMode mode) const;
};

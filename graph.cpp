#include "graph.h"
#include "ui.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>

Graph::Graph() : numCities(0) {
    for (int i = 0; i < MAXN; i++) {
        adjList[i] = nullptr;
        for (int j = 0; j < MAXN; j++) {
            adjMatrix[i][j] = 0.0;
            hasEdge[i][j]   = false;
        }
    }
}

Graph::~Graph() {
    for (int i = 0; i < MAXN; i++) {
        AdjNode* cur = adjList[i];
        while (cur) {
            AdjNode* tmp = cur->next;
            delete cur;
            cur = tmp;
        }
    }
}

void Graph::addCity(int id, const std::string& name, double lat, double lng) {
    cities[numCities].id   = id;
    cities[numCities].name = name;
    cities[numCities].lat  = lat;
    cities[numCities].lng  = lng;
    numCities++;
}

void Graph::addEdge(int from, int to, double dist, double time, double cost,
                    TravelMode mode, bool undirected) {
    // --- Adjacency list ---
    Edge e1 = {to, dist, time, cost, mode};
    AdjNode* node1 = new AdjNode(e1);
    node1->next    = adjList[from];
    adjList[from]  = node1;

    // --- Adjacency matrix (distance) ---
    adjMatrix[from][to] = dist;
    hasEdge[from][to]   = true;

    if (undirected) {
        Edge e2 = {from, dist, time, cost, mode};
        AdjNode* node2 = new AdjNode(e2);
        node2->next    = adjList[to];
        adjList[to]    = node2;

        adjMatrix[to][from] = dist;
        hasEdge[to][from]   = true;
    }
}

std::vector<Edge> Graph::getEdges(int from) const {
    std::vector<Edge> result;
    AdjNode* cur = adjList[from];
    while (cur) {
        result.push_back(cur->edge);
        cur = cur->next;
    }
    return result;
}

// ─────────────────────────────────────────────
//  Pre-loaded Indian City Network (15 cities)
// ─────────────────────────────────────────────
void Graph::loadDefaultNetwork() {
    // id, name, lat, lng
    addCity(0,  "Delhi",       28.61, 77.21);
    addCity(1,  "Mumbai",      19.07, 72.87);
    addCity(2,  "Bangalore",   12.97, 77.59);
    addCity(3,  "Chennai",     13.08, 80.27);
    addCity(4,  "Kolkata",     22.57, 88.36);
    addCity(5,  "Hyderabad",   17.38, 78.47);
    addCity(6,  "Jaipur",      26.91, 75.79);
    addCity(7,  "Ahmedabad",   23.02, 72.57);
    addCity(8,  "Pune",        18.52, 73.86);
    addCity(9,  "Lucknow",     26.85, 80.95);
    addCity(10, "Bhopal",      23.25, 77.40);
    addCity(11, "Nagpur",      21.14, 79.08);
    addCity(12, "Surat",       21.17, 72.83);
    addCity(13, "Kanpur",      26.45, 80.33);
    addCity(14, "Patna",       25.59, 85.13);

    // from, to,  dist,  time(min), cost(INR), mode
    // Delhi connections
    addEdge(0, 6,   281,  300,   650,  ROAD);
    addEdge(0, 9,   508,  480,  1200,  RAIL);
    addEdge(0, 13,  430,  420,  1050,  RAIL);
    addEdge(0, 4,  1472, 1080,  5500,  AIR);
    addEdge(0, 1,  1415,  960,  4200,  AIR);
    addEdge(0, 10,  777,  660,  1800,  RAIL);

    // Mumbai connections
    addEdge(1, 7,   534,  480,  1300,  ROAD);
    addEdge(1, 8,   149,  180,   450,  ROAD);
    addEdge(1, 2,   981,  720,  3200,  AIR);
    addEdge(1, 5,   710,  600,  2800,  AIR);
    addEdge(1, 12,  263,  300,   700,  ROAD);

    // Bangalore connections
    addEdge(2, 3,   346,  360,  1100,  ROAD);
    addEdge(2, 5,   570,  480,  1500,  ROAD);
    addEdge(2, 8,   837,  660,  2600,  RAIL);

    // Chennai connections
    addEdge(3, 5,   629,  540,  1800,  ROAD);
    addEdge(3, 4,  1659, 1200,  5800,  AIR);

    // Kolkata connections
    addEdge(4, 14,  580,  480,  1400,  RAIL);
    addEdge(4, 9,  1014,  780,  3500,  AIR);

    // Hyderabad connections
    addEdge(5, 11,  494,  420,  1300,  ROAD);
    addEdge(5, 10,  601,  540,  1600,  ROAD);

    // Jaipur connections
    addEdge(6, 7,   668,  540,  1500,  ROAD);
    addEdge(6, 10,  577,  480,  1400,  RAIL);

    // Ahmedabad connections
    addEdge(7, 12,  267,  300,   700,  ROAD);
    addEdge(7, 10,  494,  420,  1200,  ROAD);

    // Bhopal connections
    addEdge(10, 11, 352,  360,   950,  ROAD);
    addEdge(10, 13, 408,  390,  1050,  RAIL);

    // Lucknow connections
    addEdge(9, 13,  80,   90,   350,  ROAD);
    addEdge(9, 14,  571,  480,  1500,  RAIL);

    // Kanpur connections
    addEdge(13, 14, 618,  540,  1600,  RAIL);
}

// ─────────────────────────────────────────────
//  Print adjacency list
// ─────────────────────────────────────────────
void Graph::printAdjList() const {
    using namespace std;
    cout << CYAN << "\n  Adjacency List Representation:\n" << RESET;
    cout << YELLOW << "  " << string(60, '-') << "\n" << RESET;
    for (int i = 0; i < numCities; i++) {
        cout << GREEN << "  " << setw(12) << left << cities[i].name << RESET << " → ";
        AdjNode* cur = adjList[i];
        if (!cur) { cout << "(no outgoing edges)\n"; continue; }
        while (cur) {
            cout << CYAN << cities[cur->edge.to].name << RESET
                 << " [" << (int)cur->edge.distance << "km/"
                 << (int)cur->edge.time << "min/₹"
                 << (int)cur->edge.cost << "]";
            cur = cur->next;
            if (cur) cout << " → ";
        }
        cout << "\n";
    }
}

// ─────────────────────────────────────────────
//  Print adjacency matrix (distance)
// ─────────────────────────────────────────────
void Graph::printAdjMatrix() const {
    using namespace std;
    cout << CYAN << "\n  Adjacency Matrix (Distance in km, 0 = no direct edge):\n" << RESET;
    cout << YELLOW << "  " << string(5 + numCities * 7, '-') << "\n" << RESET;

    // Header
    cout << "       ";
    for (int j = 0; j < numCities; j++)
        cout << setw(6) << left << cities[j].name.substr(0,5);
    cout << "\n";

    for (int i = 0; i < numCities; i++) {
        cout << GREEN << setw(12) << left << cities[i].name.substr(0,11) << RESET;
        for (int j = 0; j < numCities; j++) {
            if (i == j)
                cout << YELLOW << setw(6) << "  0  " << RESET;
            else if (hasEdge[i][j])
                cout << CYAN << setw(6) << (int)adjMatrix[i][j] << RESET;
            else
                cout << setw(6) << "  -  ";
        }
        cout << "\n";
    }
}

// ─────────────────────────────────────────────
//  Graph statistics
// ─────────────────────────────────────────────
void Graph::printStats() const {
    using namespace std;
    int edgeCount = 0;
    for (int i = 0; i < numCities; i++) {
        AdjNode* cur = adjList[i];
        while (cur) { edgeCount++; cur = cur->next; }
    }
    int undirectedEdges = edgeCount / 2;
    cout << GREEN << "\n  Graph Statistics:\n" << RESET;
    cout << "  Total Cities  : " << CYAN << numCities << RESET << "\n";
    cout << "  Total Edges   : " << CYAN << undirectedEdges << RESET << " (undirected)\n";
    cout << "  Avg Degree    : " << CYAN
         << fixed << setprecision(2) << (double)edgeCount / numCities << RESET << "\n";
}

int Graph::getCityIndex(const std::string& name) const {
    for (int i = 0; i < numCities; i++)
        if (cities[i].name == name) return i;
    return -1;
}

#include "graph.h"
#include "ui.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>

namespace {
struct CitySeed {
    int id;
    const char* name;
    double lat;
    double lng;
};

struct EdgeSeed {
    int from;
    int to;
    double distance;
    double time;
    double cost;
    TravelMode mode;
};
}

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
    if (numCities >= MAXN) return;
    cities[numCities].id   = id;
    cities[numCities].name = name;
    cities[numCities].lat  = lat;
    cities[numCities].lng  = lng;
    numCities++;
}

void Graph::addEdge(int from, int to, double dist, double time, double cost,
                    TravelMode mode, bool undirected) {
    Edge e1 = {to, dist, time, cost, mode};
    AdjNode* node1 = new AdjNode(e1);
    node1->next    = adjList[from];
    adjList[from]  = node1;

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

// Pre-loaded Indian City Network (45 cities)
void Graph::loadDefaultNetwork() {
    numCities = 0;
    for (int i = 0; i < MAXN; i++) {
        AdjNode* cur = adjList[i];
        while (cur) {
            AdjNode* tmp = cur->next;
            delete cur;
            cur = tmp;
        }
        adjList[i] = nullptr;
        for (int j = 0; j < MAXN; j++) {
            adjMatrix[i][j] = 0.0;
            hasEdge[i][j]   = false;
        }
    }

    static const CitySeed citySeeds[] = {
        {0,  "Delhi",             28.61, 77.21},
        {1,  "Mumbai",            19.07, 72.87},
        {2,  "Bangalore",         12.97, 77.59},
        {3,  "Chennai",           13.08, 80.27},
        {4,  "Kolkata",           22.57, 88.36},
        {5,  "Hyderabad",         17.38, 78.47},
        {6,  "Jaipur",            26.91, 75.79},
        {7,  "Ahmedabad",         23.02, 72.57},
        {8,  "Pune",              18.52, 73.86},
        {9,  "Lucknow",           26.85, 80.95},
        {10, "Bhopal",            23.25, 77.40},
        {11, "Nagpur",            21.14, 79.08},
        {12, "Surat",             21.17, 72.83},
        {13, "Kanpur",            26.45, 80.33},
        {14, "Patna",             25.59, 85.13},
        {15, "Chandigarh",        30.73, 76.78},
        {16, "Jodhpur",           26.24, 73.02},
        {17, "Udaipur",           24.58, 73.68},
        {18, "Amritsar",          31.63, 74.87},
        {19, "Dehradun",          30.32, 78.03},
        {20, "Varanasi",          25.32, 82.99},
        {21, "Prayagraj",         25.44, 81.84},
        {22, "Ranchi",            23.34, 85.31},
        {23, "Guwahati",          26.14, 91.74},
        {24, "Srinagar",          34.08, 74.79},
        {25, "Jammu",             32.73, 74.87},
        {26, "Indore",            22.72, 75.86},
        {27, "Raipur",            21.25, 81.63},
        {28, "Bhubaneswar",       20.30, 85.82},
        {29, "Visakhapatnam",     17.69, 83.22},
        {30, "Vijayawada",        16.51, 80.65},
        {31, "Mysore",            12.30, 76.64},
        {32, "Mangalore",         12.91, 74.85},
        {33, "Kochi",              9.93, 76.27},
        {34, "Thiruvananthapuram", 8.52, 76.94},
        {35, "Coimbatore",        11.02, 76.96},
        {36, "Madurai",            9.93, 78.12},
        {37, "Goa",               15.49, 73.83},
        {38, "Nashik",            19.99, 73.79},
        {39, "Aurangabad",        19.88, 75.34},
        {40, "Vadodara",          22.31, 73.18},
        {41, "Rajkot",            22.30, 70.80},
        {42, "Ludhiana",          30.90, 75.85},
        {43, "Agra",              27.18, 78.01},
        {44, "Gwalior",           26.22, 78.18}
    };

    static const EdgeSeed edgeSeeds[] = {
        {0, 6, 281, 300, 650, ROAD},
        {0, 9, 508, 480, 1200, RAIL},
        {0, 13, 430, 420, 1050, RAIL},
        {0, 4, 1472, 1080, 5500, AIR},
        {0, 1, 1415, 960, 4200, AIR},
        {0, 10, 777, 660, 1800, RAIL},
        {1, 7, 534, 480, 1300, ROAD},
        {1, 8, 149, 180, 450, ROAD},
        {1, 2, 981, 720, 3200, AIR},
        {1, 5, 710, 600, 2800, AIR},
        {1, 12, 263, 300, 700, ROAD},
        {2, 3, 346, 360, 1100, ROAD},
        {2, 5, 570, 480, 1500, ROAD},
        {2, 8, 837, 660, 2600, RAIL},
        {3, 5, 629, 540, 1800, ROAD},
        {3, 4, 1659, 1200, 5800, AIR},
        {4, 14, 580, 480, 1400, RAIL},
        {4, 9, 1014, 780, 3500, AIR},
        {5, 11, 494, 420, 1300, ROAD},
        {5, 10, 601, 540, 1600, ROAD},
        {6, 7, 668, 540, 1500, ROAD},
        {6, 10, 577, 480, 1400, RAIL},
        {7, 12, 267, 300, 700, ROAD},
        {7, 10, 494, 420, 1200, ROAD},
        {10, 11, 352, 360, 950, ROAD},
        {10, 13, 408, 390, 1050, RAIL},
        {9, 13, 80, 90, 350, ROAD},
        {9, 14, 571, 480, 1500, RAIL},
        {13, 14, 618, 540, 1600, RAIL},
        {0, 15, 260, 300, 700, ROAD},
        {15, 18, 230, 240, 600, ROAD},
        {15, 42, 110, 120, 250, ROAD},
        {15, 19, 200, 240, 550, ROAD},
        {15, 25, 340, 360, 900, ROAD},
        {18, 42, 140, 150, 350, ROAD},
        {25, 24, 265, 360, 850, ROAD},
        {0, 43, 233, 210, 500, ROAD},
        {43, 44, 120, 120, 300, ROAD},
        {44, 10, 420, 360, 900, ROAD},
        {42, 0, 310, 300, 800, RAIL},
        {19, 0, 250, 300, 700, ROAD},
        {19, 9, 570, 480, 1400, RAIL},
        {24, 0, 876, 100, 3200, AIR},
        {25, 42, 280, 300, 700, ROAD},
        {18, 0, 450, 360, 1100, RAIL},
        {6, 16, 350, 330, 800, ROAD},
        {16, 17, 250, 300, 700, ROAD},
        {17, 7, 260, 300, 700, ROAD},
        {16, 7, 450, 420, 1100, RAIL},
        {17, 40, 320, 360, 800, ROAD},
        {40, 7, 110, 90, 250, ROAD},
        {40, 1, 420, 360, 1100, RAIL},
        {7, 41, 215, 240, 550, ROAD},
        {41, 40, 300, 330, 800, ROAD},
        {41, 1, 750, 540, 2300, AIR},
        {17, 6, 395, 420, 950, ROAD},
        {26, 10, 190, 210, 450, ROAD},
        {26, 1, 585, 480, 1600, RAIL},
        {26, 7, 400, 360, 1000, ROAD},
        {26, 39, 405, 390, 950, ROAD},
        {39, 38, 185, 210, 450, ROAD},
        {38, 8, 210, 240, 500, ROAD},
        {38, 1, 167, 180, 400, ROAD},
        {39, 5, 560, 480, 1500, RAIL},
        {37, 8, 450, 600, 1300, ROAD},
        {37, 32, 360, 390, 900, ROAD},
        {37, 1, 590, 480, 1800, AIR},
        {27, 11, 285, 300, 700, ROAD},
        {27, 22, 560, 480, 1400, RAIL},
        {27, 28, 550, 480, 1400, RAIL},
        {27, 26, 690, 600, 1800, RAIL},
        {27, 5, 780, 660, 2200, AIR},
        {22, 14, 330, 360, 800, ROAD},
        {22, 4, 410, 360, 1000, RAIL},
        {20, 21, 125, 120, 300, ROAD},
        {20, 14, 260, 240, 650, ROAD},
        {21, 9, 200, 210, 500, ROAD},
        {21, 13, 205, 210, 500, ROAD},
        {20, 9, 320, 300, 800, ROAD},
        {20, 4, 680, 480, 1800, RAIL},
        {21, 43, 470, 420, 1200, RAIL},
        {28, 4, 440, 360, 1100, RAIL},
        {28, 29, 445, 420, 1200, RAIL},
        {29, 30, 350, 330, 850, ROAD},
        {30, 5, 275, 300, 700, ROAD},
        {30, 3, 450, 420, 1200, RAIL},
        {29, 5, 620, 540, 1700, RAIL},
        {23, 4, 1030, 100, 3800, AIR},
        {23, 14, 850, 720, 2600, RAIL},
        {23, 28, 1450, 120, 4200, AIR},
        {31, 2, 145, 180, 350, ROAD},
        {31, 35, 200, 240, 550, ROAD},
        {35, 36, 215, 240, 550, ROAD},
        {35, 33, 190, 210, 500, ROAD},
        {33, 34, 210, 240, 550, ROAD},
        {32, 2, 350, 420, 900, ROAD},
        {32, 33, 420, 480, 1100, ROAD},
        {33, 2, 550, 480, 1600, RAIL},
        {34, 36, 300, 300, 800, ROAD},
        {36, 3, 460, 480, 1200, RAIL},
        {35, 3, 500, 480, 1300, RAIL},
        {33, 5, 1100, 90, 3500, AIR}
    };

    for (const auto& city : citySeeds)
        addCity(city.id, city.name, city.lat, city.lng);

    for (const auto& edge : edgeSeeds)
        addEdge(edge.from, edge.to, edge.distance, edge.time, edge.cost, edge.mode);
}

void Graph::printAdjList() const {
    using namespace std;
    cout << CYAN << "\n  Adjacency List Representation:\n" << RESET;
    cout << YELLOW << "  " << string(60, '-') << "\n" << RESET;
    for (int i = 0; i < numCities; i++) {
        cout << GREEN << "  " << setw(12) << left << cities[i].name << RESET << " -> ";
        AdjNode* cur = adjList[i];
        if (!cur) { cout << "(no outgoing edges)\n"; continue; }
        while (cur) {
            cout << CYAN << cities[cur->edge.to].name << RESET
                 << " [" << (int)cur->edge.distance << "km/"
                 << (int)cur->edge.time << "min/Rs"
                 << (int)cur->edge.cost << "]";
            cur = cur->next;
            if (cur) cout << " -> ";
        }
        cout << "\n";
    }
}

void Graph::printAdjMatrix() const {
    using namespace std;
    cout << CYAN << "\n  Adjacency Matrix (Distance in km, 0 = no direct edge):\n" << RESET;
    cout << YELLOW << "  " << string(5 + numCities * 7, '-') << "\n" << RESET;

    cout << "       ";
    for (int j = 0; j < numCities; j++)
        cout << setw(6) << left << cities[j].name.substr(0, 5);
    cout << "\n";

    for (int i = 0; i < numCities; i++) {
        cout << GREEN << setw(12) << left << cities[i].name.substr(0, 11) << RESET;
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

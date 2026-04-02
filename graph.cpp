#include "graph.h"

Graph::Graph() : V(0) {}

// ── Node management ────────────────────────────────────────────────────────────

void Graph::addCity(const string& name) {
    if (cityIndex.find(name) == cityIndex.end()) {
        cityIndex[name] = V++;
        cityNames.push_back(name);
        adjList.push_back({});
    }
}

bool Graph::cityExists(const string& name) const {
    return cityIndex.find(name) != cityIndex.end();
}

int Graph::getIndex(const string& name) const {
    auto it = cityIndex.find(name);
    return (it != cityIndex.end()) ? it->second : -1;
}

string Graph::getName(int idx) const {
    if (idx >= 0 && idx < V) return cityNames[idx];
    return "Unknown";
}

int Graph::cityCount() const { return V; }

const vector<string>& Graph::getCityNames() const { return cityNames; }

const vector<vector<Edge>>& Graph::getAdjList() const { return adjList; }

// ── Edge management ────────────────────────────────────────────────────────────

void Graph::addRoute(const string& from, const string& to, int dist, int cost) {
    addCity(from);
    addCity(to);
    int u = cityIndex[from];
    int v = cityIndex[to];
    adjList[u].push_back({v, dist, cost});
}

void Graph::addBidirectionalRoute(const string& from, const string& to,
                                   int dist, int cost) {
    addRoute(from, to, dist, cost);
    addRoute(to, from, dist, cost);
}

// ── Display ────────────────────────────────────────────────────────────────────

void Graph::printCities() const {
    cout << "\nCities in network (" << V << " total):\n";
    for (int i = 0; i < V; i++)
        cout << "  [" << i << "] " << cityNames[i] << "\n";
}

void Graph::printGraph() const {
    cout << "\n" << string(60, '=') << "\n";
    cout << "  CITY NETWORK — ALL ROUTES\n";
    cout << string(60, '=') << "\n";
    cout << left
         << setw(15) << "From"
         << setw(15) << "To"
         << setw(16) << "Distance (km)"
         << "Cost (INR)\n";
    cout << string(60, '-') << "\n";

    for (int u = 0; u < V; u++) {
        for (const Edge& e : adjList[u]) {
            cout << setw(15) << cityNames[u]
                 << setw(15) << cityNames[e.dest]
                 << setw(16) << e.distance
                 << e.cost << "\n";
        }
    }
}

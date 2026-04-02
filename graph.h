#ifndef GRAPH_H
#define GRAPH_H

#include <bits/stdc++.h>
using namespace std;


struct Edge {
    int    dest;      // dest city index
    int    distance;  // km
    int    cost;      // rupee
};

// ─── Graph: adj ─────────────────────────────
class Graph {
private:
    int                    V;          // number of cities
    vector<string>         cityNames;  // index -> name
    map<string, int>       cityIndex;  // name  -> index
    vector<vector<Edge>>   adjList;

public:
    Graph();

    // ── Node mana-- function
    void addCity(const string& name);
    bool cityExists(const string& name) const;
    int  getIndex(const string& name)   const;
    string getName(int idx)             const;
    int  cityCount()                    const;
    const vector<string>& getCityNames()       const;
    const vector<vector<Edge>>& getAdjList()   const;

    // ── Edge mana--fuction 
    void addRoute(const string& from, const string& to, int dist, int cost);
    void addBidirectionalRoute(const string& from, const string& to, int dist, int cost);

    // ── Display function
    void printGraph() const;
    void printCities() const;
};

#endif

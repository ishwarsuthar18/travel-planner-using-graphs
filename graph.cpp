#include "graph.h"
#include "ui.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>

// ─────────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────────
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

// ─────────────────────────────────────────────
//  Add a city
// ─────────────────────────────────────────────
void Graph::addCity(int id, const std::string& name,
                    double lat, double lng, bool airport) {
    cities[numCities].id         = id;
    cities[numCities].name       = name;
    cities[numCities].lat        = lat;
    cities[numCities].lng        = lng;
    cities[numCities].hasAirport = airport;
    numCities++;
}

// ─────────────────────────────────────────────
//  Add an edge (directed; undirected = both ways)
// ─────────────────────────────────────────────
void Graph::addEdge(int from, int to, double dist, double time, double cost,
                    TravelMode mode, bool undirected) {
    // Adjacency list
    Edge e1 = {to, dist, time, cost, mode};
    AdjNode* node1 = new AdjNode(e1);
    node1->next   = adjList[from];
    adjList[from] = node1;

    // Adjacency matrix (distance)
    adjMatrix[from][to] = dist;
    hasEdge[from][to]   = true;

    if (undirected) {
        Edge e2 = {from, dist, time, cost, mode};
        AdjNode* node2 = new AdjNode(e2);
        node2->next  = adjList[to];
        adjList[to]  = node2;

        adjMatrix[to][from] = dist;
        hasEdge[to][from]   = true;
    }
}

// ─────────────────────────────────────────────
//  Get all edges from a city
// ─────────────────────────────────────────────
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
//  Get edges filtered by travel mode
// ─────────────────────────────────────────────
std::vector<Edge> Graph::getEdgesByMode(int from, TravelMode mode) const {
    std::vector<Edge> result;
    AdjNode* cur = adjList[from];
    while (cur) {
        if (cur->edge.mode == mode)
            result.push_back(cur->edge);
        cur = cur->next;
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════════════
//  70-CITY INDIAN NETWORK
//
//  Cities are indexed 0–69.
//  ★ = has a commercial airport (22 cities with airports)
//
//  Edge data is realistic:
//    Road  : ~50 km/h average speed, ₹2-3/km
//    Rail  : ~60 km/h average speed, ₹1.5-2.5/km
//    Air   : ~800 km/h, air distance (~straight-line×1.05),
//            cost = base fare + distance component (significantly higher)
// ═══════════════════════════════════════════════════════════════════════
void Graph::loadDefaultNetwork() {

    // ── CITIES (id, name, lat, lng, hasAirport) ────────────────────────
    //  North India
    addCity( 0, "Delhi",       28.61,  77.21, true);   // ★
    addCity( 1, "Jaipur",      26.91,  75.79, true);   // ★
    addCity( 2, "Agra",        27.18,  78.01, false);
    addCity( 3, "Lucknow",     26.85,  80.95, true);   // ★
    addCity( 4, "Kanpur",      26.45,  80.33, false);
    addCity( 5, "Varanasi",    25.32,  83.01, true);   // ★
    addCity( 6, "Allahabad",   25.44,  81.84, false);
    addCity( 7, "Mathura",     27.49,  77.67, false);
    addCity( 8, "Amritsar",    31.63,  74.87, true);   // ★
    addCity( 9, "Chandigarh",  30.73,  76.78, true);   // ★
    addCity(10, "Ludhiana",    30.90,  75.86, false);
    addCity(11, "Dehradun",    30.32,  78.03, true);   // ★
    addCity(12, "Meerut",      28.98,  77.71, false);

    //  East India
    addCity(13, "Kolkata",     22.57,  88.36, true);   // ★
    addCity(14, "Patna",       25.59,  85.13, true);   // ★
    addCity(15, "Bhubaneswar", 20.30,  85.83, true);   // ★
    addCity(16, "Guwahati",    26.18,  91.74, true);   // ★
    addCity(17, "Ranchi",      23.36,  85.33, true);   // ★
    addCity(18, "Dhanbad",     23.79,  86.45, false);
    addCity(19, "Siliguri",    26.72,  88.43, false);

    //  West India
    addCity(20, "Mumbai",      19.07,  72.87, true);   // ★
    addCity(21, "Pune",        18.52,  73.86, true);   // ★
    addCity(22, "Ahmedabad",   23.02,  72.57, true);   // ★
    addCity(23, "Surat",       21.17,  72.83, false);
    addCity(24, "Vadodara",    22.30,  73.19, false);
    addCity(25, "Rajkot",      22.30,  70.79, true);   // ★
    addCity(26, "Nashik",      20.00,  73.79, false);
    addCity(27, "Aurangabad",  19.88,  75.34, true);   // ★
    addCity(28, "Solapur",     17.68,  75.91, false);

    //  Central India
    addCity(29, "Bhopal",      23.25,  77.40, true);   // ★
    addCity(30, "Indore",      22.72,  75.86, true);   // ★
    addCity(31, "Nagpur",      21.14,  79.08, true);   // ★
    addCity(32, "Jabalpur",    23.18,  79.95, false);
    addCity(33, "Gwalior",     26.22,  78.19, true);   // ★
    addCity(34, "Raipur",      21.25,  81.63, true);   // ★

    //  South India
    addCity(35, "Hyderabad",   17.38,  78.47, true);   // ★
    addCity(36, "Bangalore",   12.97,  77.59, true);   // ★
    addCity(37, "Chennai",     13.08,  80.27, true);   // ★
    addCity(38, "Coimbatore",  11.01,  76.97, true);   // ★
    addCity(39, "Kochi",        9.93,  76.26, true);   // ★
    addCity(40, "Thiruvanant",  8.52,  76.94, true);   // ★  (Thiruvanantapuram)
    addCity(41, "Visakhapatnam",17.69, 83.22, true);   // ★
    addCity(42, "Vijayawada",  16.51,  80.62, false);
    addCity(43, "Madurai",      9.93,  78.12, true);   // ★
    addCity(44, "Mysuru",      12.30,  76.65, false);
    addCity(45, "Mangaluru",   12.87,  74.84, true);   // ★
    addCity(46, "Tirupati",    13.63,  79.42, false);
    addCity(47, "Hubli",       15.36,  75.12, false);

    //  Additional cities (spread across India)
    addCity(48, "Jodhpur",     26.29,  73.02, true);   // ★
    addCity(49, "Udaipur",     24.58,  73.69, true);   // ★
    addCity(50, "Kota",        25.18,  75.85, false);
    addCity(51, "Ajmer",       26.45,  74.64, false);
    addCity(52, "Bikaner",     28.02,  73.31, false);
    addCity(53, "Jalandhar",   31.33,  75.57, false);
    addCity(54, "Shimla",      31.10,  77.17, false);
    addCity(55, "Haridwar",    29.94,  78.16, false);
    addCity(56, "Agartala",    23.83,  91.28, true);   // ★
    addCity(57, "Imphal",      24.81,  93.94, true);   // ★
    addCity(58, "Shillong",    25.57,  91.88, false);
    addCity(59, "Dimapur",     25.91,  93.72, false);
    addCity(60, "Brahmapur",   19.31,  84.80, false);
    addCity(61, "Cuttack",     20.46,  85.88, false);
    addCity(62, "Sambalpur",   21.47,  83.98, false);
    addCity(63, "Jamshedpur",  22.80,  86.18, false);
    addCity(64, "Gaya",        24.79,  85.01, false);
    addCity(65, "Muzaffarpur", 26.12,  85.39, false);
    addCity(66, "Bareilly",    28.36,  79.42, false);
    addCity(67, "Moradabad",   28.84,  78.78, false);
    addCity(68, "Aligarh",     27.88,  78.08, false);
    addCity(69, "Gorakhpur",   26.76,  83.37, false);

    // ════════════════════════════════════════════════════════════════
    //  EDGES
    //  Format: addEdge(from, to, dist_km, time_min, cost_INR, mode)
    //
    //  Road  speed ~50 km/h; cost ₹2.5/km
    //  Rail  speed ~60 km/h; cost ₹2.0/km
    //  Air   speed ~800 km/h (direct flight path ≈ straight-line dist);
    //        cost = ₹3500 base + ₹5/km (realistic budget airline fare)
    // ════════════════════════════════════════════════════════════════

    // ── North India Road/Rail ──────────────────────────────────────
    addEdge( 0,  7,  55,  66,   140, ROAD);   // Delhi – Mathura
    addEdge( 7,  2,  55,  66,   140, ROAD);   // Mathura – Agra
    addEdge( 0,  2, 233, 240,   580, RAIL);   // Delhi – Agra (Rajdhani)
    addEdge( 0,  1, 281, 285,   700, ROAD);   // Delhi – Jaipur
    addEdge( 0,  1, 281, 280,   600, RAIL);   // Delhi – Jaipur (express)
    addEdge( 0, 12,  70,  84,   175, ROAD);   // Delhi – Meerut
    addEdge( 0,  9, 251, 270,   630, ROAD);   // Delhi – Chandigarh
    addEdge( 0,  3, 551, 480,  1100, RAIL);   // Delhi – Lucknow
    addEdge( 0, 29, 777, 660,  1600, RAIL);   // Delhi – Bhopal
    addEdge( 0, 33, 323, 300,   810, RAIL);   // Delhi – Gwalior
    addEdge( 0, 66, 252, 270,   630, RAIL);   // Delhi – Bareilly
    addEdge( 0, 68, 131, 157,   328, ROAD);   // Delhi – Aligarh
    addEdge( 9, 10,  99, 119,   248, ROAD);   // Chandigarh – Ludhiana
    addEdge( 9,  8, 229, 240,   573, ROAD);   // Chandigarh – Amritsar
    addEdge( 9, 54, 116, 139,   290, ROAD);   // Chandigarh – Shimla
    addEdge( 9, 11, 304, 330,   760, ROAD);   // Chandigarh – Dehradun
    addEdge(10, 53,  52,  62,   130, ROAD);   // Ludhiana – Jalandhar
    addEdge(53,  8,  79,  95,   198, ROAD);   // Jalandhar – Amritsar
    addEdge(11, 55,  54,  65,   135, ROAD);   // Dehradun – Haridwar
    addEdge(55, 12, 185, 222,   463, ROAD);   // Haridwar – Meerut
    addEdge(12,  0,  70,  84,   175, ROAD);   // Meerut – Delhi (already added reverse)
    addEdge( 1, 51,  135,162,   338, ROAD);   // Jaipur – Ajmer
    addEdge( 1, 50,  254,305,   635, ROAD);   // Jaipur – Kota
    addEdge( 1, 48,  335,402,   838, ROAD);   // Jaipur – Jodhpur
    addEdge(51, 49,  283,340,   708, ROAD);   // Ajmer – Udaipur
    addEdge(48, 49,  261,313,   653, ROAD);   // Jodhpur – Udaipur
    addEdge(48, 52,  252,302,   630, ROAD);   // Jodhpur – Bikaner
    addEdge( 0, 52,  458,550,  1145, RAIL);   // Delhi – Bikaner
    addEdge( 2,  6,  149,179,   373, ROAD);   // Agra – Allahabad
    addEdge( 6,  4,   80, 96,   200, ROAD);   // Allahabad – Kanpur
    addEdge( 4,  3,   80, 96,   200, ROAD);   // Kanpur – Lucknow
    addEdge( 6,  5,  128,154,   320, ROAD);   // Allahabad – Varanasi
    addEdge( 5, 69,  272,326,   680, ROAD);   // Varanasi – Gorakhpur
    addEdge( 3, 66,  252,302,   630, RAIL);   // Lucknow – Bareilly
    addEdge(66, 67,   52, 62,   130, ROAD);   // Bareilly – Moradabad
    addEdge(67, 12,  136,163,   340, ROAD);   // Moradabad – Meerut
    addEdge( 3, 69,  269,323,   673, ROAD);   // Lucknow – Gorakhpur
    addEdge(69, 14,  320,384,   800, RAIL);   // Gorakhpur – Patna

    // ── East India ────────────────────────────────────────────────
    addEdge(14, 64,  103,124,   258, ROAD);   // Patna – Gaya
    addEdge(14, 65,   75, 90,   188, ROAD);   // Patna – Muzaffarpur
    addEdge(14, 13,  613,540,  1226, RAIL);   // Patna – Kolkata
    addEdge(13, 18,  271,295,   678, RAIL);   // Kolkata – Dhanbad
    addEdge(18, 63,   57, 68,   143, ROAD);   // Dhanbad – Jamshedpur
    addEdge(13, 63,  264,317,   660, ROAD);   // Kolkata – Jamshedpur
    addEdge(63, 17,  131,157,   328, ROAD);   // Jamshedpur – Ranchi
    addEdge(17, 14,  328,394,   820, RAIL);   // Ranchi – Patna
    addEdge(17, 34,  367,440,   918, RAIL);   // Ranchi – Raipur
    addEdge(13, 61,  487,430,   975, RAIL);   // Kolkata – Cuttack
    addEdge(61, 15,   25, 30,    63, ROAD);   // Cuttack – Bhubaneswar
    addEdge(15, 60,  161,193,   403, ROAD);   // Bhubaneswar – Brahmapur
    addEdge(60, 41,  444,533, 1110, ROAD);   // Brahmapur – Visakhapatnam
    addEdge(62, 17,  220,264,   550, ROAD);   // Sambalpur – Ranchi
    addEdge(62, 15,  329,395,   823, RAIL);   // Sambalpur – Bhubaneswar
    addEdge(13, 19,  625,540,  1250, RAIL);   // Kolkata – Siliguri
    addEdge(19, 16,  220,264,   550, ROAD);   // Siliguri – Guwahati
    addEdge(16, 58,   98,118,   245, ROAD);   // Guwahati – Shillong
    addEdge(16, 56,  594,713, 1485, RAIL);   // Guwahati – Agartala
    addEdge(16, 59,  330,396,   825, ROAD);   // Guwahati – Dimapur
    addEdge(59, 57,  145,174,   363, ROAD);   // Dimapur – Imphal

    // ── West India ────────────────────────────────────────────────
    addEdge(20, 21,  149,179,   373, ROAD);   // Mumbai – Pune
    addEdge(20, 21,  149,150,   300, RAIL);   // Mumbai – Pune (express)
    addEdge(20, 26,  172,206,   430, ROAD);   // Mumbai – Nashik
    addEdge(20, 23,  271,325,   678, ROAD);   // Mumbai – Surat
    addEdge(23, 24,  128,154,   320, ROAD);   // Surat – Vadodara
    addEdge(24, 22,  113,136,   283, ROAD);   // Vadodara – Ahmedabad
    addEdge(22, 25,  217,260,   543, ROAD);   // Ahmedabad – Rajkot
    addEdge(22, 30,  226,271,   565, ROAD);   // Ahmedabad – Indore
    addEdge(20, 27,  338,406,   845, ROAD);   // Mumbai – Aurangabad
    addEdge(27, 28,  252,302,   630, ROAD);   // Aurangabad – Solapur
    addEdge(28, 21,  248,298,   620, ROAD);   // Solapur – Pune
    addEdge(28, 35,  338,406,   845, ROAD);   // Solapur – Hyderabad
    addEdge(21, 47,  436,523, 1090, ROAD);   // Pune – Hubli
    addEdge(47, 36,  413,496, 1033, ROAD);   // Hubli – Bangalore
    addEdge(47, 45,  222,266,   555, ROAD);   // Hubli – Mangaluru

    // ── Central India ─────────────────────────────────────────────
    addEdge(29, 30, 191,229,   478, ROAD);   // Bhopal – Indore
    addEdge(29, 31, 355,426,   888, ROAD);   // Bhopal – Nagpur
    addEdge(29, 32, 328,394,   820, ROAD);   // Bhopal – Jabalpur
    addEdge(29, 33, 423,508, 1058, ROAD);   // Bhopal – Gwalior
    addEdge(30, 31, 348,418,   870, ROAD);   // Indore – Nagpur
    addEdge(31, 34, 292,350,   730, ROAD);   // Nagpur – Raipur
    addEdge(31, 35, 503,604, 1258, ROAD);   // Nagpur – Hyderabad
    addEdge(31, 32, 311,373,   778, ROAD);   // Nagpur – Jabalpur
    addEdge(32, 14, 679,581, 1358, RAIL);   // Jabalpur – Patna
    addEdge(33,  2, 122,146,   305, ROAD);   // Gwalior – Agra
    addEdge(33,  4, 479,575, 1198, RAIL);   // Gwalior – Kanpur
    addEdge(34, 41, 739,633, 1478, RAIL);   // Raipur – Visakhapatnam
    addEdge(34, 31, 292,350,   730, ROAD);   // Raipur – Nagpur (already added)

    // ── South India ───────────────────────────────────────────────
    addEdge(35, 42, 279,335,   698, ROAD);   // Hyderabad – Vijayawada
    addEdge(35, 36, 575,490, 1150, RAIL);   // Hyderabad – Bangalore
    addEdge(35, 41, 620,530, 1240, RAIL);   // Hyderabad – Visakhapatnam
    addEdge(35, 46, 573,688, 1433, ROAD);   // Hyderabad – Tirupati
    addEdge(42, 41, 353,424,   883, ROAD);   // Vijayawada – Visakhapatnam
    addEdge(42, 37, 427,512, 1068, ROAD);   // Vijayawada – Chennai
    addEdge(37, 36, 346,360, 1100, ROAD);   // Chennai – Bangalore
    addEdge(37, 36, 346,335,   865, RAIL);   // Chennai – Bangalore (express)
    addEdge(37, 46, 148,178,   370, ROAD);   // Chennai – Tirupati
    addEdge(37, 43, 462,554, 1155, ROAD);   // Chennai – Madurai
    addEdge(36, 44, 153,184,   383, ROAD);   // Bangalore – Mysuru
    addEdge(36, 38, 355,426,   888, ROAD);   // Bangalore – Coimbatore
    addEdge(36, 47, 413,496, 1033, ROAD);   // Bangalore – Hubli
    addEdge(36, 45, 356,427,   890, ROAD);   // Bangalore – Mangaluru
    addEdge(38, 43, 217,260,   543, ROAD);   // Coimbatore – Madurai
    addEdge(38, 39, 188,226,   470, ROAD);   // Coimbatore – Kochi
    addEdge(39, 40, 220,264,   550, ROAD);   // Kochi – Thiruvanantapuram
    addEdge(40, 43, 307,368,   768, ROAD);   // Thiruvanantapuram – Madurai
    addEdge(44, 39, 483,580, 1208, ROAD);   // Mysuru – Kochi
    addEdge(45, 39, 192,230,   480, ROAD);   // Mangaluru – Kochi

    // ── Cross-region Rail links ────────────────────────────────────
    addEdge( 0, 13,1472,1200, 2944, RAIL);  // Delhi – Kolkata (Rajdhani)
    addEdge( 0, 20,1415, 960, 2830, RAIL);  // Delhi – Mumbai (Rajdhani)
    addEdge( 0, 35,1579,1080, 3158, RAIL);  // Delhi – Hyderabad (Express)
    addEdge(20, 37,1333, 900, 2666, RAIL);  // Mumbai – Chennai (Express)
    addEdge(20, 36,1153, 780, 2306, RAIL);  // Mumbai – Bangalore
    addEdge(13, 37,1659,1100, 3318, RAIL);  // Kolkata – Chennai (Coromandel)
    addEdge( 5, 13, 802, 680, 1604, RAIL);  // Varanasi – Kolkata
    addEdge( 3,  5, 322, 300,   805, RAIL); // Lucknow – Varanasi

    // ════════════════════════════════════════════════════════════════
    //  AIR ROUTES (22 airports)
    //  Air distance ≈ straight-line (great-circle) distance
    //  Time = (distance / 800 km/h * 60) + 90 min (boarding/taxi)
    //  Cost = ₹3500 base + ₹5 per km (budget airline)
    //
    //  Airports: Delhi(0), Jaipur(1), Lucknow(3), Varanasi(5),
    //  Amritsar(8), Chandigarh(9), Dehradun(11), Kolkata(13),
    //  Patna(14), Bhubaneswar(15), Guwahati(16), Ranchi(17),
    //  Mumbai(20), Pune(21), Ahmedabad(22), Rajkot(25), Aurangabad(27),
    //  Bhopal(29), Indore(30), Nagpur(31), Gwalior(33), Raipur(34),
    //  Hyderabad(35), Bangalore(36), Chennai(37), Coimbatore(38),
    //  Kochi(39), Thiruvanantapuram(40), Visakhapatnam(41), Madurai(43),
    //  Mangaluru(45), Jodhpur(48), Udaipur(49), Agartala(56), Imphal(57)
    // ════════════════════════════════════════════════════════════════

    // Helper lambda-style: Air dist ≈ straight-line, time includes 90 min ground
    // cost = 3500 + 5*dist
    // We pre-calculated these values for accuracy.

    // Delhi (0) air routes
    addEdge( 0,  1,  268, 110,  4840, AIR);  // Delhi – Jaipur
    addEdge( 0,  3,  446, 123,  5730, AIR);  // Delhi – Lucknow
    addEdge( 0,  5,  672, 140,  6860, AIR);  // Delhi – Varanasi
    addEdge( 0,  8,  447, 123,  5735, AIR);  // Delhi – Amritsar
    addEdge( 0,  9,  246, 108,  4730, AIR);  // Delhi – Chandigarh
    addEdge( 0, 11,  286, 111,  4930, AIR);  // Delhi – Dehradun
    addEdge( 0, 13, 1298, 187, 10990, AIR);  // Delhi – Kolkata
    addEdge( 0, 14,  991, 164,  8455, AIR);  // Delhi – Patna
    addEdge( 0, 20, 1148, 176,  9240, AIR);  // Delhi – Mumbai
    addEdge( 0, 22,  782, 147,  7410, AIR);  // Delhi – Ahmedabad
    addEdge( 0, 29,  698, 142,  7490, AIR);  // Delhi – Bhopal
    addEdge( 0, 30,  786, 149,  7430, AIR);  // Delhi – Indore
    addEdge( 0, 33,  321, 114,  5105, AIR);  // Delhi – Gwalior
    addEdge( 0, 35, 1253, 183,  9765, AIR);  // Delhi – Hyderabad
    addEdge( 0, 36, 1741, 220, 12205, AIR);  // Delhi – Bangalore
    addEdge( 0, 37, 1752, 220, 12260, AIR);  // Delhi – Chennai
    addEdge( 0, 39, 2082, 246, 13910, AIR);  // Delhi – Kochi
    addEdge( 0, 48,  587, 134,  6435, AIR);  // Delhi – Jodhpur

    // Mumbai (20) air routes
    addEdge(20,  0, 1148, 176,  9240, AIR);  // already added above (undirected handles it, but AIR we add both explicitly for clarity — actually undirected=true handles it)
    addEdge(20,  1,  939, 160,  8195, AIR);  // Mumbai – Jaipur
    addEdge(20, 13, 1671, 214, 11855, AIR);  // Mumbai – Kolkata
    addEdge(20, 35,  620, 136,  6600, AIR);  // Mumbai – Hyderabad
    addEdge(20, 36, 842,  153,  7710, AIR);  // Mumbai – Bangalore
    addEdge(20, 37, 1040, 168,  8700, AIR);  // Mumbai – Chennai
    addEdge(20, 22,  426, 122,  5630, AIR);  // Mumbai – Ahmedabad
    addEdge(20, 39, 1196, 180,  9480, AIR);  // Mumbai – Kochi
    addEdge(20, 40, 1346, 191, 10230, AIR);  // Mumbai – Thiruvanantapuram
    addEdge(20, 29,  634, 137,  6670, AIR);  // Mumbai – Bhopal
    addEdge(20, 31,  841, 153,  7705, AIR);  // Mumbai – Nagpur
    addEdge(20, 48, 620,  136,  6600, AIR);  // Mumbai – Jodhpur
    addEdge(20, 27,  207, 105,  4535, AIR);  // Mumbai – Aurangabad

    // Kolkata (13) air routes
    addEdge(13, 16,  564, 132,  6320, AIR);  // Kolkata – Guwahati
    addEdge(13, 17,  372, 117,  5360, AIR);  // Kolkata – Ranchi
    addEdge(13, 35, 1180, 178,  9400, AIR);  // Kolkata – Hyderabad
    addEdge(13, 36, 1560, 207, 11300, AIR);  // Kolkata – Bangalore
    addEdge(13, 37, 1340, 190, 10200, AIR);  // Kolkata – Chennai
    addEdge(13, 56,  503, 127,  6015, AIR);  // Kolkata – Agartala
    addEdge(13, 57,  980, 163,  8400, AIR);  // Kolkata – Imphal
    addEdge(13, 15,  400, 120,  5500, AIR);  // Kolkata – Bhubaneswar
    addEdge(13, 41,  760, 147,  7300, AIR);  // Kolkata – Visakhapatnam

    // Hyderabad (35) air routes
    addEdge(35, 37,  631, 137,  6655, AIR);  // Hyderabad – Chennai
    addEdge(35, 36,  499, 127,  5995, AIR);  // Hyderabad – Bangalore
    addEdge(35, 39, 1074, 170,  8870, AIR);  // Hyderabad – Kochi
    addEdge(35, 41,  468, 125,  5840, AIR);  // Hyderabad – Visakhapatnam
    addEdge(35, 29,  629, 137,  6645, AIR);  // Hyderabad – Bhopal
    addEdge(35, 31,  444, 123,  5720, AIR);  // Hyderabad – Nagpur
    addEdge(35, 34,  479, 125,  5895, AIR);  // Hyderabad – Raipur

    // Bangalore (36) air routes
    addEdge(36, 37,  290, 112,  4950, AIR);  // Bangalore – Chennai
    addEdge(36, 39,  365, 117,  5325, AIR);  // Bangalore – Kochi
    addEdge(36, 40,  466, 125,  5830, AIR);  // Bangalore – Thiruvanantapuram
    addEdge(36, 38,  303, 113,  5015, AIR);  // Bangalore – Coimbatore
    addEdge(36, 43,  449, 124,  5745, AIR);  // Bangalore – Madurai
    addEdge(36, 45,  286, 111,  4930, AIR);  // Bangalore – Mangaluru
    addEdge(36, 41,  803, 150,  7515, AIR);  // Bangalore – Visakhapatnam
    addEdge(36, 22, 1009, 166,  8545, AIR);  // Bangalore – Ahmedabad

    // Chennai (37) air routes
    addEdge(37, 38,  500, 128,  6000, AIR);  // Chennai – Coimbatore
    addEdge(37, 39,  627, 136,  6635, AIR);  // Chennai – Kochi
    addEdge(37, 40,  717, 143,  7085, AIR);  // Chennai – Thiruvanantapuram
    addEdge(37, 43,  438, 123,  5690, AIR);  // Chennai – Madurai
    addEdge(37, 45,  530, 129,  6150, AIR);  // Chennai – Mangaluru
    addEdge(37, 41,  711, 142,  7055, AIR);  // Chennai – Visakhapatnam

    // Other airport pairs
    addEdge(22,  1,  249, 108,  4745, AIR);  // Ahmedabad – Jaipur
    addEdge(22, 29,  523, 129,  6115, AIR);  // Ahmedabad – Bhopal
    addEdge(22, 30,  261, 109,  4805, AIR);  // Ahmedabad – Indore
    addEdge(22, 48,  492, 127,  5960, AIR);  // Ahmedabad – Jodhpur
    addEdge(22, 49,  375, 117,  5375, AIR);  // Ahmedabad – Udaipur
    addEdge(22, 25,  200, 105,  4500, AIR);  // Ahmedabad – Rajkot
    addEdge(25, 20,  552, 131,  6260, AIR);  // Rajkot – Mumbai
    addEdge(48, 22,  492, 127,  5960, AIR);  // already covered above
    addEdge(49, 20,  683, 140,  6915, AIR);  // Udaipur – Mumbai
    addEdge(29, 31,  305, 113,  5025, AIR);  // Bhopal – Nagpur
    addEdge(29, 30,  177, 103,  4385, AIR);  // Bhopal – Indore
    addEdge(31, 34,  265, 110,  4825, AIR);  // Nagpur – Raipur
    addEdge(31, 36,  902, 158,  8010, AIR);  // Nagpur – Bangalore
    addEdge(33,  0,  321, 114,  5105, AIR);  // Gwalior – Delhi (already covered)
    addEdge(16, 57,  343, 116,  5215, AIR);  // Guwahati – Imphal
    addEdge(56, 57,  449, 124,  5745, AIR);  // Agartala – Imphal
    addEdge(14,  0,  991, 164,  8455, AIR);  // Patna – Delhi (already covered)
    addEdge(14, 13,  487, 126,  5935, AIR);  // Patna – Kolkata
    addEdge(17, 29,  590, 134,  6450, AIR);  // Ranchi – Bhopal
    addEdge(15, 35,  791, 149,  7455, AIR);  // Bhubaneswar – Hyderabad
    addEdge(21, 36,  726, 144,  7130, AIR);  // Pune – Bangalore
    addEdge(21, 35,  466, 125,  5830, AIR);  // Pune – Hyderabad
    addEdge(21, 37,  889, 156,  7945, AIR);  // Pune – Chennai
    addEdge(38, 40,  195, 105,  4475, AIR);  // Coimbatore – Thiruvanantapuram
    addEdge(43, 40,  266, 110,  4830, AIR);  // Madurai – Thiruvanantapuram
    addEdge(39, 36,  365, 117,  5325, AIR);  // Kochi – Bangalore (already covered)
    addEdge( 9,  0,  246, 108,  4730, AIR);  // Chandigarh – Delhi (already covered)
    addEdge( 8,  0,  447, 123,  5735, AIR);  // Amritsar – Delhi (already covered)
    addEdge(11,  0,  286, 111,  4930, AIR);  // Dehradun – Delhi (already covered)
    addEdge( 5,  0,  672, 140,  6860, AIR);  // Varanasi – Delhi (already covered)
    addEdge( 5, 13,  760, 147,  7300, AIR);  // Varanasi – Kolkata
    addEdge( 3,  0,  446, 123,  5730, AIR);  // Lucknow – Delhi (already covered)
    addEdge( 3, 20, 1123, 174,  9115, AIR);  // Lucknow – Mumbai
    addEdge(34, 13,  804, 150,  7520, AIR);  // Raipur – Kolkata
    addEdge(27, 36, 864,  154,  7820, AIR);  // Aurangabad – Bangalore
}

// ─────────────────────────────────────────────
//  Print adjacency list
// ─────────────────────────────────────────────
void Graph::printAdjList() const {
    using namespace std;
    cout << CYAN << "\n  Adjacency List Representation:\n" << RESET;
    cout << YELLOW << "  " << string(70, '-') << "\n" << RESET;
    for (int i = 0; i < numCities; i++) {
        cout << GREEN << "  " << setw(16) << left << cities[i].name << RESET << " → ";
        AdjNode* cur = adjList[i];
        if (!cur) { cout << "(no outgoing edges)\n"; continue; }
        int count = 0;
        while (cur) {
            if (count > 0 && count % 3 == 0) cout << "\n                      ";
            string modeStr = (cur->edge.mode == ROAD) ? "Rd" :
                             (cur->edge.mode == RAIL) ? "Rl" : "Ai";
            cout << CYAN << cities[cur->edge.to].name << RESET
                 << "[" << modeStr << " "
                 << (int)cur->edge.distance << "km/₹"
                 << (int)cur->edge.cost << "]";
            cur = cur->next;
            if (cur) cout << " → ";
            count++;
        }
        cout << "\n";
    }
}

// ─────────────────────────────────────────────
//  Print adjacency matrix (distance, first 20 cities for readability)
// ─────────────────────────────────────────────
void Graph::printAdjMatrix() const {
    using namespace std;
    int show = (numCities > 20) ? 20 : numCities;
    cout << CYAN << "\n  Adjacency Matrix (Distance km, first " << show << " cities):\n" << RESET;
    cout << YELLOW << "  " << string(5 + show * 7, '-') << "\n" << RESET;

    cout << "         ";
    for (int j = 0; j < show; j++)
        cout << setw(6) << left << cities[j].name.substr(0, 5);
    cout << "\n";

    for (int i = 0; i < show; i++) {
        cout << GREEN << setw(14) << left << cities[i].name.substr(0, 13) << RESET;
        for (int j = 0; j < show; j++) {
            if (i == j)
                cout << YELLOW << setw(6) << "  0  " << RESET;
            else if (hasEdge[i][j])
                cout << CYAN << setw(6) << (int)adjMatrix[i][j] << RESET;
            else
                cout << setw(6) << "  -  ";
        }
        cout << "\n";
    }
    if (numCities > 20)
        cout << YELLOW << "\n  (Showing first 20 of " << numCities << " cities)\n" << RESET;
}

// ─────────────────────────────────────────────
//  Print graph statistics
// ─────────────────────────────────────────────
void Graph::printStats() const {
    using namespace std;
    int totalEdges = 0, roadEdges = 0, railEdges = 0, airEdges = 0;
    int airportCount = 0;
    for (int i = 0; i < numCities; i++) {
        if (cities[i].hasAirport) airportCount++;
        AdjNode* cur = adjList[i];
        while (cur) {
            totalEdges++;
            if (cur->edge.mode == ROAD) roadEdges++;
            else if (cur->edge.mode == RAIL) railEdges++;
            else airEdges++;
            cur = cur->next;
        }
    }
    int undirected = totalEdges / 2;
    cout << GREEN << "\n  Graph Statistics:\n" << RESET;
    cout << "  Total Cities   : " << CYAN << numCities    << RESET << "\n";
    cout << "  Airports       : " << CYAN << airportCount << RESET << "\n";
    cout << "  Total Edges    : " << CYAN << undirected   << RESET << " (undirected)\n";
    cout << "    Road edges   : " << CYAN << roadEdges/2  << RESET << "\n";
    cout << "    Rail edges   : " << CYAN << railEdges/2  << RESET << "\n";
    cout << "    Air routes   : " << CYAN << airEdges/2   << RESET << "\n";
    cout << "  Avg Degree     : " << CYAN
         << fixed << setprecision(2) << (double)totalEdges / numCities << RESET << "\n";
}

// ─────────────────────────────────────────────
//  Print list of airports
// ─────────────────────────────────────────────
void Graph::printAirports() const {
    using namespace std;
    cout << CYAN << "\n  Airports in the Network:\n" << RESET;
    cout << YELLOW << "  " << string(45, '-') << "\n" << RESET;
    int n = 0;
    for (int i = 0; i < numCities; i++) {
        if (cities[i].hasAirport) {
            cout << "  " << GREEN << setw(3) << ++n << ". " << RESET
                 << setw(20) << left << cities[i].name
                 << " (Idx " << setw(2) << i << ")";
            if (n % 2 == 0) cout << "\n";
        }
    }
    if (n % 2 != 0) cout << "\n";
    cout << YELLOW << "  " << string(45, '-') << "\n" << RESET;
    cout << "  Total airports: " << CYAN << n << RESET << "\n";
}

// ─────────────────────────────────────────────
//  Utility: find city index by name
// ─────────────────────────────────────────────
int Graph::getCityIndex(const std::string& name) const {
    for (int i = 0; i < numCities; i++)
        if (cities[i].name == name) return i;
    return -1;
}

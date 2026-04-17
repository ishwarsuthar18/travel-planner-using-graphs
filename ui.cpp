#include "ui.h"
#include <iostream>
#include <iomanip>
#include <string>

void clearScreen() { std::cout << "\033[2J\033[H"; }

void printBanner() {
    std::cout << CYAN << R"(╔════════════════════════════════════════════════════════════════════════════╗
║                                                                            ║
║ )" << YELLOW << R"( ████████╗██████╗  █████╗ ██╗   ██╗███████╗██╗         )" << CYAN << R"(       ║
║ )" << YELLOW << R"( ╚══██╔══╝██╔══██╗██╔══██╗██║   ██║██╔════╝██║         )" << CYAN << R"(       ║
║ )" << YELLOW << R"(    ██║   ██████╔╝███████║██║   ██║█████╗  ██║         )" << CYAN << R"(       ║
║ )" << YELLOW << R"(    ██║   ██╔══██╗██╔══██║╚██╗ ██╔╝██╔══╝  ██║         )" << CYAN << R"(       ║
║ )" << YELLOW << R"(    ██║   ██║  ██║██║  ██║ ╚████╔╝ ███████╗███████╗    )" << CYAN << R"(       ║
║ )" << YELLOW << R"(    ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚══════╝    )" << CYAN << R"(       ║
║                                                                            ║
║ )" << GREEN << R"(                        ✈ SMART TRAVEL PLANNER ✈         )" << CYAN << R"(      ║
║ )" << MAGENTA << R"(              Discover Routes • Compare Fares • Save Time )" << CYAN << R"(     ║
║ )" << BLUE << R"(              Railways  |   Flights  |   Highways    )" << CYAN << R"(     ║
║ )" << WHITE << R"(                  70 Indian Cities • AI Route Engine      )" << CYAN << R"(     ║
║                                                                            ║
╚════════════════════════════════════════════════════════════════════════════╝)" 
<< RESET;
}

void printMainMenu() {
    std::cout << CYAN << "\n  ╔══════════════════════════════════════════════════════╗\n";
    std::cout << "  ║" << YELLOW << BOLD << "                  ✦ MAIN MENU ✦                    " << RESET << CYAN << "║\n";
    std::cout << "  ╠══════════════════════════════════════════════════════╣\n";
    std::cout << "  ║  " << GREEN << "1." << RESET << "  Plan Smart Route (Best Mode)              " << CYAN << "║\n";
    std::cout << "  ║  " << GREEN << "2." << RESET << "  Multi-Stop Journey Planner                " << CYAN << "║\n";
    std::cout << "  ║  " << GREEN << "3." << RESET << "  Compare All Path Algorithms               " << CYAN << "║\n";
    std::cout << "  ║  " << GREEN << "4." << RESET << "  View Network Graph Structure              " << CYAN << "║\n";
    std::cout << "  ║  " << GREEN << "5." << RESET << "  Budget-Friendly Route Search              " << CYAN << "║\n";
    std::cout << "  ║  " << GREEN << "6." << RESET << "  All-Pairs Shortest Paths                  " << CYAN << "║\n";
    std::cout << "  ║  " << GREEN << "7." << RESET << "  City Information Center                   " << CYAN << "║\n";
    std::cout << "  ║  " << GREEN << "8." << RESET << "  View All Airports                         " << CYAN << "║\n";
    std::cout << "  ║  " << RED   << "9." << RESET << "  Exit Planner                             " << CYAN << "║\n";
    std::cout << "  ╚══════════════════════════════════════════════════════╝\n" << RESET;
    std::cout << YELLOW << "\n  ➤ Enter your choice: " << RESET;
}

void pause() {
    std::cout << YELLOW << "\n  Press ENTER to continue..." << RESET;
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void printCityList(int numCities, const std::string names[]) {
    std::cout << CYAN << "\n  Available Cities:\n" << RESET;
    std::cout << "  " << std::string(60, '-') << "\n";
    for (int i = 0; i < numCities; i++) {
        std::cout << "  " << GREEN << std::setw(3) << (i + 1) << ". " << RESET
                  << std::setw(18) << std::left << names[i];
        if ((i + 1) % 3 == 0) std::cout << "\n";
    }
    if (numCities % 3 != 0) std::cout << "\n";
    std::cout << "  " << std::string(60, '-') << "\n";
}

int selectCity(const std::string& prompt, int numCities, const std::string names[]) {
    int choice = -1;
    while (choice < 1 || choice > numCities) {
        std::cout << YELLOW << "  " << prompt << " (1-" << numCities << "): " << RESET;
        std::cin >> choice;
        if (std::cin.fail() || choice < 1 || choice > numCities) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << RED << "  Invalid choice. Try again.\n" << RESET;
            choice = -1;
        }
    }
    std::cout << GREEN << "  ✓ Selected: " << names[choice - 1] << "\n" << RESET;
    return choice - 1;   // 0-indexed
}

int selectCriteria() {
    std::cout << "\n  Optimize by:\n";
    std::cout << "  " << GREEN << "1." << RESET << " Shortest Distance (km)\n";
    std::cout << "  " << GREEN << "2." << RESET << " Fastest Time\n";
    std::cout << "  " << GREEN << "3." << RESET << " Cheapest Cost (₹)\n";
    std::cout << "  Choice (1-3): ";
    int c; std::cin >> c;
    if (c < 1 || c > 3) c = 1;
    return c - 1;   // maps to OptCriteria enum (0=DISTANCE, 1=TIME, 2=COST)
}

// Returns: 0=Road, 1=Rail, 2=Air, -1=Any (no filter)
int selectTravelMode() {
    std::cout << "\n  Travel Mode:\n";
    std::cout << "  " << GREEN << "1." << RESET << " Any (Road + Rail + Air)\n";
    std::cout << "  " << GREEN << "2." << RESET << " Road only\n";
    std::cout << "  " << GREEN << "3." << RESET << " Rail only\n";
    std::cout << "  " << GREEN << "4." << RESET << " Air only  "
              << YELLOW << "(both cities must have airports)" << RESET << "\n";
    std::cout << "  Choice (1-4): ";
    int c; std::cin >> c;
    if (c < 1 || c > 4) c = 1;
    switch (c) {
        case 2: return 0;   // ROAD
        case 3: return 1;   // RAIL
        case 4: return 2;   // AIR
        default: return -1; // ANY
    }
}

int selectAlgorithm() {
    std::cout << "\n  Choose Algorithm:\n";
    std::cout << "  " << GREEN << "1." << RESET << " Dijkstra         (optimal, weighted)\n";
    std::cout << "  " << GREEN << "2." << RESET << " BFS              (fewest hops)\n";
    std::cout << "  " << GREEN << "3." << RESET << " DFS              (explore deep)\n";
    std::cout << "  " << GREEN << "4." << RESET << " A*               (heuristic-guided)\n";
    std::cout << "  " << GREEN << "5." << RESET << " Bellman-Ford     (handles neg. weights)\n";
    std::cout << "  Choice (1-5): ";
    int c; std::cin >> c;
    if (c < 1 || c > 5) c = 1;
    return c;
}

void printAlgoExplanation(int algo) {
    std::cout << MAGENTA << "\n  ─── Algorithm Info ─────────────────────────────────────\n" << RESET;
    switch (algo) {
        case 1:
            std::cout << "  Dijkstra: Greedily picks the node with the smallest known\n"
                      << "  distance from a min-heap. Guarantees optimal path when all\n"
                      << "  edge weights are non-negative.\n"
                      << "  Complexity: O((V+E) log V)  |  Space: O(V)\n";
            break;
        case 2:
            std::cout << "  BFS: Explores cities level-by-level. Always finds the path\n"
                      << "  with the fewest intermediate stops (minimum hops).\n"
                      << "  Complexity: O(V+E)  |  Space: O(V)\n";
            break;
        case 3:
            std::cout << "  DFS: Uses a stack to dive deep into one branch before\n"
                      << "  backtracking. Finds a valid path but not necessarily optimal.\n"
                      << "  Complexity: O(V+E)  |  Space: O(V)\n";
            break;
        case 4:
            std::cout << "  A*: Combines the actual travel cost with a Haversine\n"
                      << "  straight-line distance heuristic to guide search toward\n"
                      << "  the goal more efficiently than plain Dijkstra.\n"
                      << "  Complexity: O((V+E) log V)  |  Space: O(V)\n";
            break;
        case 5:
            std::cout << "  Bellman-Ford: Relaxes all edges V-1 times. Works correctly\n"
                      << "  with negative edge weights and can detect negative cycles.\n"
                      << "  Complexity: O(V×E)  |  Space: O(V)\n";
            break;
    }
    std::cout << MAGENTA << "  ───────────────────────────────────────────────────────\n" << RESET;
}

void animateSearching(const std::string& msg) {
    std::cout << YELLOW << "  " << msg << "..." << RESET << " " << GREEN << "Done!\n" << RESET;
}

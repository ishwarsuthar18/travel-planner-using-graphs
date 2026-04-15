#include "ui.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <string>

void clearScreen() { std::cout << "\033[2J\033[H"; }

void printBanner() {
    std::cout << CYAN << R"(
  ╔══════════════════════════════════════════════════════════╗
  ║                                                          ║
  ║   )" << YELLOW << R"( _____ ____      ___     ____  _       _    _   _ )" << CYAN << R"(  ║
  ║   )" << YELLOW << R"(|_   _|  _ \    / _ \   |  _ \| | __ _| |_ | \ | |)" << CYAN << R"(  ║
  ║   )" << YELLOW << R"(  | | | |_) |  | | | |  | |_) | |/ _` | __||  \| |)" << CYAN << R"(  ║
  ║   )" << YELLOW << R"(  | | |  _ <   | |_| |  |  __/| | (_| | |_ | |\  |)" << CYAN << R"(  ║
  ║   )" << YELLOW << R"(  |_| |_| \_\   \___/   |_|   |_|\__,_|\__||_| \_|)" << CYAN << R"(  ║
  ║                                                          ║
  ║)" << GREEN << R"(        DSA Course Project — Graph Algorithms v1.0        )" << CYAN << R"(║
  ║)" << MAGENTA << R"(               Indian City Network Explorer               )" << CYAN << R"(║
  ╚══════════════════════════════════════════════════════════╝
)" << RESET;
}

void printMainMenu() {
    std::cout << CYAN << "\n  ╔══════════════════════════════════════╗\n";
    std::cout << "  ║" << YELLOW << "          MAIN  MENU                  " << CYAN << "║\n";
    std::cout << "  ╠══════════════════════════════════════╣\n";
    std::cout << "  ║  " << GREEN << "1." << RESET << "  Plan a Route                    " << CYAN << "║\n";
    std::cout << "  ║  " << GREEN << "2." << RESET << "  Multi-Stop Journey              " << CYAN << "║\n";
    std::cout << "  ║  " << GREEN << "3." << RESET << "  Compare All Algorithms          " << CYAN << "║\n";
    std::cout << "  ║  " << GREEN << "4." << RESET << "  View Graph Structure            " << CYAN << "║\n";
    std::cout << "  ║  " << GREEN << "5." << RESET << "  Budget-Constrained Route        " << CYAN << "║\n";
    std::cout << "  ║  " << GREEN << "6." << RESET << "  Floyd-Warshall All-Pairs        " << CYAN << "║\n";
    std::cout << "  ║  " << GREEN << "7." << RESET << "  City Information                " << CYAN << "║\n";
    std::cout << "  ║  " << RED   << "8." << RESET << "  Exit                            " << CYAN << "║\n";
    std::cout << "  ╚══════════════════════════════════════╝\n" << RESET;
    std::cout << "\n  Enter choice: ";
}

void pause() {
    std::cout << YELLOW << "\n  Press ENTER to continue..." << RESET;
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void printCityList(int numCities, const std::string names[]) {
    std::cout << CYAN << "\n  Available Cities:\n" << RESET;
    std::cout << "  " << std::string(40, '-') << "\n";
    for (int i = 0; i < numCities; i++) {
        std::cout << "  " << GREEN << std::setw(3) << (i+1) << ". " << RESET
                  << std::setw(15) << std::left << names[i];
        if ((i+1) % 3 == 0) std::cout << "\n";
    }
    std::cout << "\n  " << std::string(40, '-') << "\n";
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
    return choice - 1;  // 0-indexed
}

int selectCriteria() {
    std::cout << "\n  Optimize by:\n";
    std::cout << "  " << GREEN << "1." << RESET << " Shortest Distance\n";
    std::cout << "  " << GREEN << "2." << RESET << " Fastest Time\n";
    std::cout << "  " << GREEN << "3." << RESET << " Cheapest Cost\n";
    std::cout << "  Choice (1-3): ";
    int c; std::cin >> c;
    if (c < 1 || c > 3) c = 1;
    return c - 1;
}

int selectAlgorithm() {
    std::cout << "\n  Choose Algorithm:\n";
    std::cout << "  " << GREEN << "1." << RESET << " Dijkstra\n";
    std::cout << "  " << GREEN << "2." << RESET << " BFS (fewest hops)\n";
    std::cout << "  " << GREEN << "3." << RESET << " DFS (explore all)\n";
    std::cout << "  " << GREEN << "4." << RESET << " A* (heuristic)\n";
    std::cout << "  " << GREEN << "5." << RESET << " Bellman-Ford\n";
    std::cout << "  Choice (1-5): ";
    int c; std::cin >> c;
    if (c < 1 || c > 5) c = 1;
    return c;
}

void printAlgoExplanation(int algo) {
    std::cout << MAGENTA << "\n  ─── Algorithm Info ───────────────────────────────────\n" << RESET;
    switch (algo) {
        case 1:
            std::cout << "  Dijkstra's: Greedily picks the shortest-known node from\n"
                      << "  a min-heap. Guarantees optimal path in non-negative graphs.\n"
                      << "  Complexity: O((V+E) log V)\n";
            break;
        case 2:
            std::cout << "  BFS: Explores cities level by level. Always finds the\n"
                      << "  path with fewest intermediate stops (hops).\n"
                      << "  Complexity: O(V+E)\n";
            break;
        case 3:
            std::cout << "  DFS: Uses a stack to dive deep into one branch, then\n"
                      << "  backtracks. Finds A path but not necessarily optimal.\n"
                      << "  Complexity: O(V+E)\n";
            break;
        case 4:
            std::cout << "  A*: Combines actual cost with a heuristic (Haversine\n"
                      << "  distance) to guide search toward the goal more intelligently.\n"
                      << "  Complexity: O((V+E) log V)\n";
            break;
        case 5:
            std::cout << "  Bellman-Ford: Relaxes all edges V-1 times. Handles negative\n"
                      << "  weights and detects negative cycles.\n"
                      << "  Complexity: O(V*E)\n";
            break;
    }
    std::cout << MAGENTA << "  ──────────────────────────────────────────────────────\n" << RESET;
}

void animateSearching(const std::string& msg) {
    std::cout << YELLOW << "  " << msg << RESET;
    std::cout.flush();
    for (int i = 0; i < 3; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        std::cout << YELLOW << "." << RESET;
        std::cout.flush();
    }
    std::cout << " " << GREEN << "Done!\n" << RESET;
}

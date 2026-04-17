#pragma once
#include <string>

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define MAGENTA "\033[35m"
#define WHITE   "\033[37m"
#define BLUE    "\033[34m"

void clearScreen();
void printBanner();
void printMainMenu();
void pause();
void printCityList    (int numCities, const std::string names[]);
int  selectCity       (const std::string& prompt, int numCities, const std::string names[]);
int  selectCriteria   ();
int  selectAlgorithm  ();
int  selectTravelMode ();   // NEW: road / rail / air / any
void printAlgoExplanation(int algo);
void animateSearching (const std::string& msg);

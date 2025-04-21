#include "utils.h"
#include "leftRecursion.h"
#include "First_follow.h"
#include "LL1.h"
#include "leftFactoring.h"


int main() {
    std::ofstream logFile("log.txt"); // Open log file
    if (!logFile) {
        std::cerr << "Error opening log file!" << std::endl;
        return 1;
    }

    // Save original cout buffer
    std::streambuf* coutBuffer = std::cout.rdbuf();

    // Redirect cout to both console and file
    std::cout.rdbuf(logFile.rdbuf());

    string filename = "cfg.txt";
    vector<string> cfg = readCFG(filename);
    if (cfg.empty()) {
        std::cerr << "CFG is empty or file not found!" << std::endl;
        return 1;
    }
    displayCFG(cfg, "Original Context-Free Grammar:");
    
    cout << "\nChecking for left factoring...\n";
    vector<string> modifiedCFG = removeLeftFactoring(cfg);
    //filterCFGEntries(modifiedCFG);
    displayCFG(modifiedCFG, "Grammar after Left Factoring Removal:");

    cout << "\nRemoving Left Recursion...\n";
    vector<string> noLeftRecursionCFG = removeLeftRecursion(modifiedCFG);
    displayCFG(noLeftRecursionCFG, "Grammar after Left Recursion Removal:");
    
    vector<string> finalCFG = mergeDuplicateProductions(noLeftRecursionCFG);
    displayCFG(finalCFG, "Final Grammar after merging duplicate productions:");
    
    computeFirstAndFollow(finalCFG);
    
    // Construct and print the LL(1) Parsing Table.
    map<string, map<string, string>> ll1Table = constructLL1Table(finalCFG);
    printLL1Table(ll1Table);

    // Restore original cout buffer
    std::cout.rdbuf(coutBuffer);
    logFile.close();

    return 0;
}
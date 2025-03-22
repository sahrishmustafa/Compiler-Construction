#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm> 
#include <map>
#include <iomanip>
#include <set>
using namespace std;

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

// void formatAndDisplayCFG(const vector<string>& cfg, const string& message) {
//     map<string, set<string>> formattedCFG;
//     size_t maxLHSLength = 0;

//     // Parse each rule and store them in a structured format
//     for (const string& rule : cfg) {
//         size_t arrowPos = rule.find("->");
//         if (arrowPos == string::npos) continue;

//         string lhs = rule.substr(0, arrowPos - 1);
//         string rhs = rule.substr(arrowPos + 2);

//         // Track max LHS length for alignment
//         maxLHSLength = max(maxLHSLength, lhs.length());

//         // Store the RHS productions, avoiding duplicates
//         formattedCFG[lhs].insert(rhs);
//     }

//     // Display formatted CFG
//     cout << "\n" << message << "\n";
//     for (const auto& rule : formattedCFG) {
//         cout << setw(maxLHSLength) << left << rule.first << " -> ";
//         size_t count = 0;
//         for (const auto& prod : rule.second) {
//             cout << prod;
//             if (++count != rule.second.size()) cout << " | ";
//         }
//         cout << endl;
//     }
// }

vector<string> readCFG(const string& filename) {
    vector<string> cfg;
    ifstream file(filename);
    string line;

    if (!file) {
        cerr << "Error opening file!" << endl;
        return cfg;
    }

    while (getline(file, line)) {
        cfg.push_back(line);
    }

    file.close();
    return cfg;
}
void displayCFG(const vector<string>& cfg, const string& message) {
    cout << "\n" << message << "\n";
    for (const string& rule : cfg) {
        cout << rule << endl;
    }
}

string findCommonPrefix(const vector<string>& productions) {
    if (productions.empty()) return "";
    vector<string> relevant;
    char firstChar = productions[0][0];

    for (const string& prod : productions) {
        if (prod[0] == firstChar) {
            relevant.push_back(prod);
        }
    }

    if (relevant.size() < 2) return ""; 
    string prefix = relevant[0];
    for (const string& prod : relevant) {
        size_t j = 0;
        while (j < prefix.size() && j < prod.size() && prefix[j] == prod[j]) {
            j++;
        }
        prefix = prefix.substr(0, j); 
        if (prefix.empty()) break; 
    }

    return prefix;
}


vector<string> removeLeftFactoring(const vector<string>& cfg) {
    vector<string> newCFG;
    map<string, vector<string>> rulesMap;
    cout << "Starting Left Factoring Removal...\n";

    for (const string& rule : cfg) {
        cout << "Processing rule: " << rule << endl;

        size_t arrowPos = rule.find("->");
        if (arrowPos == string::npos) {
            cout << "No '->' found in rule. Skipping...\n";
            newCFG.push_back(rule);
            continue;
        }

        string lhs = trim(rule.substr(0, arrowPos));
        string rhs = trim(rule.substr(arrowPos + 2));

        vector<string> productions;
        size_t pos = 0;

        while ((pos = rhs.find("|")) != string::npos) {
            productions.push_back(trim(rhs.substr(0, pos)));
            rhs.erase(0, pos + 1);
        }
        productions.push_back(trim(rhs)); 

        cout << "Extracted productions: ";
        for (const string& p : productions) cout << "'" << p << "' ";
        cout << endl;

        string commonPrefix = findCommonPrefix(productions);
        cout << "Common Prefix found: '" << commonPrefix << "'\n";

        if (!commonPrefix.empty()) {
            
            string newNonTerminal = lhs + "'";

            
            string newLHS = lhs + " -> " + commonPrefix + " " + newNonTerminal;
            vector<string> newProductions;
            vector<string> remainingProds;

            for (const string& prod : productions) {
                if (prod.find(commonPrefix) == 0) { 
                    string suffix = trim(prod.substr(commonPrefix.size()));
                    if (suffix.empty()) suffix = "$"; 
                    newProductions.push_back(suffix);
                } else {
                    remainingProds.push_back(prod); 
                }
            }

            newCFG.push_back(newLHS);
            string newProdRule = newNonTerminal + " -> " + newProductions[0];
            for (size_t i = 1; i < newProductions.size(); i++) {
                newProdRule += " | " + newProductions[i];
            }
            newCFG.push_back(newProdRule);

            for (const string& remProd : remainingProds) {
                newCFG.push_back(lhs + " -> " + remProd);
            }
            rulesMap[newNonTerminal] = newProductions;
        } else {
            newCFG.push_back(rule);
        }

        cout << "-------------------------------------\n";
    }
    
    for (auto& ruleSet : rulesMap) {
        string newLHS = ruleSet.first;
        vector<string> newRHS = ruleSet.second;
        vector<string> newGrammar;
        newGrammar.push_back(newLHS + " -> " + newRHS[0]);
        for (size_t i = 1; i < newRHS.size(); i++) {
            newGrammar.back() += " | " + newRHS[i];
        }
        vector<string> fullyFactored = removeLeftFactoring(newGrammar);
        newCFG.insert(newCFG.end(), fullyFactored.begin(), fullyFactored.end());
    }
    cout << "Left Factoring Removal Completed.\n";
    return newCFG;
}

int main() {
    string filename = "cfg.txt";
    vector<string> cfg = readCFG(filename);

    if (cfg.empty()) {
        cout << "CFG is empty or file not found!" << endl;
        return 1;
    }

    displayCFG(cfg, "Original Context-Free Grammar:");

    cout << "\nChecking for left factoring...\n";
    vector<string> modifiedCFG = removeLeftFactoring(cfg);
    displayCFG(modifiedCFG, "Grammar after Left Factoring Removal:");

    return 0;
}

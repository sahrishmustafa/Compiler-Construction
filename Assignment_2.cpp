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

vector<string> mergeDuplicateProductions(const vector<string>& cfg) {
    // Map from nonterminal (LHS) to a set of productions (RHS) to avoid duplicates.
    map<string, set<string>> grammar;

    // Process each production rule.
    for (const string& rule : cfg) {
        size_t arrowPos = rule.find("->");
        if (arrowPos == string::npos) continue;

        // Extract LHS and RHS (and trim spaces).
        string lhs = trim(rule.substr(0, arrowPos));
        string rhs = trim(rule.substr(arrowPos + 2));

        // Split the RHS productions by '|'
        stringstream ss(rhs);
        string prod;
        while (getline(ss, prod, '|')) {
            // Trim and insert into the set for LHS.
            grammar[lhs].insert(trim(prod));
        }
    }

    // Reconstruct the merged CFG.
    vector<string> mergedCFG;
    for (const auto& entry : grammar) {
        string mergedRule = entry.first + " -> ";
        bool first = true;
        for (const auto& prod : entry.second) {
            if (!first) {
                mergedRule += " | ";
            }
            mergedRule += prod;
            first = false;
        }
        mergedCFG.push_back(mergedRule);
    }

    return mergedCFG;
}


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

// New function to remove both indirect and immediate left recursion.
vector<string> removeLeftRecursion(const vector<string>& cfg) {
    cout << "Starting Left Recursion Removal...\n";
    // Build a grammar map and preserve the order of nonterminals.
    map<string, vector<string>> grammarMap;
    vector<string> nonTerminalOrder;
    for (const string& rule : cfg) {
        size_t arrowPos = rule.find("->");
        if (arrowPos == string::npos) continue;
        string lhs = trim(rule.substr(0, arrowPos));
        string rhs = trim(rule.substr(arrowPos + 2));

        // If this nonterminal is seen for the first time, record its order.
        if (grammarMap.find(lhs) == grammarMap.end())
            nonTerminalOrder.push_back(lhs);

        // Split the right-hand side by '|'
        stringstream ss(rhs);
        string prod;
        while (getline(ss, prod, '|')) {
            grammarMap[lhs].push_back(trim(prod));
        }
    }

    // Process indirect left recursion.
    int n = nonTerminalOrder.size();
    for (int i = 0; i < n; i++) {
        string A = nonTerminalOrder[i];
        // For each A, substitute productions starting with any A_j (j < i)
        for (int j = 0; j < i; j++) {
            string B = nonTerminalOrder[j];
            vector<string> newProductions;
            for (const string& prod : grammarMap[A]) {
                // Check if production begins with B.
                if (prod.size() >= B.size() && prod.substr(0, B.size()) == B) {
                    string remainder = trim(prod.substr(B.size()));
                    // Substitute each production of B.
                    for (const string& prodB : grammarMap[B]) {
                        string newProd = prodB;
                        if (!remainder.empty())
                            newProd += " " + remainder;
                        newProductions.push_back(newProd);
                    }
                }
                else {
                    newProductions.push_back(prod);
                }
            }
            grammarMap[A] = newProductions;
        }

        // Now eliminate immediate left recursion for nonterminal A.
        vector<string> alpha; // left-recursive parts (of form A -> Aα)
        vector<string> beta;  // non-left-recursive parts

        for (const string& prod : grammarMap[A]) {
            // If production begins with A itself, it is left-recursive.
            if (prod.size() >= A.size() && prod.substr(0, A.size()) == A) {
                string a_part = trim(prod.substr(A.size()));
                if (a_part.empty()) a_part = "$"; // use '$' to denote epsilon if needed
                alpha.push_back(a_part);
            }
            else {
                beta.push_back(prod);
            }
        }

        if (!alpha.empty()) {
            // Create a new nonterminal A'
            string newNonTerminal = A + "'";
            // For each beta production, add A' at the end.
            vector<string> newBeta;
            for (const string& b : beta) {
                newBeta.push_back(b + " " + newNonTerminal);
            }
            // For A', add the productions from alpha with A' recursively.
            vector<string> newAlpha;
            for (const string& a : alpha) {
                newAlpha.push_back(a + " " + newNonTerminal);
            }
            newAlpha.push_back("$"); // epsilon

            // Update grammarMap for A and add new nonterminal.
            grammarMap[A] = newBeta;
            grammarMap[newNonTerminal] = newAlpha;
            // Also record the new nonterminal in the order vector.
            nonTerminalOrder.push_back(newNonTerminal);
            n = nonTerminalOrder.size(); // update size since we've added a new nonterminal.
        }
    }

    // Convert the grammar map back to vector<string> format.
    vector<string> newCFG;
    for (const auto& nt : nonTerminalOrder) {
        if (grammarMap.find(nt) == grammarMap.end())
            continue;
        string ruleString = nt + " -> " + grammarMap[nt][0];
        for (size_t i = 1; i < grammarMap[nt].size(); i++) {
            ruleString += " | " + grammarMap[nt][i];
        }
        newCFG.push_back(ruleString);
    }
    cout << "Left Recursion Removal Completed.\n";
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

    cout << "\nRemoving Left Recursion...\n";
    vector<string> noLeftRecursionCFG = removeLeftRecursion(modifiedCFG);
    displayCFG(noLeftRecursionCFG, "Grammar after Left Recursion Removal:");

    // Merge duplicate productions.
    vector<string> finalCFG = mergeDuplicateProductions(noLeftRecursionCFG);
    displayCFG(finalCFG, "Final Grammar after merging duplicate productions:");

    return 0;
}

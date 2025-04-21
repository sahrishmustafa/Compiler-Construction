#ifndef LL1_H
#define LL1_H
#include "utils.h"
#include "First_follow.h"
// ********************
// LL(1) Parsing Table Construction
// ********************
set<string> computeFirstForProduction(const string &production, 
    map<string, vector<string>>& grammar, 
    map<string, set<string>>& firstSets) {
set<string> result;
vector<string> tokens = splitProduction(production);
bool allEpsilon = true;
for (size_t i = 0; i < tokens.size() && allEpsilon; i++) {
allEpsilon = false;
set<string> firstOfToken = computeFirst(tokens[i], grammar, firstSets);
for (const string &s : firstOfToken) {
if (s != "$")
result.insert(s);
}
if (firstOfToken.find("$") != firstOfToken.end())
allEpsilon = true;
}
if (allEpsilon)
result.insert("$");
return result;
}

map<string, map<string, string>> constructLL1Table(vector<string>& cfg) {
    map<string, vector<string>> grammar;
    // Build the grammar map from the final CFG.
    for (const string& rule : cfg) {
        size_t arrowPos = rule.find("->");
        if (arrowPos == string::npos) continue;
        string lhs = trim(rule.substr(0, arrowPos));
        string rhs = trim(rule.substr(arrowPos + 2));
        stringstream ss(rhs);
        string prod;
        while (getline(ss, prod, '|')) {
            grammar[lhs].push_back(trim(prod));
        }
    }
    
    map<string, map<string, string>> table;

    for (auto& entry : grammar) {
        string A = entry.first;
        for (const string& production : entry.second) {
            set<string> firstProd = computeFirstForProduction(production, grammar, firstSets);
            for (const string &t : firstProd) {
                if (t != "$") {
                    table[A][t] = production;
                }
            }
            if (firstProd.find("$") != firstProd.end()) {
                for (const string &t : followSets[A]) {
                    table[A][t] = production;
                }
            }
        }
    }
    return table;
}

void printLL1Table(map<string, map<string, string>> &table) {
    set<string> allTerminals;
    for (auto &entry : table) {
        for (auto &inner : entry.second) {
            allTerminals.insert(inner.first);
        }
    }
    
    const int colWidth = 8;  
    
    // Print header.
    cout << "\nLL(1) Parsing Table:\n";
    cout << setw(colWidth) << left << "NT";
    for (const string &t : allTerminals) {
        cout << setw(colWidth) << left << t;
    }
    cout << "\n" << string(colWidth + colWidth * allTerminals.size(), '-') << "\n";
    
    // For each non-terminal row.
    for (auto &entry : table) {
        cout << setw(colWidth) << left << entry.first;
        for (const string &t : allTerminals) {
            if (entry.second.find(t) != entry.second.end())
                cout << setw(colWidth) << left << entry.second[t];
            else
                cout << setw(colWidth) << left << "";
        }
        cout << "\n";
    }
}
#endif
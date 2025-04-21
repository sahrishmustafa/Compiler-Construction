#ifndef LEFTFACTORING_H
#define LEFTFACTORING_H
#include "utils.h"

// ********************
// Left Factoring Implementation
// ********************

// Helper function to compute the longest common prefix between two strings.
string getLongestCommonPrefix(const string& str1, const string& str2) {
    int length = min(str1.size(), str2.size());
    int i = 0;
    while (i < length && str1[i] == str2[i]) {
        i++;
    }
    return str1.substr(0, i);
}

// Function to apply left factoring to a single rule.
vector<string> applyLeftFactoring(const string& rule) {
    size_t arrowPos = rule.find("->");
    if (arrowPos == string::npos) return {rule};
    
    string lhs = trim(rule.substr(0, arrowPos));
    string rhs = trim(rule.substr(arrowPos + 2));
    
    vector<string> productions;
    stringstream ss(rhs);
    string production;
    while (getline(ss, production, '|')) {
        productions.push_back(trim(production));
    }
    if (productions.size() < 2) return {rule}; // No factoring needed
    
    bool isFactored = false;
    vector<string> updatedProductions;
    vector<string> newRules;
    
    // Group productions by their first character.
    map<char, vector<string>> groupedProds;
    for (auto& prod : productions) {
        if (!prod.empty()) groupedProds[prod[0]].push_back(prod);
        else updatedProductions.push_back(prod);
    }
    
    // Process each group separately.
    for (auto& entry : groupedProds) {
        vector<string>& group = entry.second;
        if (group.size() > 1) {
            isFactored = true;
            
            string commonPrefix = group[0];
            for (size_t i = 1; i < group.size(); i++) {
                commonPrefix = getLongestCommonPrefix(commonPrefix, group[i]);
            }
            
            if (!commonPrefix.empty()) {
                static int counter = 1;
                string newNonTerminal = lhs + string(counter++, '\'');
                
                vector<string> remainingProds;
                for (auto& prod : group) {
                    string suffix = trim(prod.substr(commonPrefix.size()));
                    if (suffix.empty()) suffix = "%"; // Represents epsilon
                    remainingProds.push_back(suffix);
                }
                
                updatedProductions.push_back(commonPrefix + " " + newNonTerminal);
                
                string newRule = newNonTerminal + " -> " + remainingProds[0];
                for (size_t i = 1; i < remainingProds.size(); i++) {
                    newRule += " | " + remainingProds[i];
                }
                newRules.push_back(newRule);
            } else {
                for (auto& prod : group) updatedProductions.push_back(prod);
            }
        } else {
            updatedProductions.push_back(group[0]);
        }
    }
    
    // Reconstruct the factored rule.
    string factoredRule = lhs + " -> " + updatedProductions[0];
    for (size_t i = 1; i < updatedProductions.size(); i++) {
        factoredRule += " | " + updatedProductions[i];
    }
    
    vector<string> result = {factoredRule};
    for (auto& newRule : newRules) {
        vector<string> recursivelyFactored = applyLeftFactoring(newRule);
        result.insert(result.end(), recursivelyFactored.begin(), recursivelyFactored.end());
    }
    
    return isFactored ? result : vector<string>{rule};
}

// Function to apply left factoring to an entire CFG.
vector<string> removeLeftFactoring(const vector<string>& cfg) {
    cout << "Starting Left Factoring Removal...\n";
    vector<string> updatedCFG;
    
    for (const string& rule : cfg) {
        vector<string> factoredRules = applyLeftFactoring(rule);
        updatedCFG.insert(updatedCFG.end(), factoredRules.begin(), factoredRules.end());
    }
    
    cout << "Left Factoring Removal Completed.\n";
    return updatedCFG;
}

#endif

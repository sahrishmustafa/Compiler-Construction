#ifndef LEFT_RECURSION_H
#define LEFT_RECURSION_H
#include "utils.h"
// ********************
// Remove Left Recursion
// ********************

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
                if (a_part.empty()) a_part = "%"; // use '$' to denote epsilon if needed
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
            newAlpha.push_back("%"); // epsilon

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
#endif
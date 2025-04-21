#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm> 
#include <map>
#include <iomanip>
#include <set>
#include <cctype>
#include <unordered_map>
#include <streambuf>
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


// Tokenizes a production string into individual tokens.
// Nonterminals are considered as an uppercase letter plus any following apostrophes.
// Other characters (e.g. lowercase letters, % etc.) are considered individual tokens.
vector<string> tokenizeProduction(const string& prod) {
    vector<string> tokens;
    int n = prod.size();
    int i = 0;
    while (i < n) {
        if (isspace(prod[i])) {
            i++;
            continue;
        }
        string token;
        // If uppercase letter, consume the letter and any apostrophes following it.
        if (isupper(prod[i])) {
            token.push_back(prod[i]);
            i++;
            while (i < n && prod[i] == '\'') {
                token.push_back(prod[i]);
                i++;
            }
        }
        else {
            // For lowercase letters, '%' or any other symbol, treat each as a separate token.
            token.push_back(prod[i]);
            i++;
        }
        tokens.push_back(token);
    }
    return tokens;
}

// Joins a vector of tokens with a single space between each.
string joinTokens(const vector<string>& tokens) {
    string result;
    for (size_t i = 0; i < tokens.size(); i++) {
        result += tokens[i];
        if (i != tokens.size() - 1)
            result += " ";
    }
    return result;
}

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
            // Tokenize the production and then join the tokens with a space.
            vector<string> tokens = tokenizeProduction(prod);
            mergedRule += joinTokens(tokens);
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
void filterCFGEntries(vector<string>& cfg) {
    cfg.erase(remove_if(cfg.begin(), cfg.end(), [](const string& rule) {
        return rule.find("-> ") == string::npos;
    }), cfg.end());
    for (size_t i = 0; i < cfg.size(); i++) {
        for (size_t j = i + 1; j < cfg.size(); j++) {
            // If we find a duplicate, erase it.
            if (cfg[i] == cfg[j]) {
                cfg.erase(cfg.begin() + j);
                j--; // Decrease j to recheck the new element at index j.
            }
        }
    }
}
#endif
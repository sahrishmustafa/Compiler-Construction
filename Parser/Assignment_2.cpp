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

#include <cctype>

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

// Helper: computes longest common prefix between two strings.
string longestCommonPrefix(const string& s1, const string& s2) {
    int n = min(s1.size(), s2.size());
    int i = 0;
    for (; i < n; i++) {
        if (s1[i] != s2[i])
            break;
    }
    return s1.substr(0, i);
}

// Recursive helper that factors a single production rule (for one nonterminal).
// If no factoring is possible, it returns the original rule in a vector.
vector<string> factorRuleSingle(const string& rule) {
    size_t arrowPos = rule.find("->");
    if (arrowPos == string::npos)
        return { rule };
    string lhs = trim(rule.substr(0, arrowPos));
    string rhs = trim(rule.substr(arrowPos + 2));

    // Split RHS into alternatives.
    vector<string> prods;
    stringstream ss(rhs);
    string prod;
    while (getline(ss, prod, '|')) {
        prods.push_back(trim(prod));
    }
    // If there is only one alternative, nothing to factor.
    if (prods.size() < 2)
        return { rule };

    bool factored = false;
    vector<string> newLHS_prods; // will hold new alternatives for lhs
    vector<string> extraRules;   // holds new rules generated by factoring

    // Group productions by their first character.
    // (A more refined implementation might group by first token, but here we use char.)
    map<char, vector<string>> groups;
    for (auto& p : prods) {
        if (!p.empty())
            groups[p[0]].push_back(p);
        else
            newLHS_prods.push_back(p);
    }
    // Process each group.
    for (auto& entry : groups) {
        vector<string>& groupProds = entry.second;
        if (groupProds.size() > 1) {
            // Compute the longest common prefix (LCP) among all productions in this group.
            string lcp = groupProds[0];
            for (size_t i = 1; i < groupProds.size(); i++) {
                lcp = longestCommonPrefix(lcp, groupProds[i]);
            }
            if (lcp.size() > 0) {
                factored = true;
                // Generate a new nonterminal name. We use a static counter to generate unique names.
                static int factorCounter = 1;
                string newNonTerminal = lhs + "'";
                if (factorCounter > 1)
                    newNonTerminal = lhs + string(factorCounter, '\'');
                factorCounter++;

                // For each production in the group, remove the common prefix.
                vector<string> newSubProds;
                for (auto& prod : groupProds) {
                    string remainder = prod.substr(lcp.size());
                    remainder = trim(remainder);
                    if (remainder == "")
                        remainder = "%"; // use 'ʌ' to denote epsilon
                    newSubProds.push_back(remainder);
                }
                // Add the factored alternative: common prefix + new nonterminal.
                newLHS_prods.push_back(lcp + " " + newNonTerminal);
                // Create a new rule for the new nonterminal.
                string extraRule = newNonTerminal + " -> " + newSubProds[0];
                for (size_t i = 1; i < newSubProds.size(); i++) {
                    extraRule += " | " + newSubProds[i];
                }
                extraRules.push_back(extraRule);
            }
            else {
                // No common prefix found in this group, so add the productions as they are.
                for (auto& prod : groupProds)
                    newLHS_prods.push_back(prod);
            }
        }
        else {
            // Only one production in this group, add it directly.
            newLHS_prods.push_back(groupProds[0]);
        }
    }
    // Reconstruct the new rule for lhs.
    string newRule = lhs + " -> " + newLHS_prods[0];
    for (size_t i = 1; i < newLHS_prods.size(); i++) {
        newRule += " | " + newLHS_prods[i];
    }
    vector<string> result;
    result.push_back(newRule);
    // Recursively factor each extra rule.
    for (auto& er : extraRules) {
        vector<string> rec = factorRuleSingle(er);
        result.insert(result.end(), rec.begin(), rec.end());
    }
    // If no factoring occurred, return the original rule.
    if (!factored)
        return { rule };
    return result;
}

// New left factoring implementation that applies factorRuleSingle on each production.
vector<string> removeLeftFactoring(const vector<string>& cfg) {
    cout << "Starting Left Factoring Removal...\n";
    vector<string> newCFG;
    for (const string& rule : cfg) {
        vector<string> factoredRules = factorRuleSingle(rule);
        // Add all factored rules to the new CFG.
        newCFG.insert(newCFG.end(), factoredRules.begin(), factoredRules.end());
    }
    cout << "Left Factoring Removal Completed.\n";
    return newCFG;
}


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


// ********************
// FIRST and FOLLOW Computation
// ********************

map<string, set<string>> firstSets;
map<string, set<string>> followSets;
set<string> terminals;
set<string> nonTerminals;
string startSymbol;

vector<string> splitProduction(const string& prod) {
    vector<string> tokens;
    int i = 0;
    while (i < prod.size()) {
        if (isspace(prod[i])) {
            i++;
            continue;
        }
        // If uppercase letter, start a non-terminal token.
        if (isupper(prod[i])) {
            string nt;
            nt.push_back(prod[i]);
            i++;
            // Include following apostrophes (e.g., A')
            while (i < prod.size() && prod[i] == '\'') {
                nt.push_back(prod[i]);
                i++;
            }
            tokens.push_back(nt);
        }
        else {
            // Terminal: collect until whitespace or an uppercase letter.
            string t;
            while (i < prod.size() && !isspace(prod[i]) && !isupper(prod[i])) {
                t.push_back(prod[i]);
                i++;
            }
            tokens.push_back(t);
        }
    }
    return tokens;
}

// Compute FIRST for non-terminals only. 
// If the symbol is a terminal, immediately return a set containing that symbol.
set<string> computeFirst(string symbol, map<string, vector<string>>& grammar, map<string, set<string>>& firstSets) {
    // If symbol is a terminal, return it immediately.
    if (terminals.find(symbol) != terminals.end()) {
        set<string> ret;
        ret.insert(symbol);
        return ret;
    }
    // If already computed, return stored FIRST set.
    if (!firstSets[symbol].empty())
        return firstSets[symbol];

    set<string> firstSet;
    // If symbol is not a non-terminal in our grammar, treat it as terminal.
    if (grammar.find(symbol) == grammar.end()) {
        firstSet.insert(symbol);
        // Do not store terminals in firstSets.
        return firstSet;
    }
    // Process each production for this non-terminal.
    for (const auto& production : grammar[symbol]) {
        bool epsilonPresent = true;
        vector<string> symbolsInProduction = splitProduction(production);
        for (size_t i = 0; i < symbolsInProduction.size() && epsilonPresent; i++) {
            epsilonPresent = false;
            string currSym = symbolsInProduction[i];
            set<string> firstOfCurr = computeFirst(currSym, grammar, firstSets);
            for (const string& str : firstOfCurr) {
                if (str != "%")  // Here "%" represents epsilon.
                    firstSet.insert(str);
            }
            if (firstOfCurr.find("%") != firstOfCurr.end())
                epsilonPresent = true;
        }
        if (epsilonPresent)
            firstSet.insert("%");
    }
    firstSets[symbol] = firstSet;
    return firstSet;
}

// For each occurrence of a non-terminal in productions, add FIRST of the following tokens (excluding epsilon).
// If no token follows or all following tokens derive epsilon, add FOLLOW of the LHS.
void computeFollow(const string& symbol, map<string, vector<string>>& grammar) {
    // Only compute FOLLOW for non-terminals.
    if (nonTerminals.find(symbol) == nonTerminals.end())
        return;
    if (symbol == startSymbol)
        followSets[symbol].insert("$");

    for (auto& entry : grammar) {
        string lhs = entry.first;
        for (const string& production : entry.second) {
            vector<string> tokens = splitProduction(production);
            for (size_t i = 0; i < tokens.size(); i++) {
                if (tokens[i] == symbol) {
                    bool epsilonInNext = true;
                    for (size_t j = i + 1; j < tokens.size() && epsilonInNext; j++) {
                        epsilonInNext = false;
                        set<string> firstOfNext = computeFirst(tokens[j], grammar, firstSets);
                        for (const string& str : firstOfNext) {
                            if (str != "$")
                                followSets[symbol].insert(str);
                        }
                        if (firstOfNext.find("$") != firstOfNext.end())
                            epsilonInNext = true;
                    }
                    if (i == tokens.size() - 1 || epsilonInNext) {
                        if (lhs != symbol) {
                            computeFollow(lhs, grammar);
                            for (const string& str : followSets[lhs])
                                followSets[symbol].insert(str);
                        }
                    }
                }
            }
        }
    }
}

void computeFirstAndFollow(const vector<string>& cfg) {
    map<string, vector<string>> grammar;
    // Build grammar map and record non-terminals and terminals.
    for (const string& rule : cfg) {
        size_t arrowPos = rule.find("->");
        if (arrowPos == string::npos) continue;
        string lhs = trim(rule.substr(0, arrowPos));
        string rhs = trim(rule.substr(arrowPos + 2));
        if (startSymbol.empty())
            startSymbol = lhs;
        nonTerminals.insert(lhs);
        stringstream ss(rhs);
        string prod;
        while (getline(ss, prod, '|')) {
            string trimmedProd = trim(prod);
            grammar[lhs].push_back(trimmedProd);
            vector<string> tokens = splitProduction(trimmedProd);
            for (const string& token : tokens) {
                if (token == "%")
                    continue;
                // Non-terminals: assume they are uppercase (possibly with apostrophes)
                if (!token.empty() && isupper(token[0]))
                    nonTerminals.insert(token);
                else
                    terminals.insert(token);
            }
        }
    }
    // Compute FIRST sets for non-terminals.
    for (const string& nt : nonTerminals)
        computeFirst(nt, grammar, firstSets);
    cout << "\nFIRST Sets:\n";
    for (const string& nt : nonTerminals) {
        cout << "FIRST(" << nt << ") = { ";
        for (const string& s : firstSets[nt])
            cout << s << " ";
        cout << "}\n";
    }
    // Compute FOLLOW sets for non-terminals.
    for (const string& nt : nonTerminals)
        computeFollow(nt, grammar);
    cout << "\nFOLLOW Sets:\n";
    for (const string& nt : nonTerminals) {
        cout << "FOLLOW(" << nt << ") = { ";
        for (const string& s : followSets[nt])
            cout << s << " ";
        cout << "}\n";
    }
}


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

// We construct the LL(1) table as a map of non-terminal -> (terminal -> production)
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
    // For each non-terminal production A -> α
    for (auto& entry : grammar) {
        string A = entry.first;
        for (const string& production : entry.second) {
            // Use computeFirstForProduction to get FIRST for the entire production
            set<string> firstProd = computeFirstForProduction(production, grammar, firstSets);
            // For each terminal in FIRST(α) except epsilon
            for (const string &t : firstProd) {
                if (t != "$") {
                    table[A][t] = production;
                }
            }
            // If production can derive epsilon, use FOLLOW(A)
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
    // Collect all terminals (including $) that appear in the table.
    set<string> allTerminals;
    for (auto &entry : table) {
        for (auto &inner : entry.second) {
            allTerminals.insert(inner.first);
        }
    }
    
    // Use a smaller column width for compact output.
    const int colWidth = 8;  // adjust this value if needed
    
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
    
    return 0;
}

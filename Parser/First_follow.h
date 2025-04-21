#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H
#include "utils.h"

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
        if (isupper(prod[i])) {
            string nt;
            nt.push_back(prod[i]);
            i++;
            while (i < prod.size() && prod[i] == '\'') {
                nt.push_back(prod[i]);
                i++;
            }
            tokens.push_back(nt);
        }
        else {
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

set<string> computeFirst(string symbol, map<string, vector<string>>& grammar, map<string, set<string>>& firstSets) {
    if (terminals.find(symbol) != terminals.end()) {
        return {symbol};
    }
    if (!firstSets[symbol].empty())
        return firstSets[symbol];

    set<string> firstSet;
    if (grammar.find(symbol) == grammar.end()) {
        firstSet.insert(symbol);
        return firstSet;
    }
    
    for (const auto& production : grammar[symbol]) {
        bool epsilonPresent = true;
        vector<string> symbolsInProduction = splitProduction(production);
        for (size_t i = 0; i < symbolsInProduction.size() && epsilonPresent; i++) {
            epsilonPresent = false;
            string currSym = symbolsInProduction[i];
            set<string> firstOfCurr = computeFirst(currSym, grammar, firstSets);
            for (const string& str : firstOfCurr) {
                if (str != "%")
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

void computeFollow(const string& symbol, map<string, vector<string>>& grammar) {
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
                if (!token.empty() && isupper(token[0]))
                    nonTerminals.insert(token);
                else
                    terminals.insert(token);
            }
        }
    }
    
    for (const string& nt : nonTerminals)
        computeFirst(nt, grammar, firstSets);
    cout << "\nFIRST Sets:\n";
    for (const string& nt : nonTerminals) {
        cout << "FIRST(" << nt << ") = { ";
        for (const string& s : firstSets[nt])
            cout << s << " ";
        cout << "}\n";
    }
    
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

#endif

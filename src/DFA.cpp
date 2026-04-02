#include "DFA.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cctype>
using namespace std;

string transition::getNextState(const string& currentState, const string& dead_state, char inputSymbol) {
    if (currentState == dead_state) {
        return dead_state;
    }

    auto it = transitions.find({currentState, inputSymbol});
    if (it != transitions.end()) {
        return it->second;
    }

    auto wild_it = wildcard_transitions.find(currentState);
    if (wild_it != wildcard_transitions.end()) {
        return wild_it->second;
    }

    return dead_state;
}

void transition::addTransition(string qprev, string input_symbol, string qnext){
    if (input_symbol.compare("ANY") == 0) {
        wildcard_transitions[qprev] = qnext;
    } else if (input_symbol.compare("ANY2") == 0){
        for (unsigned char c = 0; c < 128; c++)
            if (isalnum(c))
                transitions[{qprev, (char)c}] = qnext;
        transitions[{qprev, ' '}] = qnext;
    } else {
        char input_char = input_symbol[0];
        transitions[{qprev, input_char}] = qnext;
    }
}

DFA::DFA(const string& rules_file) : dead_state("q_trap") {
    loadRules(rules_file);
}

vector<string> DFA::split(const string& str, const string& cutter) {
    vector<string> tokens;
    size_t start = 0, pos;
    
    while ((pos = str.find(cutter, start)) != string::npos) {
        string token = str.substr(start, pos - start);
        
        size_t tstart = token.find_first_not_of(" \t\r\n");
        size_t tend = token.find_last_not_of(" \t\r\n");
        if (tstart != string::npos)
            tokens.push_back(token.substr(tstart, tend - tstart + 1));
        
        start = pos + cutter.size();
    }
    
    string last = str.substr(start);
    size_t tstart = last.find_first_not_of(" \t\r\n");
    size_t tend = last.find_last_not_of(" \t\r\n");
    if (tstart != string::npos)
        tokens.push_back(last.substr(tstart, tend - tstart + 1));
    
    return tokens;
}

string DFA::toLower(const string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

void DFA::loadRules(const string& filepath) {
    ifstream file(filepath);
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;
        if (line.find("#") == 0) continue;

        if (line.find("startstate") == 0) {
            auto parts = split(line, "|");
            start_state = parts[1];
        } else if (line.find("deadstate") == 0) {
            auto parts = split(line, "|");
            dead_state = parts[1];
        } else if (line.find("finalstate") == 0) {
            auto parts = split(line, "|");
            string states_str = parts[1];
            auto states = split(states_str, ",");
            final_states.insert(states.begin(), states.end());
        } else {
            auto parts = split(line, " ");
            transitions.addTransition(parts[0], parts[1], parts[2]);
        }
    }
}

bool DFA::isFinalState(const string& state) const {
    return final_states.find(state) != final_states.end();
}

bool DFA::isDeadState(const string& state) const {
    return state == dead_state;
}
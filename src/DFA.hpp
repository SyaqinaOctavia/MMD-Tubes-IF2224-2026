#ifndef DFA_H
#define DFA_H

#include <string>
#include <set>
#include <map>
#include <vector>
#include <utility>
using namespace std;

class transition {
    private:
        map<pair<string, char>, string> transitions;
        map<string, string> wildcard_transitions;
    public:
        void addTransition(string qprev, string input_symbol, string qnext);
        string getNextState(const string& current, const string& dead_state, char inputSymbol);
};

class DFA {
public:
    DFA(const string& rules_file);

    transition getTransition(){ return transitions; };
    string getDeadState(){ return dead_state; };
    string getStartState(){ return start_state; };
    bool isFinalState(const string& state) const;
    bool isDeadState(const string& state) const;

private:
    string start_state;
    set<string> final_states;
    string dead_state;
    transition transitions;
    map<string, string> token_types;
    set<string> keywords;

    void loadRules(const string& filepath);
    vector<string> split(const string& str, const string& delim);
    string toLower(const string& str);
};

#endif
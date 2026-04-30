#include "lexer.hpp"
#include "DFA.hpp"

void Lexer::advance(){
    char c = scanner.get();
    EOP = (currentChar == EOF);
    if(c == EOF){
        cout << endl << "File mencapai akhir (EOF)" << endl;
        EOP = true;
        currentChar = EOF;
        scanner.close();
        return;
    }

    currentChar = c;
}

void Lexer::readFile(string filepath){
    scanner.open(filepath);
    if(!scanner){
        cout << "Gagal membuka file pada path " << filepath << endl;
        EOP = true;
        currentChar = EOF;
        scanner.close();
        return;
    }
    line = 0;
    advance();
}

void Lexer::skipWhitespace(){
    while(isspace(currentChar)){
        if(currentChar == '\n') line++;
        advance();
    }
}

Lexer::Lexer() : EOP(false), currentChar('\0') {}
 
bool Lexer::generateTokenToFile(string srcFile, string destFile){
    cout << "CONSOLE LOG" << endl;
    ofstream output(destFile);
    if(!output){
        cerr << "Failed to open output file: " << destFile << endl;
        return false;
    }
    
    // Load DFA rules
    DFA dfa("src/Lexer/dfa_rules.txt");
    transition trans = dfa.getTransition();
    
    // Load source file
    readFile(srcFile);
    while(!isEndFile()){
        skipWhitespace();
        if(isEndFile()) break;
        
        
        string state = "q0"; // start state
        string lexeme = "";
        string last_final_state = "";
        string last_final_lexeme = "";
        
        // Build token 
        while(!isEndFile()){
            char ch = getCurrent();
            string next_state = trans.getNextState(state, "q_trap", ch);
            if(next_state == "q_trap"){
                if(lexeme.empty()){
                    lexeme += ch;
                    advance();
                }
                break;
            }
            
            // Add character to lexeme and transition
            if(state.compare("q_string") == 0 && ch == '\'') lexeme = lexeme;
            else lexeme += ch;
            state = next_state;
            int lexlen = lexeme.length();
            string space((20 * (lexlen / 20 + 1))-lexlen, ' ');
            cout << lexeme << space << "| " << state << "\n";
            advance();
            if(dfa.isFinalState(state)){
                last_final_state = state;
                last_final_lexeme = lexeme;
            }
        }
        
        // Check if we found a valid token
        if(last_final_state == ""){
            // No valid token found
            bool allAlnum = true;
            for(char c : lexeme){
                if(!isalnum(c)){
                    output << "unknown (" << lexeme << ")" << endl;
                    allAlnum = false;
                    break;
                }
            }
            if(allAlnum) output << "ident (" << lexeme << ")" << endl;
            continue;
        }
        
        // Rollback extra chars
        int extra = lexeme.length() - last_final_lexeme.length();
        for(int i = 0; i < extra; i++){
            scanner.unget();
        }
        if(extra > 0){
            char c = scanner.peek();
            if(c != EOF){
                currentChar = c;
                EOP = false;
            } else {
                EOP = true;
                currentChar = EOF;
            }
        }
        
        // Get token name
        string token_name = last_final_state;
        if(token_name.substr(0, 2) == "q_"){
            token_name = token_name.substr(2);
        }
        
        // Skip comments
        // if(token_name == "comment"){
        //     continue;
        // }
        
        // Emit token with or without value
        if(token_name == "ident" || token_name == "intcon" || 
           token_name == "realcon" || token_name == "string" || 
           token_name == "charcon" || token_name == "comment"){
            output << token_name << " (" << last_final_lexeme << ")" << endl;
        } else {
            output << token_name << endl;
        }
    }
    
    output.close();
    return true;
}

bool Lexer::generateTokenToFile(string srcFile, string destFile){
    cout << "CONSOLE LOG" << endl;
    ofstream output(destFile);
    if(!output){
        cerr << "Failed to open output file: " << destFile << endl;
        return false;
    }
    
    // Load DFA rules
    DFA dfa("src/Lexer/dfa_rules.txt");
    transition trans = dfa.getTransition();
    
    // Load source file
    readFile(srcFile);
    while(!isEndFile()){
        skipWhitespace();
        if(isEndFile()) break;
        
        
        string state = "q0"; // start state
        string lexeme = "";
        string last_final_state = "";
        string last_final_lexeme = "";
        
        // Build token 
        while(!isEndFile()){
            char ch = getCurrent();
            string next_state = trans.getNextState(state, "q_trap", ch);
            if(next_state == "q_trap"){
                if(lexeme.empty()){
                    lexeme += ch;
                    advance();
                }
                break;
            }
            
            // Add character to lexeme and transition
            if(state.compare("q_string") == 0 && ch == '\'') lexeme = lexeme;
            else lexeme += ch;
            state = next_state;
            int lexlen = lexeme.length();
            string space((20 * (lexlen / 20 + 1))-lexlen, ' ');
            cout << lexeme << space << "| " << state << "\n";
            advance();
            if(dfa.isFinalState(state)){
                last_final_state = state;
                last_final_lexeme = lexeme;
            }
        }
        
        // Check if we found a valid token
        if(last_final_state == ""){
            // No valid token found
            bool allAlnum = true;
            for(char c : lexeme){
                if(!isalnum(c)){
                    output << "unknown (" << lexeme << ")" << endl;
                    allAlnum = false;
                    break;
                }
            }
            if(allAlnum) output << "ident (" << lexeme << ")" << endl;
            continue;
        }
        
        // Rollback extra chars
        int extra = lexeme.length() - last_final_lexeme.length();
        for(int i = 0; i < extra; i++){
            scanner.unget();
        }
        if(extra > 0){
            char c = scanner.peek();
            if(c != EOF){
                currentChar = c;
                EOP = false;
            } else {
                EOP = true;
                currentChar = EOF;
            }
        }
        
        // Get token name
        string token_name = last_final_state;
        if(token_name.substr(0, 2) == "q_"){
            token_name = token_name.substr(2);
        }
        
        // Skip comments
        // if(token_name == "comment"){
        //     continue;
        // }
        
        // Emit token with or without value
        if(token_name == "ident" || token_name == "intcon" || 
           token_name == "realcon" || token_name == "string" || 
           token_name == "charcon" || token_name == "comment"){
            output << token_name << " (" << last_final_lexeme << ")" << endl;
        } else {
            output << token_name << endl;
        }
    }
    
    output.close();
    return true;
}

std::vector<Token> Lexer::generateToken(string srcFile){
    std::vector<Token> tokens;
    cout << "CONSOLE LOG" << endl;
    
    // Load DFA rules
    DFA dfa("src/Lexer/dfa_rules.txt");
    transition trans = dfa.getTransition();
    
    // Load source file
    readFile(srcFile);
    while(!isEndFile()){
        skipWhitespace();
        if(isEndFile()) break;
        
        
        string state = "q0"; // start state
        string lexeme = "";
        string last_final_state = "";
        string last_final_lexeme = "";
        
        // Build token 
        while(!isEndFile()){
            char ch = getCurrent();
            string next_state = trans.getNextState(state, "q_trap", ch);
            if(next_state == "q_trap"){
                if(lexeme.empty()){
                    lexeme += ch;
                    advance();
                }
                break;
            }
            
            // Add character to lexeme and transition
            if(state.compare("q_string") == 0 && ch == '\'') lexeme = lexeme;
            else lexeme += ch;
            state = next_state;
            int lexlen = lexeme.length();
            string space((20 * (lexlen / 20 + 1))-lexlen, ' ');
            cout << lexeme << space << "| " << state << "\n";
            advance();
            if(dfa.isFinalState(state)){
                last_final_state = state;
                last_final_lexeme = lexeme;
            }
        }
        
        // Check if we found a valid token
        if(last_final_state == ""){
            // No valid token found
            bool allAlnum = true;
            for(char c : lexeme){
                if(!isalnum(c)){
                    tokens.push_back(Token(Symbol::unknown, lexeme));
                    allAlnum = false;
                    break;
                }
            }
            if(allAlnum) tokens.push_back(Token(Symbol::ident, lexeme));
            continue;
        }
        
        // Rollback extra chars
        int extra = lexeme.length() - last_final_lexeme.length();
        for(int i = 0; i < extra; i++){
            scanner.unget();
        }
        if(extra > 0){
            char c = scanner.peek();
            if(c != EOF){
                currentChar = c;
                EOP = false;
            } else {
                EOP = true;
                currentChar = EOF;
            }
        }
        
        // Get token name
        string token_name = last_final_state;
        if(token_name.substr(0, 2) == "q_"){
            token_name = token_name.substr(2);
        }
        
        // Skip comments
        // if(token_name == "comment"){
        //     continue;
        // }
        
        // Emit token with or without value
        auto it = Token::keywordMap.find(token_name);
        if(token_name == "ident" || token_name == "intcon" || 
           token_name == "realcon" || token_name == "string" || 
           token_name == "charcon" || token_name == "comment"){
            tokens.push_back(Token(it->second, last_final_lexeme));
        } else {
            tokens.push_back(Token(it->second));
        }
    }
    
    return tokens;
}
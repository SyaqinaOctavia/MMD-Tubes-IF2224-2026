#include "lexer.hpp"

Lexer::Lexer(string filepath){
    readFile(filepath);
}

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

vector<Token> Lexer::tokenize(DFA& DFA){
    vector<Token> tokens;
    skipWhitespace();

    string lexeme = "";
    string currentState = DFA.getStartState();
    transition machine = DFA.getTransition();
    while(!EOP){
        string nextState = machine.getNextState(currentState, DFA.getDeadState(), currentChar);
        if(DFA.isDeadState(nextState)){
            if(DFA.isFinalState(currentState)){
                tokens.push_back(Token(currentState, lexeme, line));
                lexeme = "";
                if(isspace(currentChar)) skipWhitespace();
                currentState = DFA.getStartState();
                continue;
            } else {
                break;
            }
        } else if(DFA.isFinalState(nextState) && isspace(currentChar)){
            tokens.push_back(Token(nextState, lexeme, line));
            lexeme = "";
            skipWhitespace();
            currentState = DFA.getStartState();
            continue;
        } else {
            lexeme += currentChar;
            currentState = nextState;
        }
        advance();
    }
    if(!lexeme.empty() && DFA.isFinalState(currentState)) tokens.push_back(Token(currentState, lexeme, line));
    return tokens;
}

// int main(){
//     string n;
//     cin>>n;
//     Lexer lex;
//     lex.readFile(n);
//     while(!lex.isEndFile()){
//         cout << lex.getCurrent();
//         lex.advance();
//     }
// }
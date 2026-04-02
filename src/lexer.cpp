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

Token Lexer::getNextToken(DFA DFA){
    skipWhitespace();
    if(EOP) return Token("EOF", false, line);

    string lexeme = "";
    string currentState = DFA.getStartState();
    transition machine = DFA.getTransition();
    while(!isspace(currentChar) && !EOP){
        currentState = machine.getNextState(currentState, DFA.getDeadState(), currentChar);
        if(DFA.isDeadState(currentState)) break;
        lexeme += currentState;
        advance();
    }

    bool isValid = DFA.isFinalState(currentState);
    return Token(currentState, isValid, line);
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
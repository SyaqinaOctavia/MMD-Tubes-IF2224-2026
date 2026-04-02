#include "lexer.hpp"

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
    advance();
}

void Lexer::skipWhitespace(){
    while(isspace(currentChar)){
        advance();
    }
}

Lexer::Lexer() : EOP(false) {}

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
#include "lexer.hpp"

void lexer::advance(){
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

void lexer::readFile(string filepath){
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

void lexer::skipWhitespace(){
    while(isspace(currentChar)){
        advance();
    }
}

// int main(){
//     string n;
//     cin >> n;
//     readFile(n);
//     while(!EOP){
//         cout << currentChar;
//         advance();
//     }
// }
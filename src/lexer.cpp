#include <iostream>
#include <fstream>
#include <string>
using namespace std;

char currentChar;
bool EOP;
ifstream scanner;

void readChar(){
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

void readFile(string filepath){
    scanner.open(filepath);
    if(!scanner){
        cout << "Gagal membuka file pada path " << filepath << endl;
        EOP = true;
        currentChar = EOF;
        scanner.close();
        return;
    }
    readChar();
}

// int main(){
//     string n;
//     cin >> n;
//     readFile(n);
//     while(!EOP){
//         cout << currentChar;
//         readChar();
//     }
// }
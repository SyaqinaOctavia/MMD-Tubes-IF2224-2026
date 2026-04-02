#ifndef TOKEN_H
#define TOKEN_H
#include <string>
using namespace std;

class Token {
    private:
        string type;
        string value;
        bool valid;
        int line;
    public:
        Token(string value, bool valid, int line);
};

Token::Token(string value, bool valid, int line) : valid(valid), line(line) {
    if(value.substr(0, 2) == "q_"){

    } 
    else this->value = value;
}

#endif
#ifndef TOKEN_H
#define TOKEN_H
#include <string>
using namespace std;

class Token {
    private:
        string value;
        string lexeme;
        bool valid;
        int line;
    public:
        Token(string value, bool valid, int line);
};

Token::Token(string value, bool valid, int line) : value(value), valid(valid), line(line) {}

#endif
#ifndef TOKEN_H
#define TOKEN_H
#include <string>
using namespace std;

class Token {
    private:
        string state;
        string lexeme;
        int line;
    public:
        Token(string state, string lexeme, int line);
};

Token::Token(string state, string lexeme, int line) : state(state), lexeme(lexeme), line(line) {}

#endif
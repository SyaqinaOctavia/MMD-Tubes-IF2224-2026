#ifndef TOKEN_H
#define TOKEN_H
#include <string>
using namespace std;

class Token {
    private:
        string value;
        int line;
    public:
        Token(string value, int line) : value(value), line(line) {};
};

#endif
#include "Token.hpp"
#include <fstream>
#include <iostream>

std::string extractValue(const std::string& text) {
    size_t start = text.find('(');
    size_t end = text.rfind(')');

    if (start == std::string::npos ||
        end == std::string::npos ||
        start >= end) {
        return "";
    }

    return text.substr(start + 1, end - start - 1);
}

std::vector<Token> Token::readTokens(std::string filepath){
    std::ifstream file(filepath);
    std::vector<Token> tokens;
    if(!file.is_open()){
        std::cout << "File is invalid!";
        return tokens;
    }
    std::string line;
    int lineCount = 0;
    while(std::getline(file, line)){
        lineCount++;
            
        if(!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        auto it = Token::keywordMap.find(line);
        if(it == keywordMap.end()){
            if(line.find("ident") == 0){
                std::string value = extractValue(line);
                it = Token::keywordMap.find(line.substr(0, 5));
                tokens.push_back(Token(it->second, value, lineCount));
            }
            else if(line.find("intcon") == 0){
                std::string value = extractValue(line);
                it = Token::keywordMap.find(line.substr(0, 6));
                tokens.push_back(Token(it->second, value, lineCount));
            }
            else if(line.find("realcon") == 0){
                std::string value = extractValue(line);
                it = Token::keywordMap.find(line.substr(0, 7));
                tokens.push_back(Token(it->second, value, lineCount));
            } 
            else if(line.find("string") == 0){
                std::string value = extractValue(line);
                it = Token::keywordMap.find(line.substr(0, 6));
                tokens.push_back(Token(it->second, value, lineCount));
            } 
            else if(line.find("charcon") == 0){
                std::string value = extractValue(line);
                it = Token::keywordMap.find(line.substr(0, 7));
                tokens.push_back(Token(it->second, value, lineCount));
            } 
            else if(line.find("comment") == 0){
                std::string value = extractValue(line);
                it = Token::keywordMap.find(line.substr(0, 7));
                tokens.push_back(Token(it->second, value, lineCount));
            } 
            else{
                std::cout << "Warn: Invalid token detected [" << line << "]" << std::endl;
            }
        }
        else tokens.push_back(Token(it->second, lineCount));
    }

    return tokens;
}
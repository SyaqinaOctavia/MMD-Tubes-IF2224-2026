#include "ParseTree.hpp"
#include <iostream>
#include <sstream>
#include <cctype>
using namespace std;

void TreeNode::addChild(std::shared_ptr<TreeNode> newNode){
    children.push_back(newNode);
}

bool TreeNode::outputTree(std::string destFile) const {
    std::ofstream output(destFile);
    if(!output){
        cerr << "Failed to open output file: " << destFile << endl;
        return false;
    }
    return printNode(output, 0);
}

bool TreeNode::printNode(std::ofstream& output, int depth) const {
    for(int i = 0; i < depth; i++){
        if(i == depth - 1){
            output << "├─ ";
        }
        else{
            output << "│  ";
        }
    }
    output << toString(this->nodeType);
    if(isToken(this->nodeType) && value != ""){
        output << "(" << value << ")";
    } 
    output << std::endl;
    for(const auto& node : children){
        node->printNode(output, depth+1);
    }
    return true;
}
std::shared_ptr<TreeNode> TreeNode::readTreeFromFile(std::string sourceFile){
    std::shared_ptr<TreeNode> root;

    std::ifstream file(sourceFile);
    if(!file.is_open()) return root;
    std::string line;
    auto is_space_char = [](char c){
        return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
    };
    const std::string branchPipe = "│  ";
    const std::string branchLast = "├─ ";
    const std::string branchAltLast = "└─ ";
    std::vector<std::shared_ptr<TreeNode>> stack;

    while (std::getline(file, line)) {
        if(line.size() == 0) continue;
        if (line.size() >= 3 &&
            static_cast<unsigned char>(line[0]) == 0xEF &&
            static_cast<unsigned char>(line[1]) == 0xBB &&
            static_cast<unsigned char>(line[2]) == 0xBF) {
            line.erase(0, 3);
        }
        // Determine depth 
        size_t pos = 0;
        int depth = 0;
        while (pos + branchPipe.size() <= line.size()) {
            if (line.compare(pos, branchPipe.size(), branchPipe) == 0) {
                depth++;
                pos += branchPipe.size();
            } else if (line.compare(pos, branchLast.size(), branchLast) == 0) {
                depth++;
                pos += branchLast.size();
            } else if (line.compare(pos, branchAltLast.size(), branchAltLast) == 0) {
                depth++;
                pos += branchAltLast.size();
            } else break;
        }
        // Pick the Name and value
        std::string rest = line.substr(pos);

        while(!rest.empty() && is_space_char(rest.front())) rest.erase(rest.begin());
        while(!rest.empty() && is_space_char(rest.back())) rest.pop_back();
        // Extract 
        std::string name = rest;
        std::string value = "";
        size_t p = rest.find('(');
        if (p != std::string::npos) {
            size_t q = rest.find(')', p+1);
            if (q != std::string::npos) {
                name = rest.substr(0, p);
                value = rest.substr(p+1, q - p - 1);
            }
        }
        while(!name.empty() && is_space_char(name.front())) name.erase(name.begin());
        while(!name.empty() && is_space_char(name.back())) name.pop_back();
        // Make the parseTree node
        Symbol sym = toSymbol(name);
        std::shared_ptr<TreeNode> node = std::make_shared<TreeNode>(sym);

        if (!value.empty()) node->setValue(value);

        if (depth == 0) {
            root = node;
            stack.clear();
            stack.push_back(node);
        } else {
            if (stack.size() >= depth) {
                stack.resize(depth);
            }
            if (!stack.empty()) {
                stack.back()->addChild(node);
            }
            stack.push_back(node);
        }
    }

    return root;
}
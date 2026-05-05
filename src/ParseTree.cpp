#include "ParseTree.hpp"
#include <iostream>
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
    for(const auto& node : children){
        node->printNode(output, depth+1);
    }
    return true;
}
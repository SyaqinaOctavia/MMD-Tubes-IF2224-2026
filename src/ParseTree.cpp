#include "ParseTree.hpp"

void TreeNode::addChild(std::shared_ptr<TreeNode> newNode){
    children.push_back(newNode);
}
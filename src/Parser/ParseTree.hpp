#pragma once
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include "../Symbol.hpp"

class TreeNode {
    private:
        Symbol nodeType;
        std::string value;
        std::vector<std::shared_ptr<TreeNode>> children;
    public:
        TreeNode(Symbol node) : nodeType(node){}
        TreeNode(Symbol node, std::string value) : nodeType(node), value(value){}
        Symbol getNodeType() const { return nodeType; }
        std::vector<std::shared_ptr<TreeNode>> getChildren() const { return children; }
        std::string getValue(){ return value; }
        void setValue(std::string value){ this->value = value; }
        void addChild(std::shared_ptr<TreeNode> newNode);
        static std::shared_ptr<TreeNode> readTreeFromFile(std::string sourceFile);
        bool outputTree(std::string destFile) const;
        bool printNode(std::ofstream& file, int depth) const;
};
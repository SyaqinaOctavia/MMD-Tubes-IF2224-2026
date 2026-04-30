#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Symbol.hpp"

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
        void addChild(std::shared_ptr<TreeNode> newNode);
        void outputTree(std::string destFile) const;
};
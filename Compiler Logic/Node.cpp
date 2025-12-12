#include "Node.h"

Node::Node(TokenRecord t){
	token = t;
	sibling = nullptr;
	children = vector<Node*>();
    shape="ellipse";
}
Node::~Node() {
    for (Node* child : children) {
        delete child;
    }
    children.clear();
    delete sibling;
    sibling = nullptr;
}
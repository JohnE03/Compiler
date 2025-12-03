#include "Node.h"
Node::Node() {
	leftChild = nullptr;
	rightChild = nullptr;
}

Node::Node(TokenRecord t){
	token = t;
	leftChild = nullptr;
	rightChild = nullptr;
}
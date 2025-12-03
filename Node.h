#pragma once
#include <string>
#include "Token.h"
using namespace std;

class Node{
	public:
		TokenRecord token;
		Node* leftChild;
		Node* rightChild;

		Node();
		Node(TokenRecord t);
};


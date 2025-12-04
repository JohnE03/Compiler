#pragma once
#include <string>
#include <vector>
#include "Token.h"
using namespace std;

class Node{
	public:
		TokenRecord token;
		Node* sibling;
		vector<Node*> children;

		Node(TokenRecord t);
};

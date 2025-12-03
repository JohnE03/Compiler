#pragma once
#include <string>
using namespace std;

typedef enum
{
	SEMICOLON, IF, THEN, ELSE, END,
	REPEAT, UNTIL,
	IDENTIFIER, ASSIGN,
	READ, WRITE,
	LESSTHAN, EQUAL,
	PLUS, MINUS, MULT, DIV,
	OPENBRACKET, CLOSEDBRACKET,
	NUMBER
} TokenType;

typedef struct
{
	TokenType type;
	string stringVal;
	int numVal;
} TokenRecord;

class Node{
	public:
		TokenRecord token;
		Node* leftChild;
		Node* rightChild;

		Node();
		Node(TokenRecord t);
};


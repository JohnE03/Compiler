#pragma once
#include "Node.h"
#include <vector>
using namespace std;
class Parser{
private:
	int index = 0;
	vector<TokenRecord> tokens;

	TokenRecord getCurrentToken();
	void advance();
	bool match(TokenType expected);

	Node* program();
	Node* stmt_sequence();
	Node* statement();
	Node* if_stmt();
	Node* repeat_stmt();
	Node* assign_stmt();
	Node* read_stmt();
	Node* write_stmt();
	Node* exp();
	Node* simple_exp();
	Node* term();
	Node* factor();

public:
	Node* parse( vector<TokenRecord>& t );
};
vector<TokenRecord> readTokensFromString(string &s);


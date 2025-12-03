#include "Parser.h"

TokenRecord Parser::getCurrentToken(){
	return tokens[index];
}
void Parser::advance(){
	if(index < tokens.size()-1)
		index++;
}
bool Parser::match(TokenType expected){
	if(getCurrentToken().type == expected){
		advance();
		return true;
	}
	return false;
}

Node* Parser::program(){
	return stmt_sequence();
}
Node* Parser::stmt_sequence(){
	Node* stmtSeqNode = new Node(TokenRecord{});
	stmtSeqNode->children.push_back(statement());
	while (match(SEMICOLON)){
		Node* semiColon = new Node(TokenRecord{SEMICOLON,";"});
		semiColon->children.push_back(statement());
		stmtSeqNode->children.push_back(semiColon);
	}
	return stmtSeqNode;
}
Node* Parser::statement(){
	// Implementation goes here
	return nullptr;
}
Node* Parser::if_stmt(){
	// Implementation goes here
	return nullptr;
}
Node* Parser::repeat_stmt(){
	// Implementation goes here
	return nullptr;
}
Node* Parser::assign_stmt(){
	// Implementation goes here
	return nullptr;
}
Node* Parser::read_stmt(){
	// Implementation goes here
	return nullptr;
}
Node* Parser::write_stmt(){
	// Implementation goes here
	return nullptr;
}
Node* Parser::exp(){
	// Implementation goes here
	return nullptr;
}
Node* Parser::comparison_op(){
	// Implementation goes here
	return nullptr;
}
Node* Parser::simple_exp(){
	// Implementation goes here
	return nullptr;
}
Node* Parser::addop(){
	// Implementation goes here
	return nullptr;
}
Node* Parser::term(){
	// Implementation goes here
	return nullptr;
}
Node* Parser::mulop(){
	// Implementation goes here
	return nullptr;
}
Node* Parser::factor(){
	// Implementation goes here
	return nullptr;
}


Node* Parser::parse(vector<TokenRecord> tokens) {
	return program();
}
#include "Parser.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
using namespace std;


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
	Node* stmtSequenceNode = nullptr;
	Node* last = nullptr;
	int size = tokens.size() - 1;
	while (index < size) {
		if (!stmtSequenceNode) {
			stmtSequenceNode = statement();
			last = stmtSequenceNode;
		}
		else {
			last->sibling = statement();
			last = last->sibling;
		}
		
		if(getCurrentToken().type == SEMICOLON) {
			match(SEMICOLON);
		}

		TokenType t = getCurrentToken().type;
		if (t == END || t == ELSE || t == UNTIL)
			break;
	}
	return stmtSequenceNode;
}

Node* Parser::statement(){
	switch(getCurrentToken().type) {
		case IF:
			return if_stmt();
		case REPEAT:
			return repeat_stmt();
		case IDENTIFIER:
			return assign_stmt();
		case READ:
			return read_stmt();
		case WRITE:
			return write_stmt();
		default:
			cout << "Unexpected token in statement on line: " << index;
			throw runtime_error("Unexpected token in statement on line: " + to_string(index));
	}
}

Node* Parser::if_stmt(){
	Node* ifNode = new Node(getCurrentToken());
	ifNode->shape = "rectangle";
	if (match(IF)) {
		Node* conditionNode = exp();
		conditionNode->shape = "ellipse";
		ifNode->children.push_back(conditionNode);
		if (getCurrentToken().type==THEN) {
			Node* thenNode = new Node(getCurrentToken());
			thenNode->shape = "ellipse";
			match(THEN);

            //changed
			//thenNode->children.push_back(stmt_sequence());
			ifNode->children.push_back(stmt_sequence());

			if (getCurrentToken().type==ELSE) {
				Node* elseNode = new Node(getCurrentToken());
				elseNode->shape = "ellipse";
				match(ELSE);

                //changed
				//elseNode->children.push_back(stmt_sequence());
				ifNode->children.push_back(stmt_sequence());
			}
			if (getCurrentToken().type==END) {

                //changed
				//Node* endNode = new Node(getCurrentToken());
				//ifNode->children.push_back(endNode);
				match(END);

				return ifNode;
			}
			else {
				cout << "Expected 'end' token on line: " << index;
				throw runtime_error("Expected 'end' token on line: " + to_string(index));
			}
		}
		else {
			cout << "Expected 'then' token on line: " << index;
			throw runtime_error("Expected 'then' token on line: " + to_string(index));
		}
	}
	return nullptr;
}

Node* Parser::repeat_stmt(){
	Node* repeatNode = new Node(getCurrentToken());
	repeatNode->shape = "rectangle";
	if(match(REPEAT)){
		Node* repeatBodyNode = stmt_sequence();

		repeatNode->children.push_back(repeatBodyNode);
		if(getCurrentToken().type==UNTIL){
			Node* untilNode = new Node(getCurrentToken());
			match(UNTIL);

            //changed
			//untilNode->children.push_back(exp());
            repeatNode->children.push_back(exp());

			return repeatNode;
		}
		else {
			cout << "Expected 'until' token on line: " << index;
			throw runtime_error("Expected 'until' token on line: " + to_string(index));
		}
	}
	else {
		return nullptr;
	}
}

Node* Parser::assign_stmt(){//assign(ID),expr
	Node* assignNodeId = new Node(getCurrentToken());
	Node* assignNode = new Node(TokenRecord{ASSIGN, "assign"});
	assignNode->shape = "rectangle";
	if (match(IDENTIFIER)) {
		if(getCurrentToken().type==ASSIGN){
			match(ASSIGN);
			assignNode->children.push_back(assignNodeId);
			Node* exprNode = exp();

			assignNode->children.push_back(exprNode);
			return assignNode;
		}
		else {
			cout << "Expected ':=' token on line: " << index;
			throw runtime_error("Expected ':=' token on line: " + to_string(index));
		}
	}
	return nullptr;
}

Node* Parser::read_stmt(){
	Node* readNode = new Node(getCurrentToken());
	readNode->shape = "rectangle";
	if (match(READ)) {
		if (getCurrentToken().type!=IDENTIFIER) {
			cout << "Expected identifier after 'read' on line: " << index;
			throw runtime_error("Expected identifier after 'read' on line: " + to_string(index));
		} else {
			readNode->children.push_back(new Node(getCurrentToken()));
			match(IDENTIFIER);
			return readNode;
		}
	}
	return nullptr;
}

Node* Parser::write_stmt(){
	Node* writeNode = new Node(getCurrentToken());
	writeNode->shape = "rectangle";
	if (match(WRITE)) {
		Node* expNode = exp();

		writeNode->children.push_back(expNode);
		return writeNode;
	}
	return nullptr;
}

Node* Parser::exp(){
	Node* expNode = simple_exp();
	if(getCurrentToken().type == LESSTHAN || getCurrentToken().type == EQUAL) {
		Node* compOpNode = new Node(getCurrentToken());
		advance();
		compOpNode->children.push_back(expNode);
		compOpNode->children.push_back(simple_exp());
		expNode = compOpNode;
	}
	return expNode;
}

Node* Parser::simple_exp(){
	Node* simpleExpNode = term();
	while (getCurrentToken().type == PLUS || getCurrentToken().type == MINUS) {
		Node* addOpNode = new Node(getCurrentToken());
		advance();
		addOpNode->children.push_back(simpleExpNode);
		addOpNode->children.push_back(term());
		simpleExpNode = addOpNode;
	}
	return simpleExpNode;
}

Node* Parser::term(){
	Node* termNode = factor();
	while(getCurrentToken().type == MULT || getCurrentToken().type == DIV) {
		Node* mulOpNode = new Node(getCurrentToken());
		advance();
		mulOpNode->children.push_back(termNode);
		mulOpNode->children.push_back(factor());
		termNode = mulOpNode;
	}
	return termNode;
}

Node* Parser::factor(){
	Node* factorNode;
	if (match(OPENBRACKET)) {
		factorNode = exp();
		if(match(CLOSEDBRACKET)){
			return factorNode;
		} else {
			cout << "Expected closing bracket on line: " << index;
			throw runtime_error("Expected closing bracket on line: " + to_string(index));
		}
	}
	else if (getCurrentToken().type == NUMBER || getCurrentToken().type==IDENTIFIER) {

		factorNode = new Node(getCurrentToken());
		advance();
		return factorNode;
	}
	else {
		cout << "Unexpected token in factor on line: " << index;
		throw runtime_error("Unexpected token in factor on line: " + to_string(index));
	}
}

Node* Parser::parse( vector<TokenRecord>& t) {
	tokens = t;
	index=0;
	return program();
}



TokenType getTokenTypeFromName(const string& name) {
    for (int i = 0; i < sizeof(TokenNames) / sizeof(TokenNames[0]); i++) {
        if (TokenNames[i] == name) return static_cast<TokenType>(i);
    }
    throw runtime_error("Unknown token type: " + name);
}

vector<TokenRecord> readTokensFromString(string &s) {
//    string location = "";
//    cout << "Enter the file location: ";
//    cin >> location;
//
//    ifstream readFile(location);
//    if (!readFile.is_open()) {
//        cerr << "Failed to open file: " << location << endl;
//        return {};
//    }
    vector <TokenRecord> tokens;
    stringstream in(s);

    string line;

    while (getline(in, line)) {
        int commaPos = line.find(',');
        string tokenStr = line.substr(0, commaPos);
        string tokenType = line.substr(commaPos + 1);
        TokenRecord tr;

        tokenStr.erase(0, tokenStr.find_first_not_of(" \t\n\r"));
        tokenStr.erase(tokenStr.find_last_not_of(" \t\n\r") + 1);
        tokenType.erase(0, tokenType.find_first_not_of(" \t\n\r"));
        tokenType.erase(tokenType.find_last_not_of(" \t\n\r") + 1);


        tr.type = getTokenTypeFromName(tokenType);
        if(tr.type==ASSIGN)
        {
            tr.stringVal="assign";

        }
        else if(tr.type==PLUS)
        {
            tr.stringVal="op (+)";
        }
        else if(tr.type==MINUS)
        {
            tr.stringVal="op (-)";
        }
        else if(tr.type==MULT)
        {
            tr.stringVal="op (*)";
        }
        else if(tr.type==DIV)
        {
            tr.stringVal="op (/)";
        }
        else if(tr.type==LESSTHAN)
        {
            tr.stringVal="op (<)";
        }
        else if(tr.type==EQUAL)
        {
            tr.stringVal="op (=)";
        }

        else
        {
            tr.stringVal=tokenStr;
        }
        tokens.push_back(tr);
    }
    return tokens;
}
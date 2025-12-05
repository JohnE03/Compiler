#pragma once
#include <string>
#include <vector>
using namespace std;

typedef enum{
    PROGRAM,
    SEMICOLON, IF, THEN, ELSE, END,
    REPEAT, UNTIL,
    IDENTIFIER, ASSIGN,
    READ, WRITE,
    LESSTHAN, EQUAL,
    PLUS, MINUS, MULT, DIV,
    OPENBRACKET, CLOSEDBRACKET,
    NUMBER
} TokenType;

const string TokenNames[] = {
	"PROGRAM",
    "SEMICOLON", "IF", "THEN", "ELSE", "END",
    "REPEAT", "UNTIL",
    "IDENTIFIER", "ASSIGN",
    "READ", "WRITE",
    "LESSTHAN", "EQUAL",
    "PLUS", "MINUS", "MULT", "DIV",
    "OPENBRACKET", "CLOSEDBRACKET",
    "NUMBER"
};

struct TokenRecord{
    TokenType type;
    string stringVal;
};

extern vector<TokenRecord> tokens;
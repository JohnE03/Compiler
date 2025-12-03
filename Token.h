#pragma once
#include <string>
#include <vector>
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

const string TokenNames[] = {
    "SEMICOLON", "IF", "THEN", "ELSE", "END",
    "REPEAT", "UNTIL",
    "IDENTIFIER", "ASSIGN",
    "READ", "WRITE",
    "LESSTHAN", "EQUAL",
    "PLUS", "MINUS", "MULT", "DIV",
    "OPENBRACKET", "CLOSEDBRACKET",
    "NUMBER"
};

typedef struct
{
    TokenType type;
    string stringVal;
    int numVal;
} TokenRecord;

extern vector<TokenRecord> tokens;
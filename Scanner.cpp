#include <iostream>
#include <fstream>
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

vector<TokenRecord> tokens;

int main() {
    TokenRecord token{};

    string location = "";
	cout << "Enter the code file location: ";
	cin >> location;
    ifstream readFile(location);
	cout << "Enter output file location (default: output.txt): ";
	cin >> location;
    ofstream writeFile(location);

	int lineNumber = 0;
    string line, identifier, number;
    bool inComment = false, inString = false;
    bool err=0;
    while (getline(readFile, line)) {
		lineNumber++;
        for (int i=0; i < line.length();i++) {
            if (!inString) identifier = "";
            number = "";
			char ch = line[i];

            if (inComment) { //comments
                if (ch == '}') inComment = false;
                continue;
            }
            if (ch == '{') { inComment = true; continue; }

            if(inString) { //strings
                if (ch == '\"') {
                    inString = false;
                    token.type = IDENTIFIER;
                    token.stringVal = identifier;
					tokens.push_back(token);
                    writeFile << identifier << ", " << "IDENTIFIER" << endl;
                }
                else {
                    identifier += ch;
                }
                continue;
			}
            if (ch == '\"') { inString = true; continue; }

            if(ch==' ' || ch == '\t') continue;//whitespaces

            switch (ch) {
            case ';':
                token.type = SEMICOLON;
                token.stringVal = ch;
                tokens.push_back(token);
                writeFile << ch << ", " << "SEMICOLON" << endl;
                continue;
            case '+':
                token.type = PLUS;
                token.stringVal = ch;
                tokens.push_back(token);
                writeFile << ch << ", " << "PLUS" << endl;
                    continue;
            case '-':
                token.type = MINUS;
                token.stringVal = ch;
                tokens.push_back(token);
                writeFile << ch << ", " << "MINUS" << endl;
                    continue;
            case '*':
                token.type = MULT;
                token.stringVal = ch;
                tokens.push_back(token);
                writeFile << ch << ", " << "MULT" << endl;
                    continue;
            case '/':
                token.type = DIV;
                token.stringVal = ch;
                tokens.push_back(token);
                writeFile << ch << ", " << "DIV" << endl;
                    continue;
            case '<':
                token.type = LESSTHAN;
                token.stringVal = ch;
                tokens.push_back(token);
                writeFile << ch << ", " << "LESSTHAN" << endl;
                    continue;
            case '=':
                token.type = EQUAL;
                token.stringVal = ch;
                tokens.push_back(token);
                writeFile << ch << ", " << "EQUAL" << endl;
                    continue;
            case '(':
                token.type = OPENBRACKET;
                token.stringVal = ch;
                tokens.push_back(token);
                writeFile << ch << ", " << "OPENBRACKET" << endl;
                    continue;
            case ')':
                token.type = CLOSEDBRACKET;
                token.stringVal = ch;
                tokens.push_back(token);
                writeFile << ch << ", " << "CLOSEDBRACKET" << endl;
                    continue;
            }

            if (ch == ':' && line[i + 1] == '=') {
                token.type = ASSIGN;
                token.stringVal = ":=";
                tokens.push_back(token);
                writeFile << ":=" << ", " << "ASSIGN" << endl;
				i = i + 1;
                continue;
            }

            if (isalpha(ch)) {
                if(ch=='i' && line[i + 1] == 'f'){
                    token.type = IF;
                    token.stringVal = "if";
                    tokens.push_back(token);
                    writeFile << "if" << ", " << "IF" << endl;
                    i = i + 1;
                    continue;
				}
                if (ch == 't' && line[i + 1] == 'h' && line[i + 2] == 'e' && line[i + 3] == 'n') {
                    token.type = THEN;
                    token.stringVal = "then";
                    tokens.push_back(token);
                    writeFile << "then" << ", " << "THEN" << endl;
                    i = i + 3;
                    continue;
                }
                if(ch == 'e' && line[i + 1] == 'l' && line[i + 2] == 's' && line[i + 3] == 'e') {
                    token.type = ELSE;
                    token.stringVal = "else";
                    tokens.push_back(token);
                    writeFile << "else" << ", " << "ELSE" << endl;
                    i = i + 3;
					continue;
				}
                if (ch == 'e' && line[i + 1] == 'n' && line[i + 2] == 'd') {
                    token.type = END;
                    token.stringVal = "end";
                    tokens.push_back(token);
                    writeFile << "end" << ", " << "END" << endl;
                    i = i + 2;
                    continue;
				}
                if (ch == 'r' && line[i + 1] == 'e' && line[i + 2] == 'p' && line[i + 3] == 'e' && line[i + 4] == 'a' && line[i + 5] == 't') {
                    token.type = REPEAT;
                    token.stringVal = "repeat";
                    tokens.push_back(token);
                    writeFile << "repeat" << ", " << "REPEAT" << endl;
                    i = i + 5;
                    continue;
                }
                if (ch == 'u' && line[i + 1] == 'n' && line[i + 2] == 't' && line[i + 3] == 'i' && line[i + 4] == 'l') {
                    token.type = UNTIL;
                    token.stringVal = "until";
                    tokens.push_back(token);
                    writeFile << "until" << ", " << "UNTIL" << endl;
                    i = i + 4;
                    continue;
				}
                if (ch == 'r' && line[i + 1] == 'e' && line[i + 2] == 'a' && line[i + 3] == 'd') {
                    token.type = READ;
                    token.stringVal = "read";
                    tokens.push_back(token);
                    writeFile << "read" << ", " << "READ" << endl;
                    i = i + 3;
                    continue;
                }
                if (ch == 'w' && line[i + 1] == 'r' && line[i + 2] == 'i' && line[i + 3] == 't' && line[i + 4] == 'e') {
                    token.type = WRITE;
                    token.stringVal = "write";
                    tokens.push_back(token);
                    writeFile << "write" << ", " << "WRITE" << endl;
                    i = i + 4;
                    continue;
				}
                identifier += ch;
                size_t pos = i + 1;
                while (pos < line.length() && isalnum(line[pos])) {
                    identifier += line[pos];
                    pos++;
                }
                token.type = IDENTIFIER;
                token.stringVal = identifier;
                tokens.push_back(token);
                writeFile << identifier << ", " << "IDENTIFIER" << endl;
				i = i + identifier.length() - 1;
                continue;
			}
            if(isdigit(ch)) {
                number += ch;
                size_t pos = i + 1;
                while (pos < line.length() && isdigit(line[pos])) {

                    //if(!isdigit(line[pos]))  //invalid number
                    //{
                    //    err=1;
                    //}

                    number += line[pos];
                    pos++;
                }
                if(err)
                {
                    writeFile <<"ERROR INVALID IDENTIFIER: " <<number  << " on line: " << lineNumber << endl;
                    cout << "ERROR INVALID IDENTIFIER: " << number << " on line: " << lineNumber << endl;
                    break;
                }

                token.type = NUMBER;
                token.numVal = stoi(number);
                tokens.push_back(token);
                writeFile << number << ", " << "NUMBER" << endl;
				i = i + number.length() - 1;
				continue;
			}
            // If we reach here, it's an error

            writeFile <<"ERROR: unexpected character -> "<< ch << " on line: " << lineNumber << endl;
            cout << "ERROR: unexpected character -> " << ch << " on line: " << lineNumber << endl;
            err=1;
            break;

        }
        if (err) break;
    }
	writeFile.close();
    string loc = "start " + location;
    system(loc.c_str());
}
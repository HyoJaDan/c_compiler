#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <fstream>
#define MAXLEN 256
using namespace std;

int current = 0;
int num;
string inputedString="";
int inputedStringLength = 0;
string token;
enum {IDENFR, INTCON, STRCON, MAINTK, CONSTTK, INTTK, BREAKTK, CONTINUETK, IFTK, ELSETK, NOT, AND, OR, FORTK, GETINTTK, PRINTFTK, RETURNTK, PLUS, MINU, VOIDTK, MULT, DIV, MOD, LSS, LEQ, GRE, GEQ, EQL, NEQ, ASSIGN, SEMICN, COMMA, LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE}symbol;

string symbolCategory[50] = {"IDENFR", "INTCON", "STRCON", "MAINTK", "CONSTTK", "INTTK", "BREAKTK", "CONTINUETK", "IFTK", "ELSETK", "NOT", "AND", "OR", "FORTK", "GETINTTK", "PRINTFTK", "RETURNTK", "PLUS", "MINU", "VOIDTK", "MULT", "DIV", "MOD", "LSS", "LEQ", "GRE", "GEQ", "EQL", "NEQ", "ASSIGN", "SEMICN", "COMMA", "LPARENT", "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE"};

bool startsWithNewLine() {
    string temp = inputedString.substr(current, 1);
    if(temp=="\n"||temp=="\r"||temp=="\t")
        return true;
    return false;
}

void skip() {
    while(inputedString[current]==' ' ||startsWithNewLine()|| inputedString[current]=='	') {
        current++;
    }
    return;
}

void removeLineComment(){
    while(inputedString[current]!='\n')
        current++;
}
void removeBlockComment(){
    while(current<inputedStringLength){
        if(inputedString[current]=='*'&&inputedString[current+1]=='/')
            break;
        current++;
    }
    current += 2;
}

int checkReserver() {
	if (token=="main") {
		symbol = MAINTK;
	}else if (token=="const") {
		symbol = CONSTTK;
	}else if (token=="int") {
		symbol = INTTK;
	}else if (token=="break") {
		symbol = BREAKTK;
	}else if (token=="continue") {
		symbol = CONTINUETK;
	}else if (token=="if") {
		symbol = IFTK;
	}else if (token=="else") {
		symbol = ELSETK;
	}else if (token=="for") {
		symbol = FORTK;
	}else if (token=="getint") {
		symbol = GETINTTK;
	}else if (token=="printf") {
		symbol = PRINTFTK;
	}else if (token=="return") {
		symbol = RETURNTK;
	}else if (token=="void") {
		symbol = VOIDTK;
	}else {
		return -1;
	}
	return 0;
}

int getsym() {
	skip();
    token = "";
    int flag = false;
    if (isalpha(inputedString[current])) {
		while(isalpha(inputedString[current]) || isdigit(inputedString[current])|| inputedString[current]=='_') {
            token += inputedString[current];
            current++;
        }
		if (checkReserver() == -1) {
			symbol = IDENFR;
		}
        flag = true;
    }
    else if (isdigit(inputedString[current]))
    {
        num = 0;
		while(isdigit(inputedString[current])) {
            token += inputedString[current];
            current++;
        }
        num = stoi(token);
		symbol = INTCON;
        flag = true;
    }
    else if (inputedString[current] == '+')
    {
        symbol = PLUS;
        token = "+";
        current++;
        flag = true;
    }
    else if (inputedString[current] == '-')
    {
        symbol = MINU;
        token = "-";
        current++;
        flag = true;
    }
    else if (inputedString[current] == '*')
    {
        symbol = MULT;
        token = "*";
        current++;
        flag = true;
    }
    else if (inputedString[current] == '/')
    {
        if(inputedString[current+1]=='/'){
            removeLineComment();
            flag = 2;
        }
        else if(inputedString[current+1]=='*')
        {
            removeBlockComment();
            flag = 2;
        }
        else{
            symbol = DIV;
            token = '/';
            current++;
            flag = true;
        }
    }
    else if (inputedString[current] == '%')
    {
        symbol = MOD;token = "%";current++;flag = true;
    }
    else if (inputedString[current] == '<')
    {
        if(inputedString[current+1]=='=')
        {
            symbol = LEQ;
            token = "<=";
            current++;
        }
        else
        {
            symbol = LSS;
            token = "<";
        }
        current++;
        flag = true;
    }
    else if (inputedString[current] =='>')
    {
        if(inputedString[current+1]=='=')
        {
            symbol = GEQ;
            token = ">=";
            current++;
        }
        else
        {
            symbol = GRE;
            token = ">";
        }
        current++;
        flag = true;
    }else if (inputedString[current] =='=')
    {
        if(inputedString[current+1]=='=')
        {
            symbol = EQL;
            token = "==";
            current++;
        }
        else
        {
            symbol = ASSIGN;
            token = "=";
        }
        current++;
        flag = true;
    }else if (inputedString[current] =='!')
    {
        if(inputedString[current+1]=='=')
        {
            symbol = NEQ;
            current++;
            token = "!=";
        }
        else
        {
            symbol = NOT;
            token = "!";
        }
        current++;
        flag = true;
    }else if (inputedString[current]=='(') {
		symbol = LPARENT;
        token = "(";
        current++;
        flag = true;
	}else if (inputedString[current]==')') {
		symbol = RPARENT;
        token = ")";
        current++;
        flag = true;
	}else if (inputedString[current]==',') {
		symbol = COMMA;
        token = ",";
        current++;
        flag = true;
	}else if (inputedString[current]==';') {
		symbol = SEMICN;
        token = ";";
        current++;
        flag = true;
    }else if (inputedString[current]=='[') {
		symbol = LBRACK;
        token = "[";
        current++;
        flag = true;
	}else if (inputedString[current]==']') {
		symbol = RBRACK;
        token = "]";
        current++;
        flag = true;
	}else if (inputedString[current]=='{') {
		symbol = LBRACE;
        token = "{";
        current++;
        flag = true;
	}else if (inputedString[current]=='}') {
		symbol = RBRACE;
        token = "}";
        current++;
        flag = true;
	}else if(inputedString[current]=='&'&&inputedString[current+1]=='&'){
        symbol = AND;
        token = "&&";
        current+=2;
        flag = true;
    }else if(inputedString[current]=='|'&&inputedString[current+1]=='|'){
        symbol = OR;
        token = "||";
        current+=2;
        flag = true;
    }else if(inputedString[current]=='\"'){
        int endpos = inputedString.find('\"', current+1);
        token = inputedString.substr(current, endpos - current+1);
        current += endpos - current+1;
        flag = true;
        symbol = STRCON;
    }
    if(flag==true)
        return 0;
    else if(flag==2)
        return 2;
    else
        return -1;
    
}


int main(void) {
    FILE *inputFile;
    char inputString[100];
    inputFile = fopen("testfile.txt", "r");

    ofstream outputFile;
    outputFile.open("output.txt");
    while (fgets(inputString, sizeof(inputString), inputFile) != NULL)
    {
        inputedString += inputString;
    }
    inputedStringLength = inputedString.length();
    while (current != inputedStringLength)
    {
        int returnValue = getsym();
        if (returnValue == 0)
        {
            string str = symbolCategory[symbol] + " " + token+"\n";
            outputFile.write(str.c_str(), str.size());
        }
        else if(returnValue==2)
        {
            continue;
        }
        else if (returnValue==-1){
            break;
        }
        
    }
    fclose(inputFile);
    outputFile.close();

    return 0;
}

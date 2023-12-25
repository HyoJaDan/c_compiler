#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
using namespace std;

void parseConstDef();
void parseVarDef();
void parseFuncDef();
void parseStmt();
int parseExp();
void parseReadStmt();
void parsePrint();
void parseReturnStmt();
void parseMainFunc();
void parseBlock(bool isNeedReturn);
void parseFuncFParams();
int parseLVal(int type);
void parseFuncRParamsWithoutError();
void error(int lineId, string errType);
string toLowerCase(string in);
void levelUp();
void levelDown();
void parseErrorA(char ch, bool isAlreadyError);
bool parseErrorB(string idenfr, int lineID);
int parseErrorC(string idenfr, int lineID);
void parseErrorH(string idenfr, int lineID);
void parseErrorI();
void parseErrorJ();
void parseErrorK();

FILE *inputFile;
ifstream inFile("input.txt");
ofstream fout("output.txt");
ofstream errorOut("error.txt");

string token;
string inputedString="";

int inputedStringLength = 0;
int num;
int global_line;
/* l, printf오류를 위한 변수 */
int lastNonTerminalLine = 0;
/* m, break,comtinue가 순환이 아닌 변수에서 나올때를 위한 변수 */
bool isCirculer[100] = {false};
/* g, 함수에 return 이 없을때 를위한변수 */
bool isReturnCalled=false;

bool isCameFromParam = false;
int blocks[100000] = {0};
int blocksTop = 0;

struct Word
{
	string label;
	string idenfr;
	int lineId;
};
Word wordList[10000];
int top = 0;
int wordCnt = 0;
Word wordNow;

enum KindEnum {
	CONST,
	VAR,
	FUNC,
	PARAM
};

enum TypeEnum {
	INT,
	VOID,
	ARR1,
	ARR2
};
int currentFunctionType;

struct Symbol
{
	string idenfr;
	int kind;
	int type;
	int level;
};
Symbol symbolTable[10000];
int symbolTableTop = -1;
int levelNow = 0;

struct errorType
{
	int lineId;
	string errType;
} errorList[100];

struct FuncInfo
{
	int pos;
	int paramNum;
	int type;
	int paramPos[100];
};
FuncInfo getFuncInfo(string idenfr);
void parseFuncRParams(FuncInfo funcInfo);

void insertWordList(string label, string idenfr)
{
	wordList[top].label = label;
	wordList[top].idenfr = idenfr;
	wordList[top++].lineId = global_line;
}

void init()
{
    char inputString[500];
    inputFile = fopen("testfile.txt", "r");
	int line = 0;
	while (fgets(inputString, sizeof(inputString), inputFile) != NULL)
	{
		line++;
		string line_string=to_string(line);
		inputedString += "@!";
		inputedString += line_string;
		inputedString += inputString;
	}
    inputedStringLength = inputedString.length();
}

bool parsingAnnotation() {
	string temp = inputedString.substr(wordCnt, 1);
	if(temp=="\n"||temp=="\r"||temp=="\t")
		return true;
	return false;
}

void parseLineNumber(){
	if (inputedString[wordCnt] == '@' && inputedString[wordCnt + 1] == '!')
	{
		inputedString = inputedString.erase(wordCnt, 2);
		inputedStringLength -= 2;
		string temp = "";
		while (isdigit(inputedString[wordCnt]))
		{
			temp+=inputedString[wordCnt++];
		}
		wordCnt -= temp.length();
		inputedString = inputedString.erase(wordCnt, temp.length());
		inputedStringLength -= temp.length();
		global_line = stoi(temp);
	}
}

void skip() {
	parseLineNumber();
	while (inputedString[wordCnt] == ' ' || parsingAnnotation())
	{
		wordCnt++;
	}
}

void removeLineComment(){
	while(1)
	{
		string temp = inputedString.substr(wordCnt, 1);
		if(temp=="\n"||temp=="\r")
			break;
		wordCnt++;
	}
}
void removeBlockComment(){
	while(wordCnt<inputedStringLength)
	{
		if(inputedString[wordCnt]=='*'&&inputedString[wordCnt+1]=='/')
			break;
		wordCnt++;
	}
	if(wordCnt+2>=inputedStringLength)
		return;
	wordCnt += 2;
}

// 英语的部分
bool checkReserver() {
	if (token=="main")
		insertWordList("MAINTK", "main");
	else if (token == "const")
		insertWordList("CONSTTK", "const");
	else if (token == "int")
		insertWordList("INTTK", "int");
	else if (token == "break")
		insertWordList("BREAKTK", "break");
	else if (token == "continue")
		insertWordList("CONTINUETK", "continue");
	else if (token == "if")
		insertWordList("IFTK", "if");
	else if (token == "else")
		insertWordList("ELSETK", "else");
	else if (token == "for")
		insertWordList("FORTK", "for");
	else if (token == "getint")
		insertWordList("GETINTTK", "getint");
	else if (token == "printf")
		insertWordList("PRINTFTK", "printf");
	else if (token == "return")
		insertWordList("RETURNTK", "return");
	else if (token == "void")
		insertWordList("VOIDTK", "void");
	else
		return false;
	return true;
}

void Lexical_Analysis_Main() {
	skip();
	token = "";
	if (isalpha(inputedString[wordCnt])||inputedString[wordCnt]=='_') {
		while(isalpha(inputedString[wordCnt]) || isdigit(inputedString[wordCnt])|| inputedString[wordCnt]=='_') 
		{
			token += inputedString[wordCnt];
			wordCnt++;
		}
		if (!checkReserver()) {
			insertWordList("IDENFR", token);
		}
	}
     // if test has float
        /* bool isFloat = false;
        while(isdigit(inputedString[wordCnt])||inputedString[wordCnt]=='.') {
            if(inputedString[wordCnt]=='.')
                isFloat = true;
            token += inputedString[wordCnt];
            wordCnt++;
		}
        if(isFloat)
        {
            mapList[top][0] = "Float";
            mapList[top++][1] = token;
        }
        else{
            mapList[top][0] = "INTCON";
            mapList[top++][1] = token;
        } */
	else if (isdigit(inputedString[wordCnt]))
	{
		while(isdigit(inputedString[wordCnt])) {
			token += inputedString[wordCnt];
			wordCnt++;
		}
		insertWordList("INTCON", token);
	}
	else if (inputedString[wordCnt] == '+'){
		insertWordList("PLUS", "+");
		wordCnt++;
	}
	else if (inputedString[wordCnt] == '-')
	{
		insertWordList("MINU", "-");
		wordCnt++;
	}
	else if (inputedString[wordCnt] == '*')
	{
		insertWordList("MULT", "*");
		wordCnt++;
	}
	else if (inputedString[wordCnt] == '/')
	{
		if(inputedString[wordCnt+1]=='/'){
			removeLineComment();
		}
		else if(inputedString[wordCnt+1]=='*')
		{
			removeBlockComment();
		}
		else{
			insertWordList("DIV", "/");
			wordCnt++;
		}
	}
	else if (inputedString[wordCnt] == '%')
	{
		insertWordList("MOD", "%");
		wordCnt++;
	}
	else if (inputedString[wordCnt] == '<')
	{
		if(inputedString[wordCnt+1]=='=')
		{
			wordCnt++;
			insertWordList("LEQ", "<=");
		}
		else
		{
			insertWordList("LSS", "<");
		}
		wordCnt++;
	}
	else if (inputedString[wordCnt] =='>')
	{
		if(inputedString[wordCnt+1]=='=')
		{
			insertWordList("GEQ", ">=");
			wordCnt++;
		}
		else
		{
			insertWordList("GRE", ">");
		}
		wordCnt++;
	}else if (inputedString[wordCnt] =='=')
	{
		if(inputedString[wordCnt+1]=='=')
		{
			insertWordList("EQL", "==");
			wordCnt++;
		}
		else
		{
			insertWordList("ASSIGN", "=");
		}
		wordCnt++;
	}else if (inputedString[wordCnt] =='!')
	{
		if(inputedString[wordCnt+1]=='=')
		{
			insertWordList("NEQ", "!=");
			wordCnt++;
		}
		else
		{
			insertWordList("NOT", "!");
		}
		wordCnt++;
	}else if (inputedString[wordCnt]=='(') {
		insertWordList("LPARENT", "(");;
		wordCnt++;
	}else if (inputedString[wordCnt]==')') {
		insertWordList("RPARENT", ")");
		wordCnt++;
	}else if (inputedString[wordCnt]==',') {
		insertWordList("COMMA", ",");
		wordCnt++;
	}else if (inputedString[wordCnt]==';') {
		insertWordList("SEMICN", ";");
		wordCnt++;
	}else if (inputedString[wordCnt]=='[') {
		insertWordList("LBRACK", "[");
		wordCnt++;
	}else if (inputedString[wordCnt]==']') {
		insertWordList("RBRACK", "]");
		wordCnt++;
	}else if (inputedString[wordCnt]=='{') {
		insertWordList("LBRACE", "{");
		wordCnt++;
	}else if (inputedString[wordCnt]=='}') {
		insertWordList("RBRACE", "}");
		wordCnt++;
	}else if(inputedString[wordCnt]=='&'&&inputedString[wordCnt+1]=='&'){
		insertWordList("AND", "&&");
		wordCnt+=2;
	}else if(inputedString[wordCnt]=='|'&&inputedString[wordCnt+1]=='|'){
		insertWordList("OR", "||");
		wordCnt+=2;
	}else if(inputedString[wordCnt]=='\"'){
		token += inputedString[wordCnt++];
		bool isAlreadyError = false;
		while (inputedString[wordCnt] != '\"')
		{
			char ch = inputedString[wordCnt];
			parseErrorA(ch, isAlreadyError);
			token += inputedString[wordCnt];
			wordCnt++;
		}
		token += inputedString[wordCnt++];
		insertWordList("STRCON", token);
	}
}

void Lexical_Analysis()
{
	while (wordCnt != inputedStringLength)
		Lexical_Analysis_Main();

	// Lexical부분 출력
	/* for (int i = 0; i < top;i++)
	{
		fout << wordList[i].label << " " << wordList[i].idenfr << endl;
		cout << wordList[i].label << " " << wordList[i].idenfr << " " << wordList[i].lineId << endl;
	} */
}

void outputSyntax_Analysis(string input)
{
	cout << "<" << input << ">" << endl;
	fout << "<" << input << ">" << endl;
}

int Syntax_Analysis_Main(bool forward, bool withOutput = false)
{
	if (forward)
	{
		//출력 하고 앞으로 가기
		if (withOutput && wordNow.label != "" && wordNow.idenfr != "")
		{
			cout << wordNow.label << " " << wordNow.idenfr << endl;
			fout << wordNow.label << " " << wordNow.idenfr << endl;
		}
		//끝까지 갔을때 끝내기
		if (wordCnt >= top)
		{
			wordNow.label = wordNow.idenfr = "";
			return 0;
		}
		//한칸 앞으로 가기
		else
		{
			wordNow = wordList[wordCnt];
			wordCnt++;
			return 1;
		}
	}
	else
	{
		wordNow = wordList[wordCnt - 2];
		wordCnt--;
		return 1;
	}
}

int primaryExp(int type){
	// 1. (EXP)
	if(wordNow.idenfr=="(")
	{
		Syntax_Analysis_Main(true, true); //'('출력
		parseExp(); //EXP로 돌아가기
		Syntax_Analysis_Main(true, true); //') 출력
	}
	else
	{
		// 2. number
		if(wordNow.idenfr[0]>='0'&&wordNow.idenfr[0]<='9')
		{
			Syntax_Analysis_Main(true, true);
			outputSyntax_Analysis("Number");
		}
		// 3. LVal
		else 
		{
			type=parseLVal(type);
		}
	}
	outputSyntax_Analysis("PrimaryExp");

	return type;
}

void unaryOP(){
	Syntax_Analysis_Main(true, true); //부호 출력
	outputSyntax_Analysis("UnaryOp");
}

int unaryExp(int type){
	Syntax_Analysis_Main(true, false);
	string temp = wordNow.idenfr;
	Syntax_Analysis_Main(false, false);
	if (wordNow.label == "PLUS" || wordNow.label == "MINU" ||wordNow.label == "NOT")
	{
		unaryOP();
		type = unaryExp(type);
	}
	else if(wordNow.label=="IDENFR"&&temp=="(")
	{
		//  Ident '(' [FuncParams] ');
		//error handeler
		lastNonTerminalLine = wordNow.lineId;
		parseErrorC(wordNow.idenfr, wordNow.lineId); // error c

		FuncInfo funcInfo = getFuncInfo(wordNow.idenfr);
		Syntax_Analysis_Main(true, true); // Ident(함수명) 출력
		Syntax_Analysis_Main(true, true); // ( 출력

		//지금 보고있는 줄에 인자가 있다는 뜻이다.
		if(wordNow.idenfr != ")" && wordNow.idenfr != ";")
		{
			//지금 보고있는 줄에 인자가 있는데, 부르는 함수에는 인자가 없으면, d에러
			if(funcInfo.paramNum==0)
			{
				error(wordNow.lineId, "d");
				parseFuncRParamsWithoutError();
			}
			else{
				type = funcInfo.type;
				parseFuncRParams(funcInfo);
			}
		}
        //원래 함수에는 인자가 있는데 지금은 )인 경우
        else if (funcInfo.paramNum > 0 && wordNow.idenfr == ")")
            error(wordNow.lineId, "d");
		// ()가 잘 닫히는지 확인하는 오류. 함수에서 )까지 출력한다.
		parseErrorJ();
	}
	else    type=primaryExp(type);
	
	outputSyntax_Analysis("UnaryExp");
	return type;
}

int mulExp(int type){
	type = unaryExp(type);
	
	outputSyntax_Analysis("MulExp");
	if (wordNow.label == "MULT" || wordNow.label == "DIV" || wordNow.label == "MOD")
	{
		Syntax_Analysis_Main(true, true);
		type=mulExp(type);
	}
	return type;
}

int addExp(int type){
	type=mulExp(type);
	
	outputSyntax_Analysis("AddExp");
	if (wordNow.label == "PLUS" || wordNow.label == "MINU")
	{     
		Syntax_Analysis_Main(true, true);
		type=addExp(type);
	}
	return type;
}

int parseExp(){
	int type = INT;
	type=addExp(type);

	outputSyntax_Analysis("Exp");
	return type;
}

void parseConstExp(){
	addExp(-1);

	outputSyntax_Analysis("ConstExp");
}

// < > <= >=
void parseRelExp()
{
	addExp(-1);

	outputSyntax_Analysis("RelExp");
	if (wordNow.idenfr == "<" || wordNow.idenfr == "<=" || wordNow.idenfr == ">" || wordNow.idenfr == ">=")
	{
		Syntax_Analysis_Main(true, true); // < > <= >=
		parseRelExp();
	}
}

void parseEqExp()
{
	parseRelExp();
	outputSyntax_Analysis("EqExp");
	if (wordNow.idenfr == "!=" || wordNow.idenfr == "==")
	{
		Syntax_Analysis_Main(true, true); // == !=
		parseEqExp();
	}
}

void parseLAndExp()
{
	parseEqExp();

	outputSyntax_Analysis("LAndExp");
	if (wordNow.label == "AND")
	{
		Syntax_Analysis_Main(true, true); // &&
		parseLAndExp();
	}
}

void parseLOrExp()
{
	parseLAndExp();

	outputSyntax_Analysis("LOrExp");
	if (wordNow.label == "OR")
	{
		Syntax_Analysis_Main(true, true); // ||
		parseLOrExp();
	}
}

void parseCond(){
	parseLOrExp();
	
	outputSyntax_Analysis("Cond");
}

void parseIf()
{
	lastNonTerminalLine = wordNow.lineId;
	Syntax_Analysis_Main(true, true); //if
	Syntax_Analysis_Main(true, true); //(
	parseCond();
	//if하고 {가 같은 줄에 있으면 이거, 다음 줄에 있으면 -1을 해야함.
	// ()가 잘 닫히는지 확인하는 오류. 함수에서 )까지 출력한다.
	parseErrorJ();
	parseStmt();
	if(wordNow.label=="ELSETK")
	{
		
		Syntax_Analysis_Main(true, true); //else
		parseStmt();
	}
}

void parseForStmt()
{
	parseErrorH(wordNow.idenfr, wordNow.lineId); 
	parseLVal(-1);
	Syntax_Analysis_Main(true, true); // =
	parseExp();

	outputSyntax_Analysis("ForStmt");
}

void parseFor()
{
	Syntax_Analysis_Main(true, true); // for
	Syntax_Analysis_Main(true, true); // (
	if (wordNow.idenfr != ";")
	{
		parseForStmt();
	}
	Syntax_Analysis_Main(true, true); //;
	if (wordNow.idenfr != ";")
	{
		parseCond();
	}
	Syntax_Analysis_Main(true, true); // ;
	if(wordNow.idenfr!=")")
	{
		parseForStmt();
	}
	
	// ()가 잘 닫히는지 확인하는 오류. 함수에서 )까지 출력한다.
	parseErrorJ();

	int numOfBlocks = 0;
	int temp = wordCnt-1;
	while (wordList[temp++].idenfr == "{")
		numOfBlocks++;

	isCirculer[levelNow + numOfBlocks] = true;
	parseStmt();
}

void parseReadStmt()
{
	Syntax_Analysis_Main(true, true); //getint
	Syntax_Analysis_Main(true, true); //(
	Syntax_Analysis_Main(true, true); //)
}

void parseReturnStmt()
{
	isReturnCalled = true;
	Syntax_Analysis_Main(true, true); // return 출력

	// 함수가 반환값이 있다면
	if (wordNow.idenfr != ";")
	{
		if(currentFunctionType==VOID)
			error(wordNow.lineId, "f");
		parseExp();
	}
	
	//없으면 종료
}

int countFormatSpecifier(string& s) {
    int count = 0;
    size_t pos = 0;

    while ((pos = s.find("%d", pos)) != string::npos) {
        count++;
        pos += 2; // "%d" 길이만큼 건너뜁니다.
    }

    return count;
}

void parsePrint()
{
	Syntax_Analysis_Main(true, true); //printf
	Syntax_Analysis_Main(true, true); //'('
	int numOfOutput = countFormatSpecifier(wordNow.idenfr);
	int checkCount = 0;
	Syntax_Analysis_Main(true, true); // formatString

	while(wordNow.label == "COMMA") // ,라면
	{
		checkCount++;
		Syntax_Analysis_Main(true, true); //,출력
		parseExp();
	}
	if(checkCount!=numOfOutput)
	{
		error(wordNow.lineId, "l");
	}
}

int parseLVal(int type)
{
	//error handeler getType은 선언됐던 타입이 나옴. 
	int getType = parseErrorC(wordNow.idenfr, wordNow.lineId);
	if(getType!=-1)
		type = getType;
	
	int arrNum = 0;
	Syntax_Analysis_Main(true, true); // IDent 출력;
	while(wordNow.idenfr=="[")
	{
		arrNum++;

		Syntax_Analysis_Main(true, true); //[ 출력
		parseExp();

		// k error : ]가 있는지 없는지 확인후 있으면 ] 출력
		parseErrorK();
	}
	//func(a)같은 본인만 있는 상황 타입은 변하지 않는다.
	if(arrNum==0)
	{} 
	// int a[]; func(a[])	, int a[][]; func(a[][])인 상황. 타입은 int가 된다.
	else if ((arrNum == 1 && getType == ARR1) || (arrNum==2 && getType==ARR2))
		type = INT;
	// 나머지 상황. int a[2][2]; func(a[2])같은 상황. 그럼 arrNum대로 정해진다
	else {
		if (arrNum == 1 && type==ARR2)
			type = ARR1;
		//만약 3차원 배열이 나오면, 그대로 type=arrNum의 갯수대로 간다.
	}
	outputSyntax_Analysis("LVal");
	return type;
}

void parseStmt()
{
	if (wordNow.label== "IFTK")
	{
		parseIf();
	}
	else if (wordNow.label == "FORTK")
	{
		lastNonTerminalLine = wordNow.lineId;
		parseFor();
	}
	else if (wordNow.label== "BREAKTK" ||wordNow.label == "CONTINUETK")
	{
		lastNonTerminalLine = wordNow.lineId;

		if(!isCirculer[levelNow])
			error(wordNow.lineId, "m");

		Syntax_Analysis_Main(true, true);

		//;가 없는지 확인하는 에러. 있으면 ;까지 출력
		parseErrorI();
	}
	else if (wordNow.idenfr== ";")
	{
		Syntax_Analysis_Main(true, true); // ; 출력
	}
	else if (wordNow.label == "RETURNTK")
	{
		lastNonTerminalLine = wordNow.lineId;
		parseReturnStmt();

		//;가 없는지 확인하는 에러. 있으면 ;까지 출력
		parseErrorI();
	}
	else if (wordNow.label== "PRINTFTK")
	{
		lastNonTerminalLine = wordNow.lineId;
		parsePrint();

		// ()가 잘 닫히는지 확인하는 오류. 함수에서 )까지 출력한다.
		parseErrorJ();
		//;가 없는지 확인하는 에러. 있으면 ;까지 출력
		parseErrorI();
	}
	//block
	else if(wordNow.idenfr=="{")
	{
		parseBlock(false);
	}
	// 1. stmt -> LVal = Exp;
	// 2. stmt -> LVal = getint();
	// 3 exp; 
	else
	{
		//Judge  LVel
		int index = 0;
		int isLVal = false;
		while (true)
		{
			if(wordNow.label=="SEMICN")
				break;
			if(wordNow.idenfr == "=")
			{
				isLVal = true;
				break;
			}
			index++;

			//오류나면 끝내기
			if (!Syntax_Analysis_Main(true, false)) break;
		}
		//go back for LVal index
		while(index)
		{
			Syntax_Analysis_Main(false, false);
			index--;
		}

		if(isLVal)
		{
			parseErrorH(wordNow.idenfr, wordNow.lineId); 
			parseLVal(-1); // Ident 출력
			Syntax_Analysis_Main(true, true); // =
			// 2. stmt -> LVal = getint();
			if (wordNow.label == "GETINTTK")
			{
				lastNonTerminalLine = wordNow.lineId;
				Syntax_Analysis_Main(true, true); //getint
				Syntax_Analysis_Main(true, true); //(
				
				// ()가 잘 닫히는지 확인하는 오류. 함수에서 )까지 출력한다.
				parseErrorJ();
				
				//;가 없는지 확인하는 에러. 있으면 ;까지 출력
				parseErrorI();
			}
			// 1. stmt -> LVal = Exp;
			else
			{
				lastNonTerminalLine = wordNow.lineId;
				parseExp(); //exp출력}

				//;가 없는지 확인하는 에러. 있으면 ;까지 출력
				parseErrorI();
			}
		}
		// 3 바로 Exp
		else
		{
			lastNonTerminalLine = wordNow.lineId;
			parseExp(); //exp출력}

			//;가 없는지 확인하는 에러. 있으면 ;까지 출력
			parseErrorI();
		}
	}
	
	outputSyntax_Analysis("Stmt");
}

void parseConstInitVal()
{
	lastNonTerminalLine = wordNow.lineId;
	if (wordNow.idenfr == "{")
	{
		Syntax_Analysis_Main(true, true);
		if(wordNow.idenfr!="}")
		{
			parseConstInitVal();
			while(wordNow.idenfr==",")
			{
				Syntax_Analysis_Main(true, true);
				parseConstInitVal();
			}
		}
		Syntax_Analysis_Main(true, true);
	}
	else
	{
		parseConstExp();
	}
	
	outputSyntax_Analysis("ConstInitVal");
}

void parseConstDef()
{
	//error handeler
	int type = INT;
	if (!parseErrorB(wordNow.idenfr, wordNow.lineId)) // 변수명 확인
	{
		symbolTable[++symbolTableTop] = Symbol{ wordNow.idenfr, CONST, type, levelNow };
	}
	Syntax_Analysis_Main(true, true); //c 출력
	int numOfArrLayer = 0;
	while (wordNow.idenfr == "[") // [ (배열)이면
	{
		numOfArrLayer++;
		Syntax_Analysis_Main(true, true); // [
		parseConstExp(); //숫자

		// k error : ]가 있는지 없는지 확인후 있으면 ] 출력
		parseErrorK();
	}
	if(numOfArrLayer==1)
		symbolTable[symbolTableTop].type = ARR1;
	else if(numOfArrLayer==2)
		symbolTable[symbolTableTop].type = ARR2;

	Syntax_Analysis_Main(true, true); // = 출력
	parseConstInitVal();
		
	outputSyntax_Analysis("ConstDef");
}

void parseConstDecl()
{
    
	Syntax_Analysis_Main(true, true); //CONST 출력
	Syntax_Analysis_Main(true, true); //int 출력

	parseConstDef();
	while(wordNow.idenfr==",")// ; 가 나오는동안
	{
		if (!Syntax_Analysis_Main(true, true)) break; // 일단 , 출력하고 틀리면 끝내기
		if (wordNow.label== "SEMICN") break;

		parseConstDef();
	}

	//;가 없는지 확인하는 에러. 있으면 ;까지 출력
	parseErrorI();

	outputSyntax_Analysis("ConstDecl");
}

void parseInitVal()
{
    if(wordNow.idenfr=="{")
    {
        Syntax_Analysis_Main(true, true); // {
        if(wordNow.idenfr!="}")
        {
            parseInitVal();
            while(wordNow.idenfr==",")
            {
                Syntax_Analysis_Main(true, true); // ,
                parseInitVal();
            }
        }
        Syntax_Analysis_Main(true, true); // }
    }
    else
    {
        parseExp();
    }

    outputSyntax_Analysis("InitVal");
}

void parseVarDef()
{
	//error handeler
	int type = INT;
	if (!parseErrorB(wordNow.idenfr, wordNow.lineId)) // 변수명 확인
	{
		symbolTable[++symbolTableTop] = Symbol{ wordNow.idenfr, VAR, type, levelNow};
	}
	Syntax_Analysis_Main(true, true); //c 출력
	int numOfArrLayer = 0;
	while (wordNow.idenfr == "[") // [ (배열)이면
	{
		numOfArrLayer++;
		Syntax_Analysis_Main(true, true); // [
		parseConstExp(); //숫자
		
		// k error : ]가 있는지 없는지 확인후 있으면 ] 출력
		parseErrorK();
	}
	if(numOfArrLayer==1)
		symbolTable[symbolTableTop].type = ARR1;
	else if(numOfArrLayer==2)
		symbolTable[symbolTableTop].type = ARR2;


	if (wordNow.idenfr== "=") // = 즉 초기화가 있으면
	{
		Syntax_Analysis_Main(true, true); // = 출력
		parseInitVal();
	}

	outputSyntax_Analysis("VarDef");
}

void parseVarDecl()
{
	lastNonTerminalLine = wordNow.lineId;
	Syntax_Analysis_Main(true, true); //int 출력

	parseVarDef();
	while(wordNow.idenfr==",")// ; 가 나오는동안
	{
		if (!Syntax_Analysis_Main(true, true)) break; // 일단 , 출력하고 틀리면 끝내기
		if (wordNow.label== "SEMICN") break;

		parseVarDef();
	}


	//;가 없는지 확인하는 에러. 있으면 ;까지 출력
	parseErrorI();

	outputSyntax_Analysis("VarDecl");
}
void parseDecl(){
	if(wordNow.label=="INTTK")
		parseVarDecl();
	else if(wordNow.label=="CONSTTK")
		parseConstDecl();
}

// Block → '{' { BlockItem } '}'   
void parseBlock(bool isNeedReturn)
{
	//함수에서 호출된 levelUp은 그전에 처리했다.
	if(!isCameFromParam)
		levelUp();
	isCameFromParam = false;

	Syntax_Analysis_Main(true, true); // {
	while(wordNow.idenfr!="}")
	{
		//if Decl
		if (wordNow.label == "INTTK" ||wordNow.label == "CONSTTK")
		{
			parseDecl();
		}
		//if Stmt
		else{
			parseStmt();
		}
	}

	//int 함수인데 && 반환값이 없다면 && 메인함수, 일반 함수에서 불린 return은 true, stmt에서 불린 block은 없다.
	if(currentFunctionType == INT && !isReturnCalled && isNeedReturn)
	{
		error(wordNow.lineId, "g");
	}
	isReturnCalled = false;
	isCirculer[levelNow] = false;

	Syntax_Analysis_Main(true, true); // }
	levelDown();

	outputSyntax_Analysis("Block");
}

void parseFuncRParamsWithoutError()
{
	parseExp();
	while (wordNow.idenfr == ",") // , 가 나오는동안
	{
		if (!Syntax_Analysis_Main(true, true)) break; // 일단 , 출력하고 틀리면 끝내기

		parseExp();
	}
	fout << "<FuncRParams>" << endl; 
	cout << "<FuncRParams>" << endl;
}

/** parseErrorE */
void parseFuncRParams(FuncInfo funcInfo)
{
	//함수와 대조하는 것이 아닌, 함수 다음에 있는 인자와 비교하기 때문에 +1이 된다.
	int cnt = 1;
	int type = parseExp();
	//type은 지금보고있는 줄의 함수가 호출될때의 타입, symboltable[]부분이 함수의 타입
	if (type != symbolTable[cnt + funcInfo.pos].type) 
		error(wordNow.lineId, "e");

	int numOfParam = 1;
	while (wordNow.idenfr == ",") // , 가 나오는동안
	{
		cnt++;
		if (!Syntax_Analysis_Main(true, true))
			break; // 일단 , 출력하고 틀리면 끝내기
		numOfParam++;
		type = parseExp();
		if (type != symbolTable[cnt + funcInfo.pos].type) 
			error(wordNow.lineId, "e");
	} 
	if(funcInfo.paramNum!=numOfParam)
		error(wordNow.lineId, "d");

	outputSyntax_Analysis("FuncRParams");
}

void parseFuncFParam()
{
	Syntax_Analysis_Main(true, true); //int

	//error handeler
	int type = INT;
	if (!parseErrorB(wordNow.idenfr, wordNow.lineId)) // 변수명 확인
	{
		symbolTable[++symbolTableTop] = Symbol{ wordNow.idenfr, PARAM, type, levelNow};
	}
	Syntax_Analysis_Main(true, true); //ident
	int numOfArrLayer = 0;
	if (wordNow.idenfr == "[") // [ 라면, 즉 배열이라면
	{
		numOfArrLayer++;
		Syntax_Analysis_Main(true, true); // 배열 [

		// k error : ]가 있는지 없는지 확인후 있으면 ] 출력
		parseErrorK();

        //2 혹은 3,4,5,6...
		while(wordNow.idenfr=="[") // [
		{
			numOfArrLayer++;
			Syntax_Analysis_Main(true, true); // [
			parseConstExp();

			// k error : ]가 있는지 없는지 확인후 있으면 ] 출력
			parseErrorK();
		}
	}
	if(numOfArrLayer==1)
		symbolTable[symbolTableTop].type = ARR1;
	else if(numOfArrLayer==2)
		symbolTable[symbolTableTop].type = ARR2;

	outputSyntax_Analysis("FuncFParam");
}

void parseFuncFParams()
{
	parseFuncFParam();
	while(wordNow.label=="COMMA")// ; 가 나오는동안
	{
		if (!Syntax_Analysis_Main(true, true)) break; // 일단 , 출력하고 틀리면 끝내기

		parseFuncFParam();
	}
	outputSyntax_Analysis("FuncFParams");
}

// 나머지 함수일때 사용
void parseFuncDef()
{
	int type;
	if (wordNow.idenfr == "void")
		type = VOID;
	else if(wordNow.idenfr=="int")
		type = INT;

	Syntax_Analysis_Main(true, true); // void, int 구분할 필요가 없는게 return에서 차피 구분하게 만들어놓음
	
	outputSyntax_Analysis("FuncType");

	//error handeler
	if (!parseErrorB(wordNow.idenfr, wordNow.lineId)) // error b 함수명 중복 확인
	{
		symbolTable[++symbolTableTop] = Symbol{ wordNow.idenfr, FUNC, type, levelNow };
	}
	Syntax_Analysis_Main(true, true); // Ident : 함수이름


	lastNonTerminalLine = wordNow.lineId;
	//여기서 레벨업해야지 인자가 한단계 높게 설정된다.
	levelUp();
    isCameFromParam = true;
    
    Syntax_Analysis_Main(true, true); // '('
    // 두번째는 )가 없을때 바로 {가 나올테니 그걸 노린 에러
    if(wordNow.label!="RPARENT" && wordNow.idenfr != "{")
		parseFuncFParams();
	
	// ()가 잘 닫히는지 확인하는 오류. 함수에서 )까지 출력한다.
	parseErrorJ();

	parseBlock(true);

	outputSyntax_Analysis("FuncDef");
}

//메인 함수일떄 사용
void parseMainFunc()
{
	Syntax_Analysis_Main(true, true); //int
	Syntax_Analysis_Main(true, true); //main
	Syntax_Analysis_Main(true, true); // (

	lastNonTerminalLine = wordNow.lineId;
	// ()가 잘 닫히는지 확인하는 오류. 함수에서 )까지 출력한다.
	parseErrorJ();
	parseBlock(true);
	
	outputSyntax_Analysis("MainFuncDef");
}

void parseCompUnit()
{
	string temp1 = wordNow.label;
	Syntax_Analysis_Main(true, false);
	string isMain = wordNow.label;
	Syntax_Analysis_Main(true, false);
	string temp3 = wordNow.label;
	Syntax_Analysis_Main(false, false);
	Syntax_Analysis_Main(false, false);
	// 메인 함수라면
	if(temp1=="INTTK"&&isMain=="MAINTK")
	{
		currentFunctionType = INT;
		parseMainFunc();
		outputSyntax_Analysis("CompUnit");
		return;
	}
	// 함수라면
	else if((temp1=="VOIDTK"&&temp3=="LPARENT")||(temp1=="INTTK"&&temp3=="LPARENT"))
	{
		if(temp1=="VOIDTK")
			currentFunctionType = VOID;
		else if(temp1=="INTTK")
			currentFunctionType = INT;

		parseFuncDef();
		parseCompUnit();
	}
	// 전역 변수라면
	else
	{
		parseDecl();
		parseCompUnit();
	}
}

void Syntax_Analysis(){
	wordCnt = 0;
	if (Syntax_Analysis_Main(true, true))
	{
		parseCompUnit();
	}
}

string toLowerCase(string in)
{
	string out = "";
	for (int i = 0; i < in.length(); i++)
	{
		out += in[i] >= 'a' && in[i] <= 'z' ? in[i] : in[i] - ('A' - 'a');
	}
	return out;
}

// 만약 void f1(int x,int y,int z)가 있으면,
//funcInfo = { symbolTable에서 의 f1위치, 3개, void타입 } 이다.
FuncInfo getFuncInfo(string idenfr)
{
	FuncInfo funcInfo = { -1, -1, -1 };// [0]:func pos [1]:param num [2]:return type
	for (int i = symbolTableTop; i >= 0; i--)
	{
		if (toLowerCase(symbolTable[i].idenfr) == toLowerCase(idenfr) && symbolTable[i].kind == FUNC)
		{
			funcInfo.type = symbolTable[i].type;
			funcInfo.pos = i;
			int cnt = 0, ind = i + 1;
			int paramPoint[100];
			while (symbolTable[ind].kind == PARAM)
			{
				funcInfo.paramPos[cnt] = ind;
				ind++;
				cnt++;
			}
			funcInfo.paramNum = cnt;
		}
	}
	return funcInfo;
}

 
// 기본적으로 { 하나 감지될때마다 레벨업한다. main, 그냥함수, stmt에 있는 { 3가지 경우다. 나는 parseBlock에 넣었다.
void levelUp()
{
	levelNow++;
	blocks[blocksTop++] = symbolTableTop;
}

void levelDown()
{
	levelNow--;
}

int numOfErrors = 0;

/** ] 오류 */
void parseErrorK(){
	if(wordNow.idenfr!="]")
		error(wordNow.lineId, "k");
	else
		Syntax_Analysis_Main(true, true); // ]
}

/** ) 오류 */
void parseErrorJ(){
	if (wordNow.idenfr != ")")
	{
		error(lastNonTerminalLine, "j");
	}
	else
		Syntax_Analysis_Main(true, true); // ')'
}

/** ; 오류 */
void parseErrorI(){
	if (wordNow.idenfr != ";")
	{
		error(lastNonTerminalLine, "i");
	}
	else
		Syntax_Analysis_Main(true, true); // ; 출력
}

/** 상수 재선언 */
void parseErrorH(string idenfr, int lineID)
{
	for (int i = symbolTableTop; i >= 0; i--)
	{
		if (idenfr == symbolTable[i].idenfr)
		{
			if(symbolTable[i].kind==CONST)
			{
				error(lineID, "h");
			}
            else
                return;
        }
    }
}

/** 선언안된 변수 */
int parseErrorC(string idenfr, int lineID)
{
	for (int i = symbolTableTop; i >= 0; i--)
	{
		if (idenfr == symbolTable[i].idenfr)
		{
			return symbolTable[i].type;
		}
	}
	error(lineID, "c");
	return -1;
}

/** 변수 겹침 */
bool parseErrorB(string idenfr, int lineID)
{
    if(blocksTop==0)
    {
        for (int i = symbolTableTop; i >= 0; i--)
        {
            if (idenfr == symbolTable[i].idenfr && symbolTable[i].level == levelNow)
            {
                error(lineID, "b");
                return 1;
            }
        } 
    }
    else if (levelNow == 0)
    {
        for (int i = symbolTableTop; i >= 0; i--)
        {
            if (idenfr == symbolTable[i].idenfr && symbolTable[i].level == levelNow)
            {
                error(lineID, "b");
                return 1;
            }
        }
    }
    else
    {
        for (int i = symbolTableTop; i > blocks[blocksTop - 1]; i--)
        {
            if (idenfr == symbolTable[i].idenfr && symbolTable[i].level == levelNow)
            {
                error(lineID, "b");
                return 1;
            }
        }
    }
	return 0;
}

void parseErrorA(char ch, bool isAlreadyError)
{
	if(ch==37 && inputedString[wordCnt+1]!='d' && !isAlreadyError)
	{
		error(global_line, "a");
		isAlreadyError = true;
	}
	else if(ch=='\\' && inputedString[wordCnt+1]!='n' && !isAlreadyError){
		error(global_line, "a");
		isAlreadyError = true;
	}
	else if(ch!=32 && ch!=33 && ch != 37 && (ch < 40 || ch > 126) && !isAlreadyError)
	{
		error(global_line, "a");
		isAlreadyError = true;
	}
}

void error(int lineId,string errType)
{
	errorList[numOfErrors++] = {lineId, errType};
}

bool compareByLineId(const errorType& a, const errorType& b) {
    return a.lineId < b.lineId;
}

void outputError()
{
	sort(errorList, errorList + numOfErrors, compareByLineId);
	/* bool skipList[100] = {false};
	int skipNumber = 0;
	for (int i = 0; i < numOfErrors - 1; i++)
	{
		if(errorList[i].lineId==errorList[i+1].lineId)
			skipList[skipNumber++] = true;
	} */
	for (int i = 0; i < numOfErrors; i++)
	{
		/* if(skipList[i]==true)
			continue; */
		cout << errorList[i].lineId << " " << errorList[i].errType << endl;
		errorOut << errorList[i].lineId << " " << errorList[i].errType << endl;
	}
}

int main(void) {
    init(); 
    Lexical_Analysis();
    Syntax_Analysis();
    outputError();

    fclose(inputFile);
    inFile.close();
    fout.close();
    errorOut.close();
    return 0;
}

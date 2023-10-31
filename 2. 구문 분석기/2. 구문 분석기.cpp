#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

FILE *inputFile;
ofstream fout("output.txt");

string mapList[10000][3];
string token;
string inputedString="";
string couple[2] = {"", ""};

int inputedStringLength = 0;
int top = 0;
int wordCnt = 0;
int num;

void parseConstDef();
void parseVarDef();
void parseFuncDef();
void parseStmt();
void parseExp();
void parseReadStmt();
void parsePrint();
void parseReturnStmt();
void parseMainFunc();
void parseBlock();
void parseFuncFParams();
void parseLVal();
void parseFuncRParams();

void init()
{
  char inputString[100];
  inputFile = fopen("testfile.txt", "r");
  while (fgets(inputString, sizeof(inputString), inputFile) != NULL)
  {
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

void skip() {
	while(inputedString[wordCnt]==' ' ||parsingAnnotation()) {
		wordCnt++;
	}
	return;
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
int checkReserver() {
	if (token=="main") {
		mapList[top][0] = "MAINTK";
		mapList[top++][1] = "main";
	}else if (token=="const") {
		mapList[top][0] = "CONSTTK";
		mapList[top++][1] = "const";
	}else if (token=="int") {
		mapList[top][0] = "INTTK";
		mapList[top++][1] = "int";
	}else if (token=="break") {
		mapList[top][0] = "BREAKTK";
		mapList[top++][1] = "break";
	}else if (token=="continue") {
		mapList[top][0] = "CONTINUETK";
		mapList[top++][1] = "continue";
	}else if (token=="if") {
		mapList[top][0] = "IFTK";
		mapList[top++][1] = "if";
	}else if (token=="else") {
		mapList[top][0] = "ELSETK";
		mapList[top++][1] = "else";
	}else if (token=="for") {
		mapList[top][0] = "FORTK";
		mapList[top++][1] = "for";
	}else if (token=="getint") {
		mapList[top][0] = "GETINTTK";
		mapList[top++][1] = "getint";
	}else if (token=="printf") {
		mapList[top][0] = "PRINTFTK";
		mapList[top++][1] = "printf";
	}else if (token=="return") {
		mapList[top][0] = "RETURNTK";
		mapList[top++][1] = "return";
	}else if (token=="void") {
		mapList[top][0] = "VOIDTK";
		mapList[top++][1] = "void";
	}else {
		return -1;
	}
	return 0;
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
		if (checkReserver() == -1) {
			mapList[top][0] = "IDENFR";
			mapList[top++][1] = token;
		}
	}
	else if (isdigit(inputedString[wordCnt]))
	{
        while(isdigit(inputedString[wordCnt])) {
            token += inputedString[wordCnt];
            wordCnt++;
        }
        mapList[top][0] = "INTCON";
        mapList[top++][1] = token;

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
            
    }
	else if (inputedString[wordCnt] == '+'){
		mapList[top][0] = "PLUS";
		mapList[top++][1] = "+";
		wordCnt++;
	}
	else if (inputedString[wordCnt] == '-')
	{
		mapList[top][0] = "MINU";
		mapList[top++][1] = "-";
		wordCnt++;
	}
	else if (inputedString[wordCnt] == '*')
	{
		mapList[top][0] = "MULT";
		mapList[top++][1] = "*";
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
			mapList[top][0] = "DIV";
			mapList[top++][1] = '/';
			wordCnt++;
		}
	}
	else if (inputedString[wordCnt] == '%')
	{
		mapList[top][0] = "MOD";
		mapList[top++][1] = "%";
		wordCnt++;
	}
	else if (inputedString[wordCnt] == '<')
	{
		if(inputedString[wordCnt+1]=='=')
		{
			wordCnt++;
			mapList[top][0] = "LEQ";
			mapList[top++][1] = "<=";
		}
		else
		{
			mapList[top][0] = "LSS";
			mapList[top++][1] = "<";
		}
		wordCnt++;
	}
	else if (inputedString[wordCnt] =='>')
	{
		if(inputedString[wordCnt+1]=='=')
		{
			mapList[top][0] = "GEQ";
			mapList[top++][1] = ">=";
			wordCnt++;
		}
		else
		{
			mapList[top][0] = "GRE";
			mapList[top++][1] = ">";
		}
		wordCnt++;
	}else if (inputedString[wordCnt] =='=')
	{
		if(inputedString[wordCnt+1]=='=')
		{
			mapList[top][0] = "EQL";
			mapList[top++][1] = "==";
			wordCnt++;
		}
		else
		{
			mapList[top][0] = "ASSIGN";
			mapList[top++][1] = "=";
		}
		wordCnt++;
	}else if (inputedString[wordCnt] =='!')
	{
		if(inputedString[wordCnt+1]=='=')
		{
			mapList[top][0] = "NEQ";
			mapList[top++][1] = "!=";
			wordCnt++;
		}
		else
		{
			mapList[top][0] = "NOT";
			mapList[top++][1] = "!";
		}
		wordCnt++;
	}else if (inputedString[wordCnt]=='(') {
		mapList[top][0] = "LPARENT";
		mapList[top++][1] = "(";
		wordCnt++;
	}else if (inputedString[wordCnt]==')') {
		mapList[top][0] = "RPARENT";
		mapList[top++][1] = ")";
		wordCnt++;
	}else if (inputedString[wordCnt]==',') {
		mapList[top][0] = "COMMA";
		mapList[top++][1] = ",";
		wordCnt++;
	}else if (inputedString[wordCnt]==';') {
		mapList[top][0] = "SEMICN";
		mapList[top++][1] = ";";
		wordCnt++;
	}else if (inputedString[wordCnt]=='[') {
		mapList[top][0] = "LBRACK";
		mapList[top++][1] = "[";
		wordCnt++;
	}else if (inputedString[wordCnt]==']') {
		mapList[top][0] = "RBRACK";
		mapList[top++][1] = "]";
		wordCnt++;
	}else if (inputedString[wordCnt]=='{') {
		mapList[top][0] = "LBRACE";
		mapList[top++][1] = "{";
		wordCnt++;
	}else if (inputedString[wordCnt]=='}') {
		mapList[top][0] = "RBRACE";
		mapList[top++][1] = "}";
		wordCnt++;
	}else if(inputedString[wordCnt]=='&'&&inputedString[wordCnt+1]=='&'){
		mapList[top][0] = "AND";
		mapList[top++][1] = "&&";
		wordCnt+=2;
	}else if(inputedString[wordCnt]=='|'&&inputedString[wordCnt+1]=='|'){
		mapList[top][0] = "OR";
		mapList[top++][1] = "||";
		wordCnt+=2;
	}else if(inputedString[wordCnt]=='\"'){
		int endpos = inputedString.find('\"', wordCnt+1);
		token = inputedString.substr(wordCnt, endpos - wordCnt+1);
		wordCnt += endpos - wordCnt+1;

		mapList[top][0] = "STRCON";
		mapList[top++][1] = token;
	}
}

void Lexical_Analysis()
{
	while (wordCnt != inputedStringLength)
		Lexical_Analysis_Main();

	// Lexical부분 출력
	/* for (int i = 0; i < top;i++)
	{
		fout << mapList[i][0] << " " << mapList[i][1] << endl;
		cout << mapList[i][0] << " " << mapList[i][1] << endl;
	} */
}

int Syntax_Analysis_Main(bool forward, bool withOutput = false)
{
	if (forward)
	{
		//출력 하고 앞으로 가기
		if (withOutput && couple[0] != "" && couple[1] != "")
		{
            fout << couple[0] << " " << couple[1] << endl;
            cout << couple[0] << " " << couple[1] << endl;
		}
		//끝까지 갔을때 끝내기
		if (wordCnt >= top)
		{
			couple[0] = couple[1] = "";
			return 0;
		}
		//한칸 앞으로 가기
		else
		{
			couple[0] = mapList[wordCnt][0];
			couple[1] = mapList[wordCnt][1];
			wordCnt++;
			return 1;
		}
	}
	else
	{
		couple[0] = mapList[wordCnt - 2][0];
		couple[1] = mapList[wordCnt - 2][1];
		wordCnt--;
		return 1;
	}
}

void primaryExp(){
	// 1. (EXP)
	if(couple[1]=="(")
	{
		Syntax_Analysis_Main(true, true); //'('출력
		parseExp(); //EXP로 돌아가기
		Syntax_Analysis_Main(true, true); //') 출력
	}
	else
	{
		// 2. number
		if(couple[1][0]>='0'&&couple[1][0]<='9')
		{
			Syntax_Analysis_Main(true, true);
			cout << "<Number>" << endl;
			fout << "<Number>" << endl;
		}
		// 3. LVar
		else 
		{
			parseLVal();
		}
	}
	cout << "<PrimaryExp>" << endl;
	fout << "<PrimaryExp>" << endl;
}

void unaryOP(){
	Syntax_Analysis_Main(true, true); //부호 출력
	cout << "<UnaryOp>" << endl;
	fout << "<UnaryOp>" << endl;
}

void unaryExp(){
	Syntax_Analysis_Main(true, false);
	string temp = couple[1];
	Syntax_Analysis_Main(false, false);
	if (couple[0] == "PLUS" || couple[0] == "MINU" || couple[0] == "NOT")
	{
		unaryOP();
		unaryExp();
	}
	else if(couple[0]=="IDENFR"&&temp=="(")
	{
		//  Ident '(' [FuncParams] ');
		Syntax_Analysis_Main(true, true); // Ident 출력
		Syntax_Analysis_Main(true, true); // ( 출력
		if(couple[1]!=")")
			parseFuncRParams();
		Syntax_Analysis_Main(true, true); // ) 출력
	}
	else    primaryExp();
	cout << "<UnaryExp>" << endl;
	fout << "<UnaryExp>" << endl;
}

void mulExp(){
	unaryExp();
	cout << "<MulExp>" << endl;
	fout << "<MulExp>" << endl;
	if(couple[0]=="MULT"||couple[0]=="DIV"||couple[0]=="MOD")
	{
		Syntax_Analysis_Main(true, true);
		unaryExp();
	}
}

void addExp(){
	mulExp();
	cout << "<AddExp>" << endl;
	fout << "<AddExp>" << endl;
	if(couple[0]=="PLUS"||couple[0]=="MINU")
	{     
		Syntax_Analysis_Main(true, true);
		addExp();
	}
}

void parseExp(){
	addExp();
	cout << "<Exp>" << endl;
	fout << "<Exp>" << endl;
}

void parseConstExp(){
	addExp();
	cout << "<ConstExp>" << endl;
	fout << "<ConstExp>" << endl;
}

// < > <= >=
void parseRelExp()
{
	addExp();
	cout << "<RelExp>" << endl;
	fout << "<RelExp>" << endl;
	if(couple[1]=="<"||couple[1]=="<="||couple[1]==">"||couple[1]==">=")
	{
		Syntax_Analysis_Main(true, true); // < > <= >=
		parseRelExp();
	}
}

void parseEqExp()
{
	parseRelExp();
	cout << "<EqExp>" << endl;
	fout << "<EqExp>" << endl;
	if(couple[1]=="!="||couple[1]=="==")
	{
		Syntax_Analysis_Main(true, true); // == !=
		parseEqExp();
	}
}

void parseLAndExp()
{
	parseEqExp();
	cout << "<LAndExp>" << endl;
	fout << "<LAndExp>" << endl;
	if(couple[0]=="AND")
	{
		Syntax_Analysis_Main(true, true); // &&
		parseLAndExp();
	}
}

void parseLOrExp()
{
    /* 
    it was passible but grammer is LOrExp → LAndExp | LOrExp '||' LAndExp 
    so we don't know when we get '||'
    if grammer was LAndExp LOrExp than it would possible


    Syntax_Analysis_Main(true, false);
    string temp = couple[0];
    Syntax_Analysis_Main(false, false);

    if(temp=="||")
    {
        parseLOrExp();
        Syntax_Analysis_Main(true, true);
        parseLAndExp();
    }
    else
        parseLAndExp();
    
    cout << "<LOrExp>" << endl;
    fout << "<LOrExp>" << endl;  */
    parseLAndExp();
    cout << "<LOrExp>" << endl;
    fout << "<LOrExp>" << endl;
    if(couple[0]=="OR")
    {
        Syntax_Analysis_Main(true, true); // ||
        parseLOrExp();
    }
}

void parseCond(){
	parseLOrExp();
	cout << "<Cond>" << endl;
	fout << "<Cond>" << endl;
}

void parseIf()
{
	Syntax_Analysis_Main(true, true); //if
	Syntax_Analysis_Main(true, true); //(
	parseCond();
	Syntax_Analysis_Main(true, true); //)
	parseStmt();
	if(couple[0]=="ELSETK")
	{
		Syntax_Analysis_Main(true, true); //else
		parseStmt();
	}
}

void parseForStmt()
{
	parseLVal();
	Syntax_Analysis_Main(true, true); // =
	parseExp();

	cout << "<ForStmt>" << endl;
	fout << "<ForStmt>" << endl;
}

void parseFor()
{
	Syntax_Analysis_Main(true, true); // for
	Syntax_Analysis_Main(true, true); // (
    if (couple[1] != ";")
    {
		parseForStmt();
	}
	Syntax_Analysis_Main(true, true); //;
    if (couple[1] != ";")
    {
		parseCond();
	}
	Syntax_Analysis_Main(true, true); // ;
	if(couple[1]!=")")
    {
		parseForStmt();
    }
	Syntax_Analysis_Main(true, true);	// )
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
	Syntax_Analysis_Main(true, true); //return 출력
    if (couple[1] != ";")
    {
        parseExp();
    }
	//없으면 종료
}

void parsePrint()
{
	Syntax_Analysis_Main(true, true); //printf
	Syntax_Analysis_Main(true, true); //'('
	Syntax_Analysis_Main(true, true); //formatString

	while(couple[0] == "COMMA") // ,라면
	{
		Syntax_Analysis_Main(true, true); //,출력
		parseExp();
	}
	Syntax_Analysis_Main(true, true); //')'
}

void parseLVal()
{
	Syntax_Analysis_Main(true, true);//IDent 출력; 
	while(couple[1]=="[")
	{
		Syntax_Analysis_Main(true, true);//[ 출력
		parseExp();
		Syntax_Analysis_Main(true, true); //] 출력
	}
	cout << "<LVal>" << endl;
	fout << "<LVal>" << endl;
}

//중요!! 모든 문장을 읽는다. 목표 : 한 문장이 끝날때까지 다시 돌아오면 안된다.
void parseStmt()
{
	if (couple[0] == "IFTK")
	{
		parseIf();
	}
	else if (couple[0] == "FORTK")
	{
		parseFor();
	}
	else if (couple[0] == "BREAKTK" || couple[0] == "CONTINUETK")
	{
		Syntax_Analysis_Main(true, true); 
		Syntax_Analysis_Main(true, true); // ; 출력
	}
    else if (couple[1] == ";")
    {
		Syntax_Analysis_Main(true, true); // ; 출력
	}
	else if (couple[0] == "RETURNTK")
	{
		parseReturnStmt();
		Syntax_Analysis_Main(true, true); // ; 출력
	}
	else if (couple[0] == "PRINTFTK")
	{
		parsePrint();
		Syntax_Analysis_Main(true, true); // ; 출력
	}
	//block
	else if(couple[1]=="{")
	{
		parseBlock();
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
			if(couple[0]=="SEMICN")
				break;
			if(couple[1]=="=")
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
			parseLVal(); // Ident 출력
			Syntax_Analysis_Main(true, true); // =
			// 2. stmt -> LVal = getint();
			if (couple[0] == "GETINTTK")
			{
				Syntax_Analysis_Main(true, true); //getint
				Syntax_Analysis_Main(true, true); //(
				Syntax_Analysis_Main(true, true); //)
				Syntax_Analysis_Main(true, true); // ; 출력
			}
			// 1. stmt -> LVal = Exp;
			else
			{
				parseExp(); //exp출력}
				Syntax_Analysis_Main(true, true); // ; 출력
			}
		}
		// 3 바로 Exp
		else
		{
			parseExp(); //exp출력}
			Syntax_Analysis_Main(true, true); // ; 출력
		}
	}
	fout << "<Stmt>" << endl;
	cout << "<Stmt>" << endl; 
}

void parseConstInitVal()
{
	if(couple[1]=="{")
    {
        Syntax_Analysis_Main(true, true);
        if(couple[1]!="}")
        {
            parseConstInitVal();
            while(couple[1]==",")
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
    fout << "<ConstInitVal>" << endl;
    cout << "<ConstInitVal>" << endl;
}

void parseConstDef()
{
	Syntax_Analysis_Main(true, true); //c 출력

	while (couple[1] == "[") // [ (배열)이면
	{
		Syntax_Analysis_Main(true, true); // [
		parseConstExp(); //숫자
		Syntax_Analysis_Main(true, true); // ]
	}
	
    Syntax_Analysis_Main(true, true); // = 출력
    parseConstInitVal();
		
	fout << "<ConstDef>" << endl;
	cout << "<ConstDef>" << endl;
}

// 변수 2: 변수를 처음 선언할때, 모두 입력받고, <ConstDecl>출력
void parseConstDecl()
{
    
    Syntax_Analysis_Main(true, true); //CONST 출력
    Syntax_Analysis_Main(true, true); //int 출력
	
	parseConstDef();
	while(couple[1]==",")// ; 가 나오는동안
	{
		if (!Syntax_Analysis_Main(true, true)) break; // 일단 , 출력하고 틀리면 끝내기
		if (couple[0] == "SEMICN") break;

		parseConstDef();
	}
	Syntax_Analysis_Main(true, true); // ; 출력

	fout << "<ConstDecl>" << endl;
	cout << "<ConstDecl>" << endl;
}

void parseInitVal()
{
    if(couple[1]=="{")
    {
        Syntax_Analysis_Main(true, true); // {
        if(couple[1]!="}")
        {
            parseInitVal();
            while(couple[1]==",")
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
    fout << "<InitVal>" << endl;
    cout << "<InitVal>" << endl;
}

void parseVarDef()
{
	Syntax_Analysis_Main(true, true); //c 출력

	while (couple[1] == "[") // [ (배열)이면
	{
		Syntax_Analysis_Main(true, true); // [
		parseConstExp(); //숫자
		Syntax_Analysis_Main(true, true); // ]
	}
    if (couple[1] == "=") // = 즉 초기화가 있으면
    {
        Syntax_Analysis_Main(true, true); // = 출력
        parseInitVal();
    }
	fout << "<VarDef>" << endl;
	cout << "<VarDef>" << endl;
}

// 변수 2: 변수를 처음 선언할때, 모두 입력받고, <VarDecl>출력
void parseVarDecl()
{
    Syntax_Analysis_Main(true, true); //int 출력

	parseVarDef();
	while(couple[1]==",")// ; 가 나오는동안
	{
		if (!Syntax_Analysis_Main(true, true)) break; // 일단 , 출력하고 틀리면 끝내기
		if (couple[0] == "SEMICN") break;

		parseVarDef();
	}
	Syntax_Analysis_Main(true, true); // ; 출력

	fout << "<VarDecl>" << endl;
	cout << "<VarDecl>" << endl;
}
void parseDecl(){
	if(couple[0]=="INTTK")
		parseVarDecl();
	else if(couple[0]=="CONSTTK")
		parseConstDecl();
	
	//decl은 출력 안해도 되기 때문에 생략!
}

// Block → '{' { BlockItem } '}'   
// + BlockItem → Decl | Stmt  부분 해결
// 이게 제일 중요. 함수의 모든 문장을 한문장씩 읽는다. 여기서 시동되는 함수는 한 줄이 끝나야지만 돌아올수있다
// 이 함수가 끝나면 메인 함수가 끝난것이다.
void parseBlock()
{
	Syntax_Analysis_Main(true, true); // {
	while(couple[0]!="RBRACE")
	{
		//if Decl
		if (couple[0] == "INTTK" || couple[0] == "CONSTTK")
		{
			parseDecl();
		}
		//if Stmt
		else{
			parseStmt();
		}
	}
	Syntax_Analysis_Main(true, true); // }

	fout << "<Block>" << endl;
	cout << "<Block>" << endl;
}
void parseFuncRParams()
{
	parseExp();
	while (couple[1] == ",") // , 가 나오는동안
	{
		if (!Syntax_Analysis_Main(true, true)) break; // 일단 , 출력하고 틀리면 끝내기

		parseExp();
	}
	fout << "<FuncRParams>" << endl; 
	cout << "<FuncRParams>" << endl;
}

void parseFuncFParam()
{
	Syntax_Analysis_Main(true, true); //int
	Syntax_Analysis_Main(true, true); //ident
	if(couple[1]=="[") // [ 라면, 즉 배열이라면
	{	
		Syntax_Analysis_Main(true, true);//배열 [
		Syntax_Analysis_Main(true, true);// ]
        //2 혹은 3,4,5,6...
		while(couple[1]=="[") // [
		{
			Syntax_Analysis_Main(true, true);// [
			parseConstExp();
			Syntax_Analysis_Main(true, true); // ]
		}
	}
	fout << "<FuncFParam>" << endl; 
	cout << "<FuncFParam>" << endl;
}

void parseFuncFParams()
{
	parseFuncFParam();
	while(couple[0]=="COMMA")// ; 가 나오는동안
	{
		if (!Syntax_Analysis_Main(true, true)) break; // 일단 , 출력하고 틀리면 끝내기

		parseFuncFParam();
	}
	fout << "<FuncFParams>" << endl; 
	cout << "<FuncFParams>" << endl;
}

// 나머지 함수일때 사용
void parseFuncDef()
{
	Syntax_Analysis_Main(true, true); // void, int 구분할 필요가 없는게 return에서 차피 구분하게 만들어놓음
	fout << "<FuncType>" << endl; 
	cout << "<FuncType>" << endl;
	Syntax_Analysis_Main(true, true); // Ident : 함수이름
	Syntax_Analysis_Main(true, true); // '('
	if(couple[0]!="RPARENT")
		parseFuncFParams();
	Syntax_Analysis_Main(true, true); // ')'
	parseBlock();

	fout << "<FuncDef>" << endl; 
	cout << "<FuncDef>" << endl;
}

//메인 함수일떄 사용
void parseMainFunc()
{
	Syntax_Analysis_Main(true, true); //int
	Syntax_Analysis_Main(true, true); //main
	Syntax_Analysis_Main(true, true); // ( 
	Syntax_Analysis_Main(true, true); // )
	parseBlock();
	fout << "<MainFuncDef>" << endl; 
	cout << "<MainFuncDef>" << endl;
}

// 첫번째로 실행되는 함수. compUnit -> {Decl} {FuncDef} MainFuncDef 부분을 해결한다.
// 이 함수가 끝나면 Syntax_Analysis는 끝난다. 끝나고 CompUnit출력
void parseCompUnit()
{
	string temp1 = couple[0];
	Syntax_Analysis_Main(true, false);
	string isMain = couple[0];
	Syntax_Analysis_Main(true, false);
	string temp3 = couple[0];
	Syntax_Analysis_Main(false, false);
	Syntax_Analysis_Main(false, false);
	// 메인 함수라면
	if(temp1=="INTTK"&&isMain=="MAINTK")
	{
		parseMainFunc();
		fout << "<CompUnit>" << endl;
		cout << "<CompUnit>" << endl;
		return;
	}
	// 함수라면
	else if((temp1=="VOIDTK"&&temp3=="LPARENT")||(temp1=="INTTK"&&temp3=="LPARENT"))
	{
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

int main(void) {
  init(); 
  Lexical_Analysis();
  Syntax_Analysis();

  fclose(inputFile);
  fout.close();
  return 0;
}

/*
****  initVal is a good example of [], {}, | ****

{} : fanfu more than 0
ex : FuncFParams -> FuncFParam{ ',' FuncParam } // if this
while(couple[1]==",")
{
    if(!Syntex_Analysis_Main(true,true)) break;
    ParseFuncFParam();
}
fout<<"<FuncFParams>"<<endl;
// daima is this. 



문법
 // 0-> 1.是否存在Decl 2.是否存在FuncDef
编译单元 CompUnit → {Decl} {FuncDef} MainFuncDef

// 1<-0 覆盖两种声明
声明 Decl → ConstDecl | VarDecl
基本类型 BType → 'int' // 存在即可

//ConstDecl부분
// 2<-1 1.花括号内重复0 次 2.花括号内重复多次
常量声明 ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';'
// 3<-2 변수, 1차원, 2차원 배열을 포함
常数定义 ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
// 4<-3 위에서 선언한 변수, 1차원, 2차원 배열을 초기화
常量初值 ConstInitVal → ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'

// 5<-1 1花括号内重复0次 2.花括号内重复多次
变量声明 VarDecl → BType VarDef { ',' VarDef } ';'
// 包含普通变量、一维数组、二维数组定义
变量定义 VarDef → Ident { '[' ConstExp ']' } | Ident { '[' ConstExp ']' } '=' InitVal
// 1.表达式初值 2.一维数 组初值 3.二维数组初值
变量初值 InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'
 -> a. Exp : 1
 -> b. { Exp } : {1}
 -> c. { Exp, Exp ... } : { 1, 2, 3 }
 -> d. { initVal } -> {{ InitVal }} -> {{Exp}} : {{1,2}}
 -> e.                {{ InitVal }} -> {{Exp,Exp...}} : {{1,2},{3,4}}


// 0. 메인 함수
主函数定义 MainFuncDef → 'int' 'main' '(' ')' Block

// 일반 함수
函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block // 1.无形参 2.有形参
// 覆盖两种类型的函数
函数类型 FuncType → 'void' | 'int'
// 1. 1.花括号内重复0次 2.花括号内重复多次
函数形参表 FuncFParams → FuncFParam { ',' FuncFParam }
// 1.普通变量 2.一维数组变量 3.二维数组变量<- 이거안함
函数形参 FuncFParam → BType Ident ['[' ']' { '[' ConstExp ']' }]




// 1.花括号内重复0次 2.花括号内重复多次
语句块 Block → '{' { BlockItem } '}'
// 覆盖两种语句块项
语句块项 BlockItem → Decl | Stmt

// 每种类型的语句都要覆盖
语句 Stmt →
LVal '=' Exp ';'
| [Exp] ';' //有无Exp两种情况
| Block
| 'if' '(' Cond ')' Stmt [ 'else' Stmt ] // 1.有else 2.无else
//1. 无缺省 2. 缺省第一个 ForStmt 3. 缺省Cond 4. 缺省第二个ForStmt
| 'for' '(' [ForStmt] ';' [Cond] ';' [ForStmt] ')' Stmt
| 'break' ';' | 'continue' ';'
| 'return' [Exp] ';' // 1.有Exp 2.无Exp
| LVal '=' 'getint''('')'';'
| 'printf''('FormatString{','Exp}')'';' // 1.有Exp 2.无Exp

语句 ForStmt → LVal '=' Exp // 存在即可
表达式 Exp → AddExp 注:SysY 表达式是int 型表达式 // 存在即可
条件表达式 Cond → LOrExp // 存在即可
// 1.普通变量 2.一维数组 3.二维数组
左值表达式 LVal → Ident {'[' Exp ']'}

// 3
// 三种情况均需覆盖
基本表达式 PrimaryExp → '(' Exp ')' | LVal | Number
数值 Number → IntConst // 存在即可

// 3种情况均需覆盖,函数调用也需要覆盖FuncRParams的不同情况
一元表达式 UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp // 存在即可
单目运算符 UnaryOp → '+' | '−' | '!' 注:'!'仅出现在条件表达式中 // 三种均需覆盖
// 1.花括号内重复0次 2.花括号内重复多次 3.Exp需要覆盖数组传参和部分数组传参
函数实参表 FuncRParams → Exp { ',' Exp }


// 1// 1.UnaryExp 2.* 3./ 4.% 均需覆盖
乘除模表达式 MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp // 1.MulExp 2.+ 需覆盖 3.- 需 覆盖

关系表达式 RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp // 1.AddExp 2.< 3.> 4.<= 5.>= 均需覆盖
相等性表达式 EqExp → RelExp | EqExp ('==' | '!=') RelExp // 1.RelExp 2.== 3.!= 均需 覆盖

逻辑与表达式 LAndExp → EqExp | LAndExp '&&' EqExp // 1.EqExp 2.&& 均需覆盖
逻辑或表达式 LOrExp → LAndExp | LOrExp '||' LAndExp // 1.LAndExp 2.|| 均需覆盖
常量表达式 ConstExp → AddExp 注:使用的Ident 必须是常量 // 存在即可
 */

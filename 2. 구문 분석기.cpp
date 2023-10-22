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
void parseFuncParams();
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
	while(inputedString[wordCnt]==' ' ||parsingAnnotation()|| inputedString[wordCnt]=='	') {
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
	/* 
			true, true : 출력하고 앞으로 가기
			true, false :그냥 앞으로 가기
			false : 한칸 뒤로가기
		*/
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
	//뒤로가기
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
	if(couple[0]=="LPARENT")
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
			/* int num = stoi(couple[1]); */
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
		// 아직 없음 7. Ident '(' [FuncParams] '); 부분
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
	while(couple[0]=="MULT"||couple[0]=="DIV"||couple[0]=="MOD")
	{
		cout << "<MulExp>" << endl;
		fout << "<MulExp>" << endl;
		Syntax_Analysis_Main(true, true);
		unaryExp();
	}
	cout << "<MulExp>" << endl;
	fout << "<MulExp>" << endl;
}

void addExp(){
	mulExp();
	while(couple[0]=="PLUS"||couple[0]=="MINU")
	{
		cout << "<AddExp>" << endl;
		fout << "<AddExp>" << endl;     
		Syntax_Analysis_Main(true, true);
		mulExp();
	}
	cout << "<AddExp>" << endl;
	fout << "<AddExp>" << endl;
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

void parseReadStmt()
{
	Syntax_Analysis_Main(true, true); //getint
	Syntax_Analysis_Main(true, true); //(
	Syntax_Analysis_Main(true, true); //)
}

//return 문일때
//'return' [Exp] ';' // 1.有Exp 2.无Exp

void parseReturnStmt()
{
	Syntax_Analysis_Main(true, true); //return 출력
	if (couple[0] == "LPARENT") //만약 ( 가 있으면 반환문이 있는것으로 간주,
	{
		/* Syntax_Analysis_Main(true, true); // ( 출력 */
		parseExp(); // return 문의 중 exp부분
		/* Syntax_Analysis_Main(true, true); // ) 출력 */
	}
	//만약 return 문에서 ()없이 그냥 return c가 나왔으면, 
	else if(couple[0]=="IDENFR" || couple[0]=="INTCON"||couple[0]=="MINU")
	{
		parseExp();
	}
	//없으면 종료
}

void parsePrint()
{
	Syntax_Analysis_Main(true, true); //printf
	Syntax_Analysis_Main(true, true); //'('
	Syntax_Analysis_Main(true, true);//formatString

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
	//여기 현재 LBRACE { 라고 되어있는데 암만봐도 LBRACK [같다. 
	while(couple[1]=="[")
	{
		Syntax_Analysis_Main(true, true);//[ 출력
		parseExp();
		Syntax_Analysis_Main(true, true); //] 출력
	}
	cout << "<LVal>" << endl;
	fout << "<LVal>" << endl;
}

// < > <= >=
void parseRelExp()
{
	addExp();
	while(couple[0]=="LSS"||couple[0]=="LEQ"||couple[0]=="GRE"||couple[0]=="GEQ")
	{
		cout << "<RelExp>" << endl;
		fout << "<RelExp>" << endl;
		Syntax_Analysis_Main(true, true); // < > <= >=
		addExp();
	}
	cout << "<RelExp>" << endl;
	fout << "<RelExp>" << endl;
}

void parseEqExp()
{
	parseRelExp();
	while(couple[0]=="EQL"||couple[0]=="NEQ")
	{
		cout << "<EqExp>" << endl;
		fout << "<EqExp>" << endl;
		Syntax_Analysis_Main(true, true); // == !=
		parseRelExp();
	}
	cout << "<EqExp>" << endl;
	fout << "<EqExp>" << endl;
}

void parseLAndExp()
{
	parseEqExp();
	while(couple[0]=="AND")
	{
		cout << "<LAndExp>" << endl;
		fout << "<LAndExp>" << endl;
		Syntax_Analysis_Main(true, true); // &&
		parseEqExp();
	}
	cout << "<LAndExp>" << endl;
	fout << "<LAndExp>" << endl;
}

void parseLOrExp()
{
	parseLAndExp();
	while(couple[0]=="OR")
	{
		cout << "<LOrExp>" << endl;
		fout << "<LOrExp>" << endl;
		Syntax_Analysis_Main(true, true); // ||
		parseLAndExp();
	}
	cout << "<LOrExp>" << endl;
	fout << "<LOrExp>" << endl;
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
	if(couple[0]!="SEMICN")
	{
		parseForStmt();
	}
	Syntax_Analysis_Main(true, true); //;
	if(couple[0]!="SEMICN")
	{
		parseCond();
	}
	Syntax_Analysis_Main(true, true); // ;
	if(couple[1]!=")")
		parseForStmt();
	Syntax_Analysis_Main(true, true);	// )
	parseStmt();
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
	// break,continue, 
	else if (couple[0] == "BREAKTK" || couple[0] == "CONTINUETK")
	{
		Syntax_Analysis_Main(true, true); 
		Syntax_Analysis_Main(true, true); // ; 출력
	}
	//;
	else if (couple[0] == "SEMICN")
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
	else if(couple[0]=="LBRACE")
	{
		parseBlock();
	}
	// 1. stmt -> LVal = Exp;
	// 2. stmt -> LVal = getint();
	// 3 exp; //이게 함수를 부르는 거였네~~
	// 결국 else는 ident인 상황
	else
	{
		//LVel판단
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
		//LVal판단하려고 앞으로 간거 다시 초기화
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
	if(couple[0]=="LBRACE") // { 즉 초기화되는 변수가 배열인 경우
	{
		Syntax_Analysis_Main(true, true); // {
		//만약 2차원 배열인 경우
		if(couple[0]=="LBRACE")
		{
			while(couple[0]=="LBRACE") // { 인 동안
			{
				Syntax_Analysis_Main(true, true); // {

				while(couple[0]!="RBRACE")
				{
					parseConstExp(); //a=b등 변수로 초기화되면 LVal, a=10처럼 숫자로 초기화 되면 number
					cout << "<ConstInitVal>" << endl;
					fout << "<ConstInitVal>" << endl;
					if(couple[0]=="RBRACE")
							break;

					Syntax_Analysis_Main(true, true); // ','출력
				}
				Syntax_Analysis_Main(true, true); // }
				cout << "<ConstInitVal>" << endl;
				fout << "<ConstInitVal>" << endl;
				if(couple[0]=="COMMA")Syntax_Analysis_Main(true, true); // ','출력
			}

			Syntax_Analysis_Main(true, true); // }
		}
		//1차원 배열인 경우
		else
		{    
			while(couple[0]!="RBRACE")
			{
				parseConstExp(); //a=b등 변수로 초기화되면 LVal, a=10처럼 숫자로 초기화 되면 number
				cout << "<ConstInitVal>" << endl;
				fout << "<ConstInitVal>" << endl;

				if(couple[0]=="RBRACE")
					break;

				Syntax_Analysis_Main(true, true); // ','출력
			}
			Syntax_Analysis_Main(true, true); // }
		}
	}
	else //일반 변수일 경우
	{
		parseConstExp();
	}
	fout << "<ConstInitVal>" << endl;
	cout << "<ConstInitVal>" << endl;
}

void parseConstDef()
{
	//지금이 const. 이 부분이 필요한게, int a,b,c등이면 int가 매번 없어서 출력할필요 없어.
	if(couple[0]=="CONSTTK")
	{
		Syntax_Analysis_Main(true, true); //CONST 출력
		Syntax_Analysis_Main(true, true); //int 출력
	}
	Syntax_Analysis_Main(true, true); //c 출력

	if (couple[0] == "LBRACK") // [ (배열)이면
	{
		Syntax_Analysis_Main(true, true); // [
		parseConstExp(); //숫자
		Syntax_Analysis_Main(true, true); // ]
		if (couple[0] == "LBRACK") // [ 이면 (2중 배열)
		{
			Syntax_Analysis_Main(true, true); //[
			parseConstExp(); //숫자
			Syntax_Analysis_Main(true, true); //]
		}

		//초기화 부분
		if (couple[0] == "ASSIGN") // =
		{
			Syntax_Analysis_Main(true, true); // =
			parseConstInitVal();
		}
	}
	else // 일반 변수면
	{
		if (couple[0] == "ASSIGN") // = 즉 초기화가 있으면
		{
			Syntax_Analysis_Main(true, true); // = 출력
			parseConstInitVal();
		}
		//초기화 없으면 생략
	}
	fout << "<ConstDef>" << endl;
	cout << "<ConstDef>" << endl;
}

// 변수 2: 변수를 처음 선언할때, 모두 입력받고, <ConstDecl>출력
void parseConstDecl()
{
	parseConstDef();
	while(couple[0]=="COMMA")// ; 가 나오는동안
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
	if(couple[0]=="LBRACE") // { 즉 초기화되는 변수가 배열인 경우
	{
		Syntax_Analysis_Main(true, true); // {
		//만약 2차원 배열인 경우
		if(couple[0]=="LBRACE")
		{
			while(couple[0]=="LBRACE") // { 인 동안
			{
				Syntax_Analysis_Main(true, true); // {

				while(couple[0]!="RBRACE")
				{
					parseExp(); //a=b등 변수로 초기화되면 LVal, a=10처럼 숫자로 초기화 되면 number

					fout << "<InitVal>" << endl;
					cout << "<InitVal>" << endl;
					if(couple[0]=="RBRACE")
						break;

					Syntax_Analysis_Main(true, true); // ','출력
				}
				Syntax_Analysis_Main(true, true); // }
				fout << "<InitVal>" << endl;
				cout << "<InitVal>" << endl;
				if(couple[0]=="COMMA")Syntax_Analysis_Main(true, true); // ','출력
			}

			Syntax_Analysis_Main(true, true); // }
		}
		//1차원 배열인 경우
		else
		{    
			while(couple[0]!="RBRACE")
			{
					parseExp(); //a=b등 변수로 초기화되면 LVal, a=10처럼 숫자로 초기화 되면 number
					fout << "<InitVal>" << endl;
					cout << "<InitVal>" << endl;

					if(couple[0]=="RBRACE")
							break;

					Syntax_Analysis_Main(true, true); // ','출력
			}
			Syntax_Analysis_Main(true, true); // }
		}
	}
	else //일반 변수일 경우
	{
		parseExp();
	}
	fout << "<InitVal>" << endl;
	cout << "<InitVal>" << endl;
}

void parseVarDef()
{
	//지금이 int라면. 이 부분이 필요한게, int a,b,c등이면 int가 매번 없어서 출력할필요 없어.
	if(couple[0]=="INTTK")
		Syntax_Analysis_Main(true, true); //int 출력

	Syntax_Analysis_Main(true, true); //c 출력

	if (couple[0] == "LBRACK") // [ (배열)이면
	{
		Syntax_Analysis_Main(true, true); // [
		parseConstExp(); //숫자
		Syntax_Analysis_Main(true, true); // ]
		if (couple[0] == "LBRACK") // [ 이면 (2중 배열)
		{
			Syntax_Analysis_Main(true, true); //[
			parseConstExp(); //숫자
			Syntax_Analysis_Main(true, true); //]
		}

		//초기화 부분
		if (couple[0] == "ASSIGN") // =
		{
			Syntax_Analysis_Main(true, true); // =
			parseInitVal();
		}
	}
	else // 일반 변수면
	{
		if (couple[0] == "ASSIGN") // = 즉 초기화가 있으면
		{
			Syntax_Analysis_Main(true, true); // = 출력
			parseInitVal();
		}
		//초기화 없으면 생략
	}
	fout << "<VarDef>" << endl;
	cout << "<VarDef>" << endl;
}

// 변수 2: 변수를 처음 선언할때, 모두 입력받고, <VarDecl>출력
void parseVarDecl()
{
	parseVarDef();
	while(couple[0]=="COMMA")// ; 가 나오는동안
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
	while (couple[0] == "COMMA") // , 가 나오는동안
	{
		if (!Syntax_Analysis_Main(true, true)) break; // 일단 , 출력하고 틀리면 끝내기

		parseExp();
	}
	fout << "<FuncRParams>" << endl; 
	cout << "<FuncRParams>" << endl;
}

void parseFuncParam()
{
	Syntax_Analysis_Main(true, true); //int
	Syntax_Analysis_Main(true, true); //ident
	if(couple[0]=="LBRACK") // [ 라면, 즉 배열이라면
	{	
		Syntax_Analysis_Main(true, true);//배열 [
		Syntax_Analysis_Main(true, true);// ]
		if(couple[0]=="LBRACK")
		{
			Syntax_Analysis_Main(true, true);// [
			parseConstExp();
			Syntax_Analysis_Main(true, true); // ]
		}
	}
	fout << "<FuncFParam>" << endl; 
	cout << "<FuncFParam>" << endl;
}

void parseFuncParams()
{
	parseFuncParam();
	while(couple[0]=="COMMA")// ; 가 나오는동안
	{
		if (!Syntax_Analysis_Main(true, true)) break; // 일단 , 출력하고 틀리면 끝내기

		parseFuncParam();
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
		parseFuncParams();
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
	string temp2 = couple[0];
	Syntax_Analysis_Main(true, false);
	string temp3 = couple[0];
	Syntax_Analysis_Main(false, false);
	Syntax_Analysis_Main(false, false);
	// 메인 함수라면
	if(temp1=="INTTK"&&temp2=="MAINTK")
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

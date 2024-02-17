#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
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
ofstream fout("output.txt");
ofstream errorOut("error.txt");
ofstream pcodeOut("pcoderesult.txt");

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

struct Symbol22
{
	string idenfr;
	int kind;
	int type;
	int level;
};
Symbol22 symbolTable[10000];
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

class Label{
    private:
        int point = 0;

    public:
        Label() { }

        int getPoint() {
            return point;
        }

        void setPoint(int point) {
            this->point = point;
        }
};

class Code{
    private:
        string name;
        int level=0;
        int addr=0;
        string print="";
        Label* label=nullptr; // Label 클래스가 정의되어 있어야 합니다.

        int type = 0;

    public:
        // 1, 2, 3, 4, 5는 각 생성자의 타입을 나타냅니다.
        // 1: name, level, addr
        // 2: name, addr
        // 3: name
        // 4: name, print
        // 5: name, label
		Code() { } // 기본 생성자 추가

        Code(string name, int level, int addr) : name(name), level(level), addr(addr), type(1) { }

        Code(string name, int addr) : name(name), addr(addr), type(2) { }

        Code(string name) : name(name), type(3) { }

        Code(string name, string print) : name(name), print(print), type(4) { }

        Code(string name, Label* label) : name(name), label(label), type(5) { }

        void show(){
            if(type == 1){
                cout << name << "  " << level << "  " << addr << endl;
            }else if(type == 2){
                cout << name << "  " << addr << endl;
            }else if(type == 3){
                cout << name << endl;
            }else if(type == 4){
                cout << name << "  " << print << endl;
            }else if(type == 5){
                cout << name << "  " << label->getPoint() << endl; // Label 클래스의 getPoint() 함수가 필요합니다.
            }
        }

        string getName(){
            return name;
        }

        Label* getLabel(){
            return label;
        }

        int getLevel(){
            return level;
        }

        int getAddr(){
            return addr;
        }

        string getPrint(){
            return print;
        }
};
vector<Code> codeList;
int codeAt = 0;

////////////



class Symbol {
private:
    string name;
    int dim;
    int dim1 = 0;
    int dim2 = 0;
    int address = 0;
    bool isConst = false;
    bool isGlobal = false;
public:
    Symbol(string name, int dim) : name(name), dim(dim) {}

    bool operator==(const Symbol& sym) {
        return this->name == sym.name;
    }

    virtual void show_value() {}

    // 이하 getter, setter 함수는 자바 코드와 유사하게 변환하였습니다.

    int getAddress() {
        return address;
    }

    void setAddress(int address) {
        this->address = address;
    }

    string getName() {
        return name;
    }

    void setName(string name) {
        this->name = name;
    }

    int getDim() {
        return dim;
    }

    void setDim(int dim) {
        this->dim = dim;
    }

    int getDim1() {
        return dim1;
    }

    void setDim1(int dim1) {
        this->dim1 = dim1;
    }

    int getDim2() {
        return dim2;
    }

    void setDim2(int dim2) {
        this->dim2 = dim2;
    }

	//c++은 오버라이드 안되니까 isConst->getIsConst로 바꾼다
    bool getIsConst() {
        return isConst;
    }

    void setConst(bool aConst) {
        isConst = aConst;
    }
	//오버라이드 되니까 isGlobal -> getIsGlobal로 바꾼다
    bool getIsGlobal() {
        return isGlobal;
    }

    void setGlobal(bool global) {
        isGlobal = global;
    }

    virtual string getType() {
        return "";
    }
};

class Block {
private:
    string type;
    vector<Block*> CBlock;
    Block* FBlock;
    vector<Symbol*> SymbolTable;
    int level;
    bool returnTk;
public:
    Block(string type, Block* FBlock, int level)
        : type(type), FBlock(FBlock), level(level), returnTk(false) {}
    
    void addCBlock(Block* block) {
        this->CBlock.push_back(block);
    }

    bool addSymbol(Symbol* symbol) {
        if(find(SymbolTable.begin(), SymbolTable.end(), symbol) == SymbolTable.end()){
            SymbolTable.push_back(symbol);
            return true;
        }else {
            return false;
        }
    }

    bool containSymbol(Symbol* symbol) {
        return find(SymbolTable.begin(), SymbolTable.end(), symbol) != SymbolTable.end();
    }

    Symbol* search(string str) {
        Block* searchBlock = this;
        Symbol* target = nullptr;
        bool found = false;
        while(searchBlock != nullptr && !found){
            for (Symbol* symbol : searchBlock->getSymbolTable()) {
                if (symbol->getName() == str) {
                    found = true;
                    target = symbol;
                    break;
                }
            }
            searchBlock = searchBlock->getFBlock();
        }
        return target;
    }

    // 이하 getter, setter 및 show 함수는 자바 코드와 유사하게 변환하였습니다.

    vector<Block*> getCBlock() {
        return CBlock;
    }

    Block* getFBlock() {
        return FBlock;
    }

    vector<Symbol*> getSymbolTable() {
        return SymbolTable;
    }
	
    bool isReturnTk() {
        return returnTk;
    }

    int getLevel() {
        return level;
    }

    string getType() {
        return type;
    }
    void setBlockItems(vector<Symbol*> blockItems) {
        this->SymbolTable = blockItems;
    }

    void setFBlock(Block* FBlock) {
        this->FBlock = FBlock;
    }

    void setLevel(int level) {
        this->level = level;
    }

    void setReturnTk(bool returnTk) {
        this->returnTk = returnTk;
    }

    void setType(string type) {
        this->type = type;
    }
};

class Param_symbol : public Symbol {
private:
    string type = "param";
public:
    Param_symbol(string name, int dim) : Symbol(name, dim) {}

    bool operator==(const Param_symbol& sym) {
        return Symbol::operator==(sym);
    }

    string getType() {
        return type;
    }

    void setType(string type) {
        this->type = type;
    }
};
class Var_symbol : public Symbol {
private:
    string type = "var";
public:
    Var_symbol(string name, int dim) : Symbol(name, dim) {}

    bool operator==(const Var_symbol& sym) {
        return Symbol::operator==(sym);
    }

    string getType() {
        return type;
    }

    void setType(string type) {
        this->type = type;
    }
};

class Func_symbol : public Symbol {
private:
    string type = "func";
    vector<Param_symbol*> params;
    int startCode;
public:
    Func_symbol(string name, int dim) : Symbol(name, dim) {}

    void addParam(Param_symbol* param_symbol) {
        this->params.push_back(param_symbol);
    }

    bool operator==(const Func_symbol& sym) {
        return Symbol::operator==(sym);
    }

    void show() {
        for (Param_symbol* param : params) {
            cout << "param: " << param->getName() << "\n";
        }
    }

    string getType() {
        return type;
    }

    void setType(string type) {
        this->type = type;
    }

    int getStartCode() {
        return startCode;
    }

    vector<Param_symbol*> getParams() {
        return params;
    }

    void setParams(vector<Param_symbol*> params) {
        this->params = params;
    }

    void setStartCode(int startCode) {
        this->startCode = startCode;
    }
};

class Const_symbol : public Symbol {
private:
    string type = "const";
    vector<int> values;

public:
    Const_symbol(string name, int dim) : Symbol(name, dim) {
        setConst(true);
    }

    void setType(string type) {
        this->type = type;
    }

    string getType() {
        return type;
    }

    void setValues(vector<int> values) {
        this->values = values;
    }

    vector<int> getValues() {
        return values;
    }
};

class Record {
private:
    int retValue;

public:
    Record(int retValue)
        :  retValue(retValue) {}

    int getRetValue() {
        return retValue;
    }

    void setRetValue(int retValue) {
        this->retValue = retValue;
    }
};


class RecordValue {
private:
    vector<int> values;

public:
    RecordValue(vector<int> values)
        : values(values) {}

    void setValues(vector<int> values) {
        this->values = values;
    }

    vector<int> getValues() {
        return values;
    }
};
Record ExpValue();
Record LValValue();
int level = 1;
Block *curBlock = new Block("global", NULL, level);
Func_symbol *cur_func_symbol = NULL;
int address = 0;
bool isMainFunc = false;
bool isIntFunc = false;
int flag = 0;
bool need_lods = true;
int isFor = 0;
vector<Label*> forLabel1;
vector<Label*> forLabel2;
/////////////////

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
			//11
			Code code1("LDC", stoi(wordNow.idenfr));
			codeList.push_back(code1);
			Syntax_Analysis_Main(true, true);
			outputSyntax_Analysis("Number");
		}
		// 3. LVal
		else 
		{
			need_lods = true;
			type = parseLVal(type);
			//11
			if(need_lods){
				Code code1("LODS");
				codeList.push_back(code1);
			}
		}
	}
	outputSyntax_Analysis("PrimaryExp");

	return type;
}

Record PrimaryExpValue(){
	Record *record = new Record(0);
	int value;
	int midValue;

	if(wordNow.idenfr=="(")
	{
		Syntax_Analysis_Main(true, true); //'('출력
		Record record1 = ExpValue();
		value = record1.getRetValue();
		record->setRetValue(value);
		Syntax_Analysis_Main(true, true); //') 출력
	}
	else
	{
		// 2. number
		if(wordNow.idenfr[0]>='0'&&wordNow.idenfr[0]<='9')
		{
			//11
			value = stoi(wordNow.idenfr);
			record->setRetValue(value);
			Syntax_Analysis_Main(true, true);
			outputSyntax_Analysis("Number");
		}
		// 3. LVal
		else 
		{
			Record record2 = LValValue();
			value = record2.getRetValue();
			record->setRetValue(value);
		}
	}
	outputSyntax_Analysis("PrimaryExp");
	return *record;
}

void unaryOP(){
	Syntax_Analysis_Main(true, true); //부호 출력
	outputSyntax_Analysis("UnaryOp");
}

int unaryExp(int type){
	Syntax_Analysis_Main(true, false);
	string temp = wordNow.idenfr;
	Syntax_Analysis_Main(false, false);
	string op;

	//UnaryOp UnaryExp
	if (wordNow.label == "PLUS" || wordNow.label == "MINU" || wordNow.label == "NOT")
	{
		op = wordNow.label;
		unaryOP();
		type = unaryExp(type);

		//22
		if(op=="MINU")
		{
			Code code1("MINU");
			codeList.push_back(code1);
		}
		else if(op=="NOT")
		{
			Code code2("NOT");
			codeList.push_back(code2);
		}
	}//Ident ([FuncRParams])
	else if(wordNow.label=="IDENFR"&&temp=="(")
	{
		//  Ident '(' [FuncParams] ');
		//error handeler
		lastNonTerminalLine = wordNow.lineId;
		parseErrorC(wordNow.idenfr, wordNow.lineId); // error c

		//22
		string func_name = wordNow.idenfr;
		Symbol *symbol = curBlock->search(func_name);
		Code code1("INT", 3);
		codeList.push_back(code1);

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
		Code code2("DOWN", 3 + dynamic_cast<Func_symbol*>(symbol)->getParams().size());
		codeList.push_back(code2);
		
		Code code3("CAL",dynamic_cast<Func_symbol*>(symbol)->getStartCode());
		codeList.push_back(code3);

		// 원래 함수에는 인자가 있는데 지금은 )인 경우
		if (funcInfo.paramNum > 0 && wordNow.idenfr == ")")
            error(wordNow.lineId, "d");
		// ()가 잘 닫히는지 확인하는 오류. 함수에서 )까지 출력한다.
		parseErrorJ();
	}
	else    type=primaryExp(type);
	
	outputSyntax_Analysis("UnaryExp");
	return type;
}

Record UnaryExpValue() {
	string op;
	int midValue;
	int value = 0;
	Record *record = new Record(0);

	Syntax_Analysis_Main(true, false);
	string temp = wordNow.idenfr;
	Syntax_Analysis_Main(false, false);

	//UnaryOp UnaryExp
	if (wordNow.label == "PLUS" || wordNow.label == "MINU" || wordNow.label == "NOT")
	{
		op = wordNow.label;
		unaryOP();

		Record record1 = UnaryExpValue();
		midValue = record1.getRetValue();

		//22
		if(op=="PLUS")
		{
			value = midValue;
		}
		else if(op=="MINU")
		{
			value = -1 * midValue;
		}
		record->setRetValue(value);
		outputSyntax_Analysis("UnaryExp");
	} // Ident ([FuncRParams])
	else if (wordNow.label == "IDENFR" || wordNow.label == "LPARENT" || wordNow.label == "INTCON")
	{
		Record record1 = PrimaryExpValue();
        value = record1.getRetValue();
        outputSyntax_Analysis("UnaryExp");
        record->setRetValue(value);
	}
	return *record;
}

int mulExp(int type){
	type = unaryExp(type);
	
	outputSyntax_Analysis("MulExp");
	while (wordNow.label == "MULT" || wordNow.label == "DIV" || wordNow.label == "MOD")
	{
		string op = wordNow.label;
		Syntax_Analysis_Main(true, true);
		type=unaryExp(type);

		if(op=="MULT")
		{
			Code code1("MUL");
			codeList.push_back(code1);
		}
		else if(op=="DIV")
		{
			Code code1("DIV");
			codeList.push_back(code1);
		}
		else if(op=="MOD")
		{
			Code code1("MOD");
			codeList.push_back(code1);
		}
		outputSyntax_Analysis("MulExp");
	}
	return type;
}


Record mulExpValue(){
    Record *record = new Record(0);
    int value;
    int midValue;
    string op;
    Record record1 = UnaryExpValue();

    value = record1.getRetValue();
	
	outputSyntax_Analysis("MulExp");

	while (wordNow.label == "MULT" || wordNow.label == "DIV" || wordNow.label == "MOD")
	{
		op = wordNow.label;
		Syntax_Analysis_Main(true, true);
		Record record2 = UnaryExpValue();
		
        midValue = record2.getRetValue();

		if(op=="MULT")
		{
			value = value * midValue;
		}
		else if(op=="DIV")
		{
			value = value / midValue;
		}
		else if(op=="MOD")
		{
			value = value % midValue;
		}
		outputSyntax_Analysis("MulExp");
	}
	record->setRetValue(value);
	return *record;
}

int addExp(int type){
	type=mulExp(type);
	
	outputSyntax_Analysis("AddExp");
	while (wordNow.label == "PLUS" || wordNow.label == "MINU")
	{
		string op = wordNow.label;
		Syntax_Analysis_Main(true, true);
		type=mulExp(type);

		if(op=="PLUS")
		{
			Code code1("ADD");
			codeList.push_back(code1);
		}
		else if(op=="MINU")
		{
			Code code1("SUB");
			codeList.push_back(code1);
		}
		outputSyntax_Analysis("AddExp");
	}
	return type;
}
Record AddExpValue() {
	Record *record = new Record(0);
	string op;
	int value;
    int midValue;
    Record record1 = mulExpValue(); 
    value = record1.getRetValue();
	
	outputSyntax_Analysis("AddExp");
	while (wordNow.label == "PLUS" || wordNow.label == "MINU")
	{
		op = wordNow.label;
		Syntax_Analysis_Main(true, true);
		Record record2 = mulExpValue();

		midValue = record2.getRetValue();
		
		if (op == "PLUS")
		{
			value = value + midValue;
		}
		else if(op=="MINU")
		{
			value = value - midValue;
		}
	}
	record->setRetValue(value);
	return *record;
}

int parseExp(){
	int type = INT;
	type=addExp(type);

	outputSyntax_Analysis("Exp");
	return type;
}

Record ExpValue(){
	Record *record = new Record(0);
	int value;
	Record record1 = AddExpValue();
	value = record1.getRetValue();
	outputSyntax_Analysis("Exp");
	record->setRetValue(value);
	return *record;
}

void parseConstExp(){
	addExp(-1);

	outputSyntax_Analysis("ConstExp");
}

Record constExpValue(){
	Record *record=new Record(0);
	int value = 0;

	Record record1 = AddExpValue();
	value = record1.getRetValue();
	
	outputSyntax_Analysis("ConstExp");
	record->setRetValue(value);

	return *record;
}

// < > <= >=
void parseRelExp()
{
	addExp(-1);

	outputSyntax_Analysis("RelExp");
	while (wordNow.idenfr == "<" || wordNow.idenfr == "<=" || wordNow.idenfr == ">" || wordNow.idenfr == ">=")
	{
		string op = wordNow.idenfr;
		Syntax_Analysis_Main(true, true); // < > <= >=
		addExp(-1);

		//22
		if(op==">")
		{
			Code code1("BGT");
			codeList.push_back(code1);
		}
		else if(op==">=")
		{
			Code code2("BGE");
			codeList.push_back(code2);
		}
		else if(op=="<")
		{
			Code code3("BLT");
			codeList.push_back(code3);
		}
		else if(op=="<=")
		{
			Code code4("BLE");
			codeList.push_back(code4);
		}
		outputSyntax_Analysis("RelExp");
	}
}

void parseEqExp()
{
	parseRelExp();
	outputSyntax_Analysis("EqExp");

	while (wordNow.idenfr == "!=" || wordNow.idenfr == "==")
	{
		string op = wordNow.idenfr;
		Syntax_Analysis_Main(true, true); // == !=
		parseRelExp();

		if(op=="==")
		{
			Code code1("BEQ");
			codeList.push_back(code1);
		}
		else if(op=="!=")
		{
			Code code2("BNE");
			codeList.push_back(code2);
		}
		outputSyntax_Analysis("EqExp");
	}
}

void parseLAndExp()
{
	parseEqExp();

	outputSyntax_Analysis("LAndExp");
	while (wordNow.label == "AND")
	{
		Syntax_Analysis_Main(true, true); // &&

		//22
		Label *label = new Label();
		Code code1("JP0", label);
		codeList.push_back(code1);
		Code code2("DOWN", 1);
		codeList.push_back(code2);

		parseEqExp();
		label->setPoint(codeList.size());
		outputSyntax_Analysis("LAndExp");
	}
}

void parseLOrExp()
{
	parseLAndExp();

	outputSyntax_Analysis("LOrExp");
	while (wordNow.label == "OR")
	{
		Syntax_Analysis_Main(true, true); // ||
		//22
		Label *label = new Label();
		Code code1("JP1", label);
		codeList.push_back(code1);
		Code code2("DOWN", 1);
		codeList.push_back(code2);

		parseLAndExp();
		label->setPoint(codeList.size());
		outputSyntax_Analysis("LOrExp");
	}
}

void parseCond(){
	parseLOrExp();
	
	outputSyntax_Analysis("Cond");
}

void parseIf()
{
	//22
	Label *label1 = new Label();
	Label *label2 = new Label();
	
	lastNonTerminalLine = wordNow.lineId;
	Syntax_Analysis_Main(true, true); //if
	Syntax_Analysis_Main(true, true); //(
	parseCond();

	Code code1("BZT", label1);
	codeList.push_back(code1);
	// if하고 {가 같은 줄에 있으면 이거, 다음 줄에 있으면 -1을 해야함.
	//  ()가 잘 닫히는지 확인하는 오류. 함수에서 )까지 출력한다.
	parseErrorJ();
	parseStmt();
	if(wordNow.label=="ELSETK")
	{
		Syntax_Analysis_Main(true, true); //else

		Code code2("J", label2);
		codeList.push_back(code2);
		label1->setPoint(codeList.size());

		parseStmt();

		label2->setPoint(codeList.size());
	}else{
		label1->setPoint(codeList.size());
	}
}

void parseForStmt()
{
	parseErrorH(wordNow.idenfr, wordNow.lineId); 
	parseLVal(-1);
	Syntax_Analysis_Main(true, true); // =
	parseExp();

	Code code("STOS");
	codeList.push_back(code);

	outputSyntax_Analysis("ForStmt");
}

void parseFor()
{
	//만약 2 -> 3 이면, 2에 Code추가, 3에 라벨 추가.
	/* for( 1 ; (2,5) ; 4){
		3
	} 6*/
	int oneNotCalled = true;
	int twoNotCalled = true;
	int threeNotCalled = true;
	Label *labelZeroToThree = new Label();
	Label *labelOneToThree = new Label();
	Label *labelTwoToThree = new Label();  // J to 3 { block } 2 -> 3
	Label *labelThreeToTwo = new Label();
	Label *labelThreeToThree = new Label();
	Label *labelThreeToFour = new Label(); // J to for의 3번째( i++ ) 3 -> 4
	Label *labelFourToThree = new Label();
	Label *labelFourToFive = new Label(); // J to for의 2번째 ( i<3 ) 4 -> 5(2)
	Label *labelToBreak = new Label(); // BZT to 6; 2 -> 6, 5 -> 6

	isFor++;
	forLabel1.push_back(labelToBreak); // break을 위한 for 밖으로 나가는 라벨
	
	
	Syntax_Analysis_Main(true, true); // for
	Syntax_Analysis_Main(true, true); // (
	if (wordNow.idenfr != ";")
	{
		oneNotCalled = false;
		parseForStmt();
	}
	Syntax_Analysis_Main(true, true); //;
	if (wordNow.idenfr != ";")
	{
		// 만약 4번이 없을시
		labelThreeToTwo->setPoint(codeList.size());
		
		twoNotCalled = false;
		labelFourToFive->setPoint(codeList.size());

		parseCond();
		Code codeToBreak("BZT", labelToBreak);
		codeList.push_back(codeToBreak);
		Code codeTwoToThree("J", labelTwoToThree);
		codeList.push_back(codeTwoToThree);
	}
	Syntax_Analysis_Main(true, true); // ;
	if(wordNow.idenfr!=")")
	{
		threeNotCalled = false;
		//12 || 1
		if (twoNotCalled)
		{	
			Code codeZeroToThree("J", labelZeroToThree);
			codeList.push_back(codeZeroToThree);
			labelThreeToFour->setPoint(codeList.size());

			parseForStmt();

			Code codeFourToThree("J", labelFourToThree);
			codeList.push_back(codeFourToThree);
		}
		else
		{
			labelThreeToFour->setPoint(codeList.size());
			parseForStmt();
			Code codeFourToFive("J", labelFourToFive);
			codeList.push_back(codeFourToFive);
		}
	}

	// ()가 잘 닫히는지 확인하는 오류. 함수에서 )까지 출력한다.
	parseErrorJ();

	int numOfBlocks = 0;
	int temp = wordCnt-1;
	while (wordList[temp++].idenfr == "{")
		numOfBlocks++;

	isCirculer[levelNow + numOfBlocks] = true;

	if(twoNotCalled){
		//2
		if(!oneNotCalled && !threeNotCalled)
		{
			labelOneToThree->setPoint(codeList.size());
		}
		//12
		else if(oneNotCalled && !threeNotCalled){
		}
		labelFourToThree->setPoint(codeList.size());
		labelZeroToThree->setPoint(codeList.size());
	}
	else if(!twoNotCalled)
	{
		labelTwoToThree->setPoint(codeList.size());
	}
	labelThreeToThree->setPoint(codeList.size());

	//forLabel2.push_back(labelThreeToThree);을 위한 코드
	if(twoNotCalled && threeNotCalled){
		forLabel2.push_back(labelThreeToThree);
	}
	else if(threeNotCalled && !twoNotCalled){
		forLabel2.push_back(labelThreeToTwo);
	}
	else if(!twoNotCalled && !threeNotCalled){
		forLabel2.push_back(labelThreeToFour);
	}
	parseStmt();

	if(threeNotCalled)
	{
		//3
		if(!oneNotCalled&&!twoNotCalled)
		{
			Code codeThreeToTwo("J", labelThreeToTwo);
			codeList.push_back(codeThreeToTwo);
		}
		//23
		else if(!oneNotCalled && twoNotCalled)
		{
			Code codeThreeToThree("J", labelThreeToThree);
			codeList.push_back(codeThreeToThree);
		}
		//2
		else if (oneNotCalled && !twoNotCalled)
		{
			Code codeThreeToTwo("J", labelThreeToTwo);
			codeList.push_back(codeThreeToTwo);
		}
		//123
		else if(oneNotCalled&&twoNotCalled&&threeNotCalled)
		{
			Code codeThreeToThree("J", labelThreeToThree);
			codeList.push_back(codeThreeToThree);
		}
	}
	else if(!threeNotCalled)
	{
		Code codeThreeToFour("J", labelThreeToFour);
		codeList.push_back(codeThreeToFour);
	}
	labelToBreak->setPoint(codeList.size());
	isFor--;
	forLabel1.pop_back();
	forLabel2.pop_back();
}

void parseReadStmt()
{
	Syntax_Analysis_Main(true, true); //getint
	Syntax_Analysis_Main(true, true); //(
	Syntax_Analysis_Main(true, true); //)
}

void parseReturnStmt()
{
	//22
	if(isIntFunc)
	{
		Code code1("LDA", 0, 0);
		codeList.push_back(code1);
	}

	isReturnCalled = true;
	Syntax_Analysis_Main(true, true); // return 출력

	// 함수가 반환값이 있다면
	if (wordNow.idenfr != ";")
	{
		if(currentFunctionType==VOID)
			error(wordNow.lineId, "f");
		parseExp();
	}
	//22
	if(isIntFunc)
	{
		Code code2("STOS");
		codeList.push_back(code2);
	}
	if(isMainFunc)
	{
		Code code3("RET_TO_END");
		codeList.push_back(code3);
	}
	else
	{
		Code code3("RET");
		codeList.push_back(code3);
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
	string strcon = wordNow.idenfr;
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
	Code code1("PRF", strcon);
	codeList.push_back(code1);
}

int parseLVal(int type)
{
	int dim = 0;
	int arrNum = 0;//cishu
	// error handeler getType은 선언됐던 타입이 나옴.
	int getType = parseErrorC(wordNow.idenfr, wordNow.lineId);
	if(getType!=-1)
		type = getType;
	
	//22
	string lval_name = wordNow.idenfr;
	Symbol *symbol = curBlock->search(lval_name);
	dim = symbol->getDim();
	Syntax_Analysis_Main(true, true); // IDent 출력;
	while(wordNow.idenfr=="[")
	{
		arrNum++;

		Syntax_Analysis_Main(true, true); //[ 출력
		parseExp();
		if (arrNum == 1 && dim == 2){
			Code code1("LDC", symbol->getDim1());
			Code code2("MUL");
			codeList.push_back(code1);
			codeList.push_back(code2);
		}

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
	//여기 보완해야할거 씹많음
	if(symbol!=NULL){
		int isGlobal = symbol->getIsGlobal() ? 1 : 0; // 
		if(symbol->getDim()==0){
			Code code1("LDA", isGlobal, symbol->getAddress());
			codeList.push_back(code1);
		}
		else if (symbol->getDim() == 1 && arrNum == 1)
		{
			if(symbol->getType()=="param"){
				Code code1("LOD", isGlobal, symbol->getAddress());
				codeList.push_back(code1);
			}else{
				Code code1("LDA", isGlobal, symbol->getAddress());
				codeList.push_back(code1);
			}
			Code code2("ADD");
			codeList.push_back(code2);
		}
		else if (symbol->getDim() == 2 && arrNum == 2){
			if(symbol->getType()=="param"){
				Code code1("LOD", isGlobal, symbol->getAddress());
				codeList.push_back(code1);
			}else{
				Code code1("LDA", isGlobal, symbol->getAddress());
				codeList.push_back(code1);
			}
			Code code2("ADD");
			Code code3("ADD");
			codeList.push_back(code2);
			codeList.push_back(code3);
		}
		else if (arrNum == 0 && (dim == 1 || dim == 2))
		{
			if(symbol->getType()=="param"){
				Code code1("LOD", isGlobal, symbol->getAddress());
				codeList.push_back(code1);
			}else{
				Code code1("LDA", isGlobal, symbol->getAddress());
				codeList.push_back(code1);
			}
			need_lods = false;
		}
		else if(arrNum==1&&dim==2){
			if(symbol->getType()=="param"){
				Code code1("LOD", isGlobal, symbol->getAddress());
				codeList.push_back(code1);
			}else{
				Code code1("LDA", isGlobal, symbol->getAddress());
				codeList.push_back(code1);
			}
			Code code2("ADD");
			codeList.push_back(code2);
			need_lods = false;
		}
	}
	outputSyntax_Analysis("LVal");
	return type;
}

Record LValValue(){
	Record *record = new Record(0);
	int dim1 = 0;
	int dim2 = 0;
	int midValue = 0;
	int dim;
	int value = 0;
	
	int arrNum = 0; //cishu
	string lval_name = wordNow.idenfr;
	Symbol *symbol = curBlock->search(lval_name);
	dim = symbol->getDim();

	Syntax_Analysis_Main(true, true); // IDent 출력;
	while(wordNow.idenfr=="[")
	{
		arrNum++;

		Syntax_Analysis_Main(true, true); //[ 출력
		Record record1 = ExpValue();
		midValue = record1.getRetValue();
		if(arrNum==1 && dim==1){
			dim = midValue;
		}
		else if(arrNum==1&&dim==2){
			dim2 = midValue;
		}else if(arrNum==2&&dim==2){
			dim1 = midValue;
		}

		// k error : ]가 있는지 없는지 확인후 있으면 ] 출력
		parseErrorK();
	}
	int off = 0;
	if(dim==0)
		off = 0;
	else if(dim==1)
		off = dim1;
	else if(dim==2)
		off = dim2 * symbol->getDim1() + dim1;
	
	value = dynamic_cast<Const_symbol*>(symbol)->getValues()[off];
	record->setRetValue(value);
	outputSyntax_Analysis("LVal");

	return *record;
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
		if (wordNow.label == "BREAKTK" && isFor > 0)
		{
			Code code1("J", forLabel1.back());
			codeList.push_back(code1);
		}
		if (wordNow.label == "CONTINUETK" && isFor > 0)
		{
			Code code1("J", forLabel2.back());
			codeList.push_back(code1);
		}
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
		//22
		Block *block = new Block("block", curBlock, curBlock->getLevel() + 1);
		block->setFBlock(curBlock);
		curBlock->addCBlock(block);
		curBlock = block;

		parseBlock(false);

		//22
		curBlock = curBlock->getFBlock();
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
				//22
				Code code1("GET");
				codeList.push_back(code1);

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
			//22
			
			Code code2("STOS");
			codeList.push_back(code2);
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

RecordValue parseConstInitVal()
{
	vector<int> values;
	RecordValue *recordValue = new RecordValue(values);

	lastNonTerminalLine = wordNow.lineId;
	if (wordNow.idenfr == "{")
	{
		Syntax_Analysis_Main(true, true);

		if (wordNow.idenfr != "}")
		{
			RecordValue recordValue1 = parseConstInitVal();
			for (int i = 0;i<recordValue1.getValues().size();i++){
				values.push_back(recordValue1.getValues()[i]);
			}
				/* values.insert(values.end(), recordValue1.getValues().begin(), recordValue1.getValues().end()); */

			while(wordNow.idenfr==",")
			{
				Syntax_Analysis_Main(true, true);
				RecordValue recordValue2 = parseConstInitVal();
				/* values.insert(values.end(), recordValue2.getValues().begin(), recordValue2.getValues().end()); */
				for (int i = 0;i<recordValue2.getValues().size();i++){
					values.push_back(recordValue2.getValues()[i]);
				}
			}
		}
		Syntax_Analysis_Main(true, true);
	}
	
	else
	{
		Record record = constExpValue();
		values.push_back(record.getRetValue());
		int value = record.getRetValue();
		Code code1("INT", 1);
		Code code2("LDA", 0, address);
		address++;
		Code code3("LDC", value);
		Code code4("STOS");
		codeList.push_back(code1);
		codeList.push_back(code2);
		codeList.push_back(code3);
		codeList.push_back(code4);
	}
	
	outputSyntax_Analysis("ConstInitVal");
	recordValue->setValues(values);
	return *recordValue;
}

void parseConstDef()
{
	//22
	int dim1 = 0;
	int dim2 = 0;
	int midValue = 0;
	int numOfArrLayer = 0; // cishu

	Const_symbol *const_symbol = new Const_symbol("", 0);
	curBlock->addSymbol(const_symbol);
	const_symbol->setName(wordNow.idenfr);
	// error handeler
	int type = INT;
	if (!parseErrorB(wordNow.idenfr, wordNow.lineId)) // 변수명 확인
	{
		symbolTable[++symbolTableTop] = Symbol22{ wordNow.idenfr, CONST, type, levelNow };
	}
	Syntax_Analysis_Main(true, true); //c 출력
	while (wordNow.idenfr == "[") // [ (배열)이면
	{
		const_symbol->setDim(const_symbol->getDim() + 1);
		numOfArrLayer++;
		Syntax_Analysis_Main(true, true); // [

		Record record1 = constExpValue();
		midValue = record1.getRetValue();
		if(numOfArrLayer==1){
			dim1 = midValue;
		}else if(numOfArrLayer==2){
			dim2 = dim1;
			dim1 = midValue;
		}

		// k error : ]가 있는지 없는지 확인후 있으면 ] 출력
		parseErrorK();
	}

	//22
	const_symbol->setDim1(dim1);
	const_symbol->setDim2(dim2);
	const_symbol->setAddress(address);
	if (flag == 1)
		const_symbol->setGlobal(true);

	if(numOfArrLayer==1)
		symbolTable[symbolTableTop].type = ARR1;
	else if(numOfArrLayer==2)
		symbolTable[symbolTableTop].type = ARR2;

	Syntax_Analysis_Main(true, true); // = 출력
	
	//parseConstInitVal();
	RecordValue RecordValue = parseConstInitVal();
	const_symbol->setValues(RecordValue.getValues());

	//2
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
		//11
		Code code1("INT", 1);
		Code code2("LDA", 0, address);
		codeList.push_back(code1);
		codeList.push_back(code2);
		address++;
		parseExp();
		Code code3("STOS");
		codeList.push_back(code3);
	}

	outputSyntax_Analysis("InitVal");
}

void parseVarDef()
{
	int numOfArrLayer = 0; //cishu와 같다.
	int midValue = 0;
	// arr[2][3]이 있으면, dim1=2, dim2=3이다. 이걸 numOfArrLayer로 값 구하고 복사
	int dim1 = 0;
	int dim2 = 0;
	Var_symbol *var_symbol = new Var_symbol("", 0);
	curBlock->addSymbol(var_symbol);
	var_symbol->setName(wordNow.idenfr);
	// error handeler
	int type = INT;
	if (!parseErrorB(wordNow.idenfr, wordNow.lineId)) // 변수명 확인
	{
		symbolTable[++symbolTableTop] = Symbol22{ wordNow.idenfr, VAR, type, levelNow};
	}
	Syntax_Analysis_Main(true, true); //c 출력
	while (wordNow.idenfr == "[") // [ (배열)이면
	{
		Syntax_Analysis_Main(true, true); // [
		numOfArrLayer++;
		var_symbol->setDim(var_symbol->getDim() + 1);

		/*이거는 arr[2]에서 2를 구할수 없기 떄문에 안쓴다.
		 parseConstExp(); // 숫자 */
		Record record1 = constExpValue();
		//여기서 2를 받는다.
		midValue = record1.getRetValue();
		if(numOfArrLayer==1)
			dim1 = midValue;
		else if(numOfArrLayer==2)
		{
			dim2 = dim1;
			dim1 = midValue;
		}

		// k error : ]가 있는지 없는지 확인후 있으면 ] 출력
		parseErrorK();
	}
	// 22
	var_symbol->setDim1(dim1);
	var_symbol->setDim2(dim2);
	var_symbol->setAddress(address);
	int total = 0;
	if (var_symbol->getDim() == 0)
		total = 1;
	else if (var_symbol->getDim() == 1)
		total = dim1;
	else if (var_symbol->getDim() == 2)
		total = dim1 * dim2;
		
	if(flag==1)
		var_symbol->setGlobal(true);

	if (numOfArrLayer == 1)
		symbolTable[symbolTableTop].type = ARR1;
	else if(numOfArrLayer==2)
		symbolTable[symbolTableTop].type = ARR2;


	if (wordNow.idenfr== "=") // = 즉 초기화가 있으면
	{
	    Syntax_Analysis_Main(true, true); // = 출력
		parseInitVal();
	}
	else{
		for (int i = 0; i < total;i++)
		{
			Code code1("INT", 1);
			Code code2("LDA", 0, address);
			address++;
			Code code3("LDC", 0);
			Code code4("STOS");
			codeList.push_back(code1);
			codeList.push_back(code2);
			codeList.push_back(code3);
			codeList.push_back(code4);
		}
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
	// 22
	Param_symbol *param_symbol = new Param_symbol("", 0);
	string type2;
	string name;
	int dim1 = 0;
	int dim2 = 0;
	int midvalue = 0;

	Syntax_Analysis_Main(true, true); // int

	//error handeler
	int type = INT;
	if (!parseErrorB(wordNow.idenfr, wordNow.lineId)) // 변수명 확인
	{
		symbolTable[++symbolTableTop] = Symbol22{ wordNow.idenfr, PARAM, type, levelNow};
	}

	param_symbol->setName(wordNow.idenfr);
	
	Syntax_Analysis_Main(true, true); // ident
	int numOfArrLayer = 0;
	if (wordNow.idenfr == "[") // [ 라면, 즉 배열이라면
	{
		param_symbol->setDim(1);
		param_symbol->setDim1(0);
		numOfArrLayer++;
		Syntax_Analysis_Main(true, true); // 배열 [

		// k error : ]가 있는지 없는지 확인후 있으면 ] 출력
		parseErrorK();

        //2 혹은 3,4,5,6...
		while(wordNow.idenfr=="[") // [
		{
			param_symbol->setDim(2);
			numOfArrLayer++;
			Syntax_Analysis_Main(true, true); // [
			//parseConstExp();
			Record record1 = constExpValue();
			midvalue = record1.getRetValue();
			param_symbol->setDim2(0);
			param_symbol->setDim1(midvalue);

			// k error : ]가 있는지 없는지 확인후 있으면 ] 출력
			parseErrorK();
		}
	}
	cur_func_symbol->addParam(param_symbol);
	curBlock->addSymbol(param_symbol);
	param_symbol->setAddress(address);
	address++;

	if (numOfArrLayer == 1)
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
	// 22
	// func_symbol 선언, 주소 3으로 초기화, 시작 코드는 codeList.size();
	Func_symbol *func_symbol = new Func_symbol("", 0);
	address = 3;
	func_symbol->setStartCode(codeList.size());
	// Label codeList에 넣기
	Label *label = new Label();
	Code code("INT_L", label);
	codeList.push_back(code);

	//func_symbol.dim에 값 넣기
	int type;
	string type2;
	if (wordNow.idenfr == "void")
	{
		type = VOID;
		type2 = "void";
		func_symbol->setDim(-1);
	}
	else if(wordNow.idenfr=="int")
	{
		type = INT;
		type2 = "int";
		func_symbol->setDim(0);
	}

	Syntax_Analysis_Main(true, true); // void, int 구분할 필요가 없는게 return에서 차피 구분하게 만들어놓음
	
	outputSyntax_Analysis("FuncType");

	//error handeler
	if (!parseErrorB(wordNow.idenfr, wordNow.lineId)) // error b 함수명 중복 확인
	{
		symbolTable[++symbolTableTop] = Symbol22{ wordNow.idenfr, FUNC, type, levelNow };
	}
	// func_symbol.name에 값 넣기
	func_symbol->setName(wordNow.idenfr);
	Syntax_Analysis_Main(true, true); // Ident : 함수이름

	//
	curBlock->addSymbol(func_symbol);
	Block *block1 = new Block(type2, curBlock, curBlock->getLevel() + 1);
	curBlock->addCBlock(block1);
	curBlock = block1;
	cur_func_symbol = func_symbol;

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

	if(!isIntFunc){
		/* if (codeList[codeList.size() - 1].getName() != "RET" && wordList[wordCnt - 5].idenfr == ";" && wordList[wordCnt - 4].idenfr == "return")
		{
			Code code1("RET");
			codeList.push_back(code1);
		} */
		if ((wordList[wordCnt - 5].idenfr != ";" && wordList[wordCnt - 4].idenfr == "return") || codeList[codeList.size() - 1].getName() != "RET")
		{
			Code code1("RET");
			codeList.push_back(code1);
		}
	}
	label->setPoint(address);
	curBlock = curBlock->getFBlock();

	outputSyntax_Analysis("FuncDef");
}

//메인 함수일떄 사용
void parseMainFunc()
{
	//22
	Func_symbol *func_symbol = new Func_symbol("", 0);
	string type;
	string name;
	address = 0;
	func_symbol->setStartCode(codeList.size());
	func_symbol->setName("main");
	Label *label = new Label();
	Code code("INT_L", label);
	codeList.push_back(code);

	curBlock->addSymbol(func_symbol);
	Block* block1 = new Block("int", curBlock, curBlock->getLevel()+1);
	curBlock->addCBlock(block1);
	curBlock = block1;
	cur_func_symbol = func_symbol;

	// 11


	Syntax_Analysis_Main(true, true); // int
	Syntax_Analysis_Main(true, true); //main
	Syntax_Analysis_Main(true, true); // (

	lastNonTerminalLine = wordNow.lineId;
	// ()가 잘 닫히는지 확인하는 오류. 함수에서 )까지 출력한다.
	parseErrorJ();
	parseBlock(true);

	// 22
	label->setPoint(address);
	curBlock = curBlock->getFBlock();

	outputSyntax_Analysis("MainFuncDef");
}


void parseCompUnit()
{
	// 전역 변수라면
	flag = 1;
	while (wordList[wordCnt-1].label == "CONSTTK" || (wordList[wordCnt-1].label == "INTTK" && wordList[wordCnt].label == "IDENFR" && wordList[wordCnt+1].label != "LPARENT"))
	{
		parseDecl();
	}
	Label *label = new Label();
	Code code1("JTM", label);
	codeList.push_back(code1);
	// 함수라면
	flag = 0;
	while ((wordList[wordCnt-1].label == "VOIDTK" && wordList[wordCnt+1].label == "LPARENT") || (wordList[wordCnt-1].label == "INTTK" && wordList[wordCnt].label == "IDENFR" && wordList[wordCnt+1].label == "LPARENT"))
	{
		if(wordList[wordCnt-1].label=="VOIDTK")
		{
			currentFunctionType = VOID;
			isIntFunc = false;
		}
		else if(wordList[wordCnt-1].label=="INTTK")
		{
			currentFunctionType = INT;
			isIntFunc = true;
		}

		parseFuncDef();
	}
	// 메인 함수라면
	if (wordList[wordCnt-1].label == "INTTK" && wordList[wordCnt].label == "MAINTK" && wordList[wordCnt+1].label == "LPARENT")
	{
		//11
		isMainFunc = true;
		currentFunctionType = INT;

		parseMainFunc();
		outputSyntax_Analysis("CompUnit");
	}
	label->setPoint(cur_func_symbol->getStartCode());
}

void Syntax_Analysis(){
	wordCnt = 0;
	wordNow = wordList[wordCnt++];
	parseCompUnit();
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

class Interpreter
{
private:
	int dstack[1000000] = {0};
	int BAddr = 0;
	int at = 0;
    int sp = -1;
	int curretInputValue = 0;

public:
    vector<string> interpret() {
        int addr;
        vector<string> res;
        while (at < codeList.size()) {
            Code curCode = codeList[at];
			
            cout << curCode.getName() <<"  : "<< at<< "\n";
            cout << "code.getLevel() = " << curCode.getLevel() << "\n";
            cout << "code.getAddr() = " << curCode.getAddr() << "\n";
            cout << "code.getPrint() = " << curCode.getPrint() << "\n";
			if(curCode.getLabel()!=nullptr)
				cout << "code.getLabel() = "<< curCode.getLabel()->getPoint()<<endl;
			if (curCode.getName() == "INT")
			{
				sp += curCode.getAddr();
				at++;
			}
			else if (curCode.getName() == "DOWN")
			{
				sp -= curCode.getAddr();
				at++;
			}
			else if (curCode.getName() == "LOD")
			{
				sp++;
				if (curCode.getLevel() == 0) {
					addr = BAddr + curCode.getAddr();
				} else {
					addr = curCode.getAddr();
				}
				dstack[sp] = dstack[addr];
				at++;
			}
			else if (curCode.getName() == "LODS")
			{
				dstack[sp] = dstack[dstack[sp]];
				at++;
			}
			else if (curCode.getName() == "LDA")
			{
				sp++;
				if (curCode.getLevel() == 0) {
					addr = BAddr + curCode.getAddr();
				} else {
					addr = curCode.getAddr();
				}
				dstack[sp] = addr;
				at++;
			}
			else if (curCode.getName() == "LDC")
			{
				sp++;
				dstack[sp] = curCode.getAddr();
				at++;
			}
			else if (curCode.getName() == "STOS")
			{
				sp--;
				dstack[dstack[sp]] = dstack[sp + 1];
				sp--;
				at++;
			}
			else if (curCode.getName() == "ADD")
			{
				sp--;
				dstack[sp] = dstack[sp] + dstack[sp + 1];
				at++;
			}
			else if (curCode.getName() == "SUB")
			{
				sp--;
				dstack[sp] = dstack[sp] - dstack[sp + 1];
				at++;
			}
			else if (curCode.getName() == "MUL")
			{
				sp--;
				dstack[sp] = dstack[sp] * dstack[sp + 1];
				at++;
			}
			else if (curCode.getName() == "DIV")
			{
				sp--;
				dstack[sp] = dstack[sp] / dstack[sp + 1];
				at++;
			}
			else if (curCode.getName() == "MOD")
			{
				sp--;
				dstack[sp] = dstack[sp] % dstack[sp + 1];
				at++;
			}
			else if (curCode.getName() == "MINU")
			{
				dstack[sp] = -dstack[sp];
				at++;
			}
			else if (curCode.getName() == "GET")
			{	
				sp++;
				cin >> dstack[sp];
				at++;
			}
			else if (curCode.getName() == "PRF")
			{
				string s = curCode.getPrint();
				// 문자열 내의 모든 따옴표(")를 제거합니다.
				s.erase(remove(s.begin(), s.end(), '\"'), s.end());
				// "%d"라는 서식 지정자의 개수를 세기 위한 변수를 초기화 합니다.
				int ci = count(s.begin(), s.end(), '%');
				// 스택 포인터를 "%d"의 개수만큼 뒤로 이동시킵니다.
				sp = sp - ci;
				// "%d"를 스택에서 가져온 숫자로 치환합니다.
				for (int i = 0; i < ci; i++) {
					size_t pos = s.find("%d");
					if (pos != string::npos) {
						// 스택에서 숫자를 가져와 문자열로 변환합니다.
						s.replace(pos, 2, to_string(dstack[sp + i + 1]));
					}
				}
				at++;
				// 문자열 내의 모든 "\\n"을 "\n"으로 교체해 줄바꿈 문자를 처리합니다.
				size_t pos = 0;
				while ((pos = s.find("\\n", pos)) != string::npos)
				{
					s.replace(pos, 2, "\n");
					pos += 1;
				}
				res.push_back(s);
			}
			else if (curCode.getName() == "JTM")
			{
				BAddr = sp + 1;
				at = curCode.getLabel()->getPoint();
			}
			else if (curCode.getName() == "CAL")
			{
				dstack[sp + 1] = 0;
				dstack[sp + 2] = BAddr;
				dstack[sp + 3] = at + 1;
				BAddr = sp + 1;
				sp = sp + 3;
				at = curCode.getAddr();
			}
			else if (curCode.getName() == "RET")
			{
				at = dstack[BAddr + 2];
				sp = BAddr;
				BAddr = dstack[BAddr + 1];
			}
			else if (curCode.getName() == "RET_TO_END")
			{
				at = codeList.size();
			}
			else if (curCode.getName() == "INT_L")
			{
				sp += curCode.getLabel()->getPoint();
				at++;
			}
			else if (curCode.getName() == "BGT") {
				sp--;
				dstack[sp] = (dstack[sp] > dstack[sp + 1]) ? 1 : 0;
				at++;
			} else if (curCode.getName() == "BGE") {
				sp--;
				dstack[sp] = (dstack[sp] >= dstack[sp + 1]) ? 1 : 0;
				at++;
			} else if (curCode.getName() == "BLT") {
				sp--;
				dstack[sp] = (dstack[sp] < dstack[sp + 1]) ? 1 : 0;
				at++;
			} else if (curCode.getName() == "BLE") {
				sp--;
				dstack[sp] = (dstack[sp] <= dstack[sp + 1]) ? 1 : 0;
				at++;
			} else if (curCode.getName() == "BEQ") {
				sp--;
				dstack[sp] = (dstack[sp] == dstack[sp + 1]) ? 1 : 0;
				at++;
			} else if (curCode.getName() == "BNE") {
				sp--;
				dstack[sp] = (dstack[sp] != dstack[sp + 1]) ? 1 : 0;
				at++;
			} else if (curCode.getName() == "BZT") {
				if (dstack[sp] == 0) {
					at = curCode.getLabel()->getPoint();
				} else {
					at++;
				}
				sp--;
			} else if (curCode.getName() == "J") {
				at = curCode.getLabel()->getPoint();
			} else if (curCode.getName() == "JP0") {
				if (dstack[sp] == 0) {
					at = curCode.getLabel()->getPoint();
				} else {
					at++;
				}
			} else if (curCode.getName() == "JP1") {
				if (dstack[sp] == 1) {
					at = curCode.getLabel()->getPoint();
				} else {
					at++;
				}
			} else if (curCode.getName() == "NOT") {
				if (dstack[sp] == 0) {
					dstack[sp] = 1;
				} else {
					dstack[sp] = 0;
				}
				at++;
			} else {
				at++;
			}


			for (int i=0; i < 10; i++) {
                cout << dstack[i] << " ";
            }
            cout << "\n";
            cout << "BAddr = " << BAddr << "\n";
            cout << "at = " << at << "\n";
            cout << "sp = " << sp << "\n";
            cout << "\n\n";
        }
        return res;
    }
	void outputPCode(vector<string> interpret)
	{

		/* 
		FILE *inputFile;
		ifstream inFile("input.txt");
		ofstream fout("output.txt");
		ofstream errorOut("error.txt");
		ofstream pcodeOut("pcoderesult.txt");
		 */
		for (int i = 0; i < interpret.size();i++)
		{
			cout << interpret[i];
			pcodeOut << interpret[i];
		}
	}
};


int main(void) {
    init(); 
    Lexical_Analysis();
    Syntax_Analysis();
    outputError();

	cout << "=====================================" << endl;
	
	Interpreter interpreter;
	vector<string> interpret = interpreter.interpret();


	interpreter.outputPCode(interpret);

	/* delete cur_func_symbol;
	delete curBlock; */
	
	fclose(inputFile);
    fout.close();
    errorOut.close();
	pcodeOut.close();

	return 0;
}
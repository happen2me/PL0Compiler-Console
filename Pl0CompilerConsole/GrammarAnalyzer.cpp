#include "pch.h"
#include "GrammarAnalyzer.h"
#include <string>
#include <iostream>
#include "Error.h"
using namespace std;


#define DEBUG 1
#define VERBOSE 0


#define DUMMY_PROC_ADDRESS -1
#define INVALID_ADDRESS -1

#define MAX_NEST_LEVEL 3



#define NOT_FOUND -1

GrammarAnalyzer::GrammarAnalyzer()
{
}

GrammarAnalyzer::GrammarAnalyzer(const std::vector<Word>& wordList):
	wordStack()
{
	for (int i = wordList.size() - 1; i >= 0; i--) {
		wordStack.push(wordList[i]);
		if (DEBUG && VERBOSE) {
			std::cout << "pushed " << wordStack.top().name << std::endl;
		}		
	}
}


GrammarAnalyzer::~GrammarAnalyzer()
{
}

void GrammarAnalyzer::emit(Instruction::InstructionType type, int l, int m)
{
	pcodes.push_back(Instruction(type, l, m));
}

bool GrammarAnalyzer::read()
{
	if (!wordStack.empty()) {
		cur = wordStack.top();
		if (VERBOSE) {
			std::cout << "Read word " << cur.name << ": " << cur.val << std::endl;
		}
		
		wordStack.pop();
		if (!wordStack.empty()) {
			nextWord = wordStack.top();
		}
		else {
			nextWord = Word::CreateEmptyWord();
		}
		return true;
	}
	else {
		cur = Word::CreateEmptyWord();
		return false;
	}
		
}

void GrammarAnalyzer::runCompile()
{
	read();
	MAIN_PROC();
}

//<主程序>::=<分程序>.
void GrammarAnalyzer::MAIN_PROC()
{
	lev = -1;
	try {
		SUB_PROC();
		
		if (!checkType(Word::SP_DOT)) {
			Error::raise(cur.line, Error::EXPECT_DOT_AT_END);
		}

	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
	}

	if (wordStack.empty()) {
		std::cout << "\nGrammar Analyze finished" << std::endl;
	}
	else {
		std::cerr << "\nThere are still " << wordStack.size() << " words remains in statck" << std::endl;
		std::cerr << "cur is in line " << cur.line+1 << std::endl;
	}
	printSymbolTable();
	printPcodes();
}

//<分程序>::=[<常量说明部分>][<变量说明部分>][<过程说明部分>]<语句>
void GrammarAnalyzer::SUB_PROC()
{
	int stored_lev = lev;
	lev += 1;

	if (lev > MAX_NEST_LEVEL) {
		Error::raise(cur.line, Error::EXCEED_MAX_LEVEL);
	}

	int dx = 3;
	int stored_tx = getTx();
	int stored_cx = getCx();

	emit(Instruction::JMP, 0, 0);

	if (cur.type == Word::KW_CONST) {
		CONST_DECLARATION();
	}
	if (cur.type == Word::KW_VAR) {
		VAR_DECLARATION(dx);
	}
	if (cur.type == Word::KW_PROCEDURE) {
		PROCEDURE_DECLARATION();
	}

	pcodes[stored_cx].m = getCx();

	if (lev > 0) {
		table[stored_tx-1].address = getCx();
	}
	

	emit(Instruction::INC, 0, dx); // space for var

	STATEMENT();

	emit(Instruction::OPR, 0, Instruction::OT_RET);

	lev = stored_lev;
}

//<语句>::=<赋值语句> | <条件语句> | <当循环语句> | <过程调用语句> | <复合语句> | <读语句> | <写语句> | <重复语句> | <空>
void GrammarAnalyzer::STATEMENT()
{
	if (cur.type == Word::IDENTIFIER) {
		ASSIGNMENT_STATEMENT();
	}
	else if (cur.isReserved()) {
		if (cur.type == Word::KW_IF) {
			CONDITIONAL_STATEMENT();
		}
		else if (cur.type == Word::KW_WHILE) {
			WHILE_STATEMENT();
		}
		else if (cur.type == Word::KW_CALL) {
			CALL_STATEMENT();
		}
		else if (cur.type == Word::KW_BEGIN) {
			COMPOUND_STATEMENT();
		}
		else if (cur.type == Word::KW_READ) {
			READ_STATEMENT();
		}
		else if (cur.type == Word::KW_WRITE) {
			WRITE_STATEMENT();
		}
		else if (cur.type == Word::KW_REPEAT) {
			REPEAT_STATEMENT();
		}
	}
}

//<表达式>::=[+|-]<项>{<加法运算符><项>}
void GrammarAnalyzer::EXPRESSION()
{
	std::set<Word::WordType> expression_first = {Word::OP_PLUS, Word::OP_MINUS, Word::SP_LEFT_PAR, Word::IDENTIFIER, Word::CONST};
	
	if (expression_first.count(cur.type) == 0) {
		Error::raise(cur.line, Error::INCORECT_SYMBOL_LEADS_EXPRESSION);
	}
	
	bool neg_start = false;
	if (cur.name == "+" || cur.name == "-") {
		neg_start = checkType(Word::OP_MINUS);
		read();
	}
	TERM();
	if (neg_start) {
		emit(Instruction::OPR, 0, Instruction::OT_NEG);
	}

	while (cur.name == "+" || cur.name == "-")
	{
		bool is_plus = checkType(Word::OP_PLUS);
		read();
		TERM();
		if (is_plus) {
			emit(Instruction::OPR, 0, Instruction::OT_ADD);
		}
		else {
			emit(Instruction::OPR, 0, Instruction::OT_SUB);
		}
	}
}

//<条件>::=<表达式><关系运算符><表达式> | odd<表达式>
void GrammarAnalyzer::CONDITION()
{
	if (cur.type == Word::KW_ODD) {
		read();
		EXPRESSION();
		emit(Instruction::OPR, 0, Instruction::OT_ODD);
	}
	else {
		EXPRESSION();
		Word::WordType opr_type = cur.type;
		if (opr_type == Word::OP_ASSIGN) { // auto modification
			opr_type = Word::OP_EQUAL;
		}
		read();
		EXPRESSION();
		switch (opr_type)
		{
		case Word::OP_EQUAL:
			emit(Instruction::OPR, 0, Instruction::OT_EQL);
			break;
		case Word::OP_NOT_EQUAL:
			emit(Instruction::OPR, 0, Instruction::OT_NEQ);
			break;
		case Word::OP_LESS:
			emit(Instruction::OPR, 0, Instruction::OT_LSS);
			break;
		case Word::OP_LESS_EQUAL:
			emit(Instruction::OPR, 0, Instruction::OT_LEQ);
			break;
		case Word::OP_ABOVE:
			emit(Instruction::OPR, 0, Instruction::OT_GTR);
			break;
		case Word::OP_ABOVE_EQUAL:
			emit(Instruction::OPR, 0, Instruction::OT_GEQ);
			break;
		default:
			if (!cur.isRetionalOperator()) {
				if (checkType(Word::OP_ASSIGN)) {
					Error::raise(cur.line, Error::USE_EQUAL_INSTEAD_OF_ASSIGN);
				}
				else {
					Error::raise(cur.line, Error::EXPECT_RATIONAL_OPERATOR);
				}
			}
			break;
		}
	}
}

//<因子>::=<标识符> | <无符号整数> | '('<表达式>')'
void GrammarAnalyzer::FACTOR()
{
	if (cur.type == Word::IDENTIFIER) {

		int pos = position(cur.name, lev);

		if (pos == NOT_FOUND) {
			Error::raise(cur.line, Error::UNDECLARED_IDENTIFIER);
		}
		else {
			if (table[pos].type == Symbol::CONST) {
				emit(Instruction::LIT, 0, table[pos].val);
			}
			else if(table[pos].type == Symbol::VAR){
				emit(Instruction::LOD, lev - table[pos].level, table[pos].address);
			}
			else {
				Error::raise(cur.line, Error::EXPRESSION_CANNOT_CONTAIN_PROC);
			}
		}

		read();
	}
	else if (cur.type == Word::CONST) {

		emit(Instruction::LIT, 0, cur.val);

		read();

	}
	else if (cur.name == "(") {
		read();
		EXPRESSION();
		test(cur.line, Word::SP_RIGHT_PAR, Error::MISSING_RIGHT_PARENTHESIS);
	}
	else {
		Error::raise(cur.line, Error::UNEXPECTED);
	}
}

//<项>::=<因子>{<乘法运算符><因子>}
void GrammarAnalyzer::TERM()
{
	FACTOR();
	while (cur.name == "*" || cur.name == "/") {
		bool is_multiply = checkType(Word::OP_MULTIPLY);

		read();
		FACTOR();
		
		if (is_multiply) {
			emit(Instruction::OPR, 0, Instruction::OT_MUL);
		}
		else {
			emit(Instruction::OPR, 0, Instruction::OT_DIV);
		}
	}
}

//<常量说明部分>::=const <常量定义>{,<常量定义>}
void GrammarAnalyzer::CONST_DECLARATION()
{

	test(cur.line, Word::KW_CONST, Error::EXPECT);
	CONST_DEFINITION();
	while (cur.name == ",")
	{
		read();
		CONST_DEFINITION();
	}

	test(cur.line, Word::SP_SEMICOLON, Error::MISSING_COMMA_OR_SEMICOLON);

}

//<常量定义>::=<标识符>=<无符号整数>
void GrammarAnalyzer::CONST_DEFINITION()
{
	confirm(Word::IDENTIFIER);
	std::string const_name = cur.name;
	read();
	
	if (checkType(Word::OP_ASSIGN)) {
		Error::raise(cur.line, Error::USE_EQUAL_INSTEAD_OF_ASSIGN);
		read();
	}
	else {
		confirm(Word::OP_EQUAL);
		read();
	}
	
	confirm(Word::CONST);

	enter(Symbol::CONST, const_name, cur.val);
	read();
}
//<变量说明部分>::=var<标识符>{,<标识符>}
void GrammarAnalyzer::VAR_DECLARATION(int& dx)
{
	confirm(Word::KW_VAR);
	read();
	//// confirm(Word::IDENTIFIER);
	//if (!checkType(Word::IDENTIFIER)) {
	//	Error::raise(cur.line, Error::SHOULD_IDENTIFIRE);
	//}
	//read();
	while (cur.type == Word::IDENTIFIER)
	{
		enter(Symbol::VAR, cur.name, lev, dx);
		dx += 1;
		read();
		if (checkType(Word::SP_COMMA)) {
			read();
		}
		else {
			break;
		}
	}
	
	
	if (dx == 3) { //means no var added
		Error::raise(cur.line, Error::SHOULD_BE_FOLLOWED_BY_IDENTIFIRE);
	}

	if (checkType(Word::SP_SEMICOLON)) {
		read();
	}
	else {
		Error::raise(cur.line, Error::MISSING_COMMA_OR_SEMICOLON);
	}
	
}

//<过程说明部分> ::= procedure<标识符>;<分程序>;{<过程说明部分>}
void GrammarAnalyzer::PROCEDURE_DECLARATION()
{
	confirm(Word::KW_PROCEDURE);
	read();
	//confirm(Word::IDENTIFIER);
	if (checkType(Word::IDENTIFIER)) {
		enter(Symbol::PROC, cur.name, lev, DUMMY_PROC_ADDRESS); //TODO: fill address
	}
	else {
		Error::raise(cur.line, Error::SHOULD_BE_FOLLOWED_BY_IDENTIFIRE);
	}
	read();
	
	if (checkType(Word::SP_SEMICOLON)) {
		read();
	}
	else {
		Error::raise(cur.line, Error::MISSING_COMMA_OR_SEMICOLON);
	}

	SUB_PROC();

	if (checkType(Word::SP_SEMICOLON)) {
		read();
	}
	else {
		Error::raise(cur.line, Error::MISSING_COMMA_OR_SEMICOLON);
	}

	while (!cur.isEmptyWord() && cur.type == Word::KW_PROCEDURE)
	{
		PROCEDURE_DECLARATION();
	}
}

// <赋值语句>::=<标识符>:=<表达式>
void GrammarAnalyzer::ASSIGNMENT_STATEMENT()
{
	
	confirm(Word::IDENTIFIER);

	int pos = position(cur.name, lev);

	if (pos == NOT_FOUND) {
		Error::raise(cur.line, Error::UNDECLARED_IDENTIFIER);
	}
	else if(table[pos].type == Symbol::PROC || table[pos].type == Symbol::CONST){
		Error::raise(cur.line, Error::ASSIGNED_TO_CONST_OR_PROC);
	}

	read();

	//if (checkType(Word::OP_EQUAL)) {
	//	Error::raise(cur.line, Error::USE_ASSIGN_INSTEAD_OF_EQUAL);
	//	read();
	//}
	//else if(checkType(Word::OP_ASSIGN)){
	//	read();
	//}
	//else {
	//	Error::raise(cur.line, Error::EXPECT_ASSIGN);
	//}
	confirm(Word::OP_ASSIGN);
	read();
	if (DEBUG && VERBOSE) {
		cout << "!!!! cur.type is " << Word::translator[cur.type] << " name is " << cur.name << endl;
	}	

	EXPRESSION();
	if (pos != NOT_FOUND) {
		emit(Instruction::STO, lev - table[pos].level, table[pos].address);
	}
}

// <复合语句> :: = begin<语句>{ ; <语句> }end
void GrammarAnalyzer::COMPOUND_STATEMENT()
{
	confirm(Word::KW_BEGIN);
	read();
	STATEMENT();
	while (cur.name == ";")
	{
		read();
		STATEMENT();
	}

	//confirm(Word::KW_END);
	test(cur.line, Word::KW_END, Error::EXPECT_SEMICOLON_OR_END);

	//read();
}

// <条件语句> ::= if<条件>then<语句>[else<语句>]
void GrammarAnalyzer::CONDITIONAL_STATEMENT()
{
	confirm(Word::KW_IF);
	read();
	CONDITION();

	if (confirm(Word::KW_THEN)) {
		read();
	}
	else {
		Error::raise(cur.line, Error::EXPECT_THEN);
	}

	int jpc_cx = getCx();

	emit(Instruction::JPC, 0, 0);

	STATEMENT();
	if (!cur.isEmptyWord() && cur.type == Word::KW_ELSE) {
		read();

		pcodes[jpc_cx].m = getCx() + 1; //jumps past if

		jpc_cx = getCx(); // change to jmp_cx

		emit(Instruction::JMP, 0, 0);

		STATEMENT();
	}
	pcodes[jpc_cx].m = getCx();
}
//<当循环语句>::=while<条件>do<语句>
void GrammarAnalyzer::WHILE_STATEMENT()
{
	confirm(Word::KW_WHILE);
	read();

	int condition_cx = getCx();

	CONDITION();

	int jpc_cx = getCx();

	emit(Instruction::JPC, 0, 0);

	//confirm(Word::KW_DO);
	test(cur.line, Word::KW_DO, Error::EXPECT_DO);

	if (DEBUG && VERBOSE) {
		std::cout << "before do  cur is " << cur.name << std::endl;
	}
	
	STATEMENT();

	emit(Instruction::JMP, 0, condition_cx);
	pcodes[jpc_cx].m = getCx(); // points to code after statement
}

// <过程调用语句>::=call<标识符>
void GrammarAnalyzer::CALL_STATEMENT()
{
	confirm(Word::KW_CALL);
	read();

	if (!checkType(Word::IDENTIFIER)) {
		Error::raise(cur.line, Error::EXPECT_IDENTIFIER_AFTER_CALL);
	}
	else {
		int pos = position(cur.name, lev);

		if (pos == NOT_FOUND) {
			Error::raise(cur.line, Error::UNDECLARED_IDENTIFIER);
		}
		else if (table[pos].type == Symbol::PROC) {
			emit(Instruction::CALL, lev - table[pos].level, table[pos].address);
		}
		else {
			Error::raise(cur.line, Error::CANNOT_CALL_VAR_OR_CONST);
		}

		read();
	}
	
}

//<读语句>::=read'('<标识符>{,<标识符>}')'
void GrammarAnalyzer::READ_STATEMENT()
{
	confirm(Word::KW_READ);
	read();
	confirmName("(");
	read();
	//confirm(Word::IDENTIFIER);
	//read();
	//while (cur.name == ",") {
	//	read();
	//	confirm(Word::IDENTIFIER);
	//	read();
	//}

	while (checkType(Word::IDENTIFIER))
	{
		emit(Instruction::RED, 0, 0); //读取元素置于栈顶

		int pos = position(cur.name, lev);

		if (pos == NOT_FOUND) {
			Error::raise(cur.line, Error::UNDECLARED_IDENTIFIER);			
		}
		else if(table[pos].type == Symbol::CONST || table[pos].type == Symbol::PROC){
			Error::raise(cur.line, Error::ASSIGNED_TO_CONST_OR_PROC);
		}
		else {
			emit(Instruction::WRT, lev - table[pos].level, table[pos].address); //把栈顶元素写入指定位置
		}

		read();
		if (checkType(Word::SP_COMMA)) {
			read();
		}
		else {
			break;
		}
	}

	confirmName(")");
	read();
}

// <写语句>::=write '('<表达式>{,<表达式>}')'
void GrammarAnalyzer::WRITE_STATEMENT()
{
	confirm(Word::KW_WRITE);
	read();
	confirmName("(");
	read();
	EXPRESSION();
	emit(Instruction::WRT, 0, 1);
	while (cur.name == ",") {
		read();
		EXPRESSION();
		emit(Instruction::WRT, 0, 1);
	}
	confirmName(")");
	read();
}

//<重复语句> :: = repeat<语句>{ ; <语句> }until<条件>
void GrammarAnalyzer::REPEAT_STATEMENT()
{
	//TODO: gen pcode
	confirm(Word::KW_REPEAT);
	read();

	int statemetn_cx = getCx();

	STATEMENT();
	while (cur.name == ";") {
		read();
		STATEMENT();		
	}
	confirm(Word::KW_UNTIL);
	read();
	CONDITION();

	emit(Instruction::JPC, 0, statemetn_cx);

}

bool GrammarAnalyzer::confirm(Word::WordType expectedType)
{
	if (!cur.isEmptyWord()) {
		if (cur.type == expectedType) {
			return true;
		}
		else {
			Error::raiseMissingError(cur.line, Word::translator[expectedType]);
		}
	}
	return false;
}

bool GrammarAnalyzer::confirmName(std::string expectedVal)
{
	if (!cur.isEmptyWord() && cur.name == expectedVal) {
		return true;
	}
	Error::raiseMissingError(cur.line, expectedVal);
	return false;
}

bool GrammarAnalyzer::checkType(Word::WordType expectedType)
{
	if (cur.isEmptyWord()) {
		return false;
	}
	return cur.type == expectedType;
}

void GrammarAnalyzer::enter(Symbol::SymbolType type, std::string name, int value)
{
	table.push_back(Symbol(type, name, value));
}

void GrammarAnalyzer::enter(Symbol::SymbolType type, std::string name, int level, int address)
{
	table.push_back(Symbol(type, name, level, address));
}

bool GrammarAnalyzer::checkDup(std::string name, int level)
{
	for (int i = table.size()-1; i >= 0; i--) {
		if (table[i].name == name && table[i].level == level) {
			return true;
		}
	}
	return false;
}

int GrammarAnalyzer::find(std::string name)
{
	for (int i = table.size() - 1; i >= 0; i--) {
		if (table[i].name == name) {
			return i;
		}
	}
	return -1;
}

void GrammarAnalyzer::printSymbolTable()
{
	std::cout << "\nType\tName\tValue\tLevel\tAddress" << std::endl;
	for (int i = 0; i < (int)table.size(); i++) {
		std::cout.width(8);
		std::cout << std::left << Symbol::translator[table[i].type];
		std::cout.width(8);
		std::cout << std::left << table[i].name;
		std::cout.width(8);
		if (table[i].type == Symbol::CONST) {
			std::cout << table[i].val;
			std::cout.width(8);
			std::cout << std::left << "-";
			std::cout.width(8);
			std::cout << std::left << "-" << std::endl;
		}
		else {			
			std::cout << std::left << "-";
			std::cout.width(8);
			std::cout << std::left << table[i].level;
			std::cout.width(8);
			std::cout << std::left << table[i].address << std::endl;
		}
	}
}

void GrammarAnalyzer::printPcodes()
{
	std::cout << endl;
	for (int i = 0; i < (int)pcodes.size(); i++) {
		std::cout << i << "\t" << Instruction::translator[pcodes[i].op] << "\t" << pcodes[i].l << "\t" << pcodes[i].m << std::endl;
	}
}

int GrammarAnalyzer::position(std::string identifier, int level)
{
	for (int i = table.size() - 1; i >= 0; i--) {
		if (table[i].name == identifier && table[i].level <= level) {
			return i;
		}
	}
	return -1;
}

int GrammarAnalyzer::getTx()
{
	return table.size();
}

int GrammarAnalyzer::getCx()
{
	return pcodes.size();
}

bool GrammarAnalyzer::test(int line, Word::WordType word_type, Error::ErrorType error_type)
{
	if (checkType(word_type)) {
		read();
		return true;
	}
	else {
		Error::raise(line, error_type);
		std::cout << "\t " << Word::translator[word_type] << " expected" << std::endl;
		return false;
	}
}



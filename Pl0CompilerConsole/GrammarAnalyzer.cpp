#include "pch.h"
#include "GrammarAnalyzer.h"
#include <string>
#include <iostream>
#include <fstream>
#include "Error.h"
using namespace std;


#define DEBUG 0
#define VERBOSE 0


#define DUMMY_PROC_ADDRESS -1
#define INVALID_ADDRESS -1

#define MAX_NEST_LEVEL 3

#define NOT_FOUND -1

GrammarAnalyzer::GrammarAnalyzer()
{
}

GrammarAnalyzer::GrammarAnalyzer(const std::vector<Word>& wordList) :
	word_stack(),
	error_count(0)
{
	for (int i = wordList.size() - 1; i >= 0; i--) {
		word_stack.push(wordList[i]);
		if (DEBUG && VERBOSE) {
			std::cout << "pushed " << word_stack.top().name << std::endl;
		}
	}
}


GrammarAnalyzer::~GrammarAnalyzer()
{
}

void GrammarAnalyzer::gen(Instruction::InstructionType type, int l, int m)
{
	pcodes.push_back(Instruction(type, l, m));
}

bool GrammarAnalyzer::read()
{
	if (!word_stack.empty()) {
		current_word = word_stack.top();
		if (VERBOSE) {
			std::cout << "Read word " << current_word.name << ": " << current_word.val << std::endl;
		}

		word_stack.pop();
		if (!word_stack.empty()) {
			next_word = word_stack.top();
		}
		else {
			next_word = Word::CreateEmptyWord();
		}
		return true;
	}
	else {
		current_word = Word::CreateEmptyWord();
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
		BLOCK();

		if (!checkType(Word::SP_DOT)) {
			raiseWrapper(current_word.line, Error::EXPECT_DOT_AT_END);
		}

	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
	}

	if (word_stack.empty()) {
		std::cout << "\nGrammar Analyze finished" << std::endl;
	}
	else {
		std::cerr << "\nGrammar analysis failed. There are still " << word_stack.size() << " words remains in statck" << std::endl;
		std::cerr << "cur is in line " << current_word.line + 1 << std::endl;
	}
}

//<分程序>::=[<常量说明部分>][<变量说明部分>][<过程说明部分>]<语句>
void GrammarAnalyzer::BLOCK()
{
	int stored_lev = lev;
	lev += 1;
	if (DEBUG) {
		cout << "LEVEL changed to " << lev << " at line " << current_word.line << endl;
	}

	if (lev > MAX_NEST_LEVEL) {
		raiseWrapper(current_word.line, Error::EXCEED_MAX_LEVEL);
	}

	int dx = 3;
	int stored_tx = getTx();
	int stored_cx = getCx();

	gen(Instruction::JMP, 0, 0);

	if (current_word.type == Word::KW_CONST) {
		CONST_DECLARATION();
	}
	if (current_word.type == Word::KW_VAR) {
		VAR_DECLARATION(dx);
	}
	if (current_word.type == Word::KW_PROCEDURE) {
		PROCEDURE_DECLARATION();
	}

	pcodes[stored_cx].m = getCx();

	if (lev > 0) {
		table[stored_tx - 1].address = getCx();
	}


	gen(Instruction::INC, 0, dx); // space for var

	STATEMENT();

	gen(Instruction::OPR, 0, Instruction::OT_RET);

	lev = stored_lev;
	if (DEBUG) {
		cout << "LEVEL changed to " << lev << " at line " << current_word.line <<  endl;
	}
}

//<语句>::=<赋值语句> | <条件语句> | <当循环语句> | <过程调用语句> | <复合语句> | <读语句> | <写语句> | <重复语句> | <空>
void GrammarAnalyzer::STATEMENT()
{
	if (current_word.type == Word::IDENTIFIER) {
		ASSIGNMENT_STATEMENT();
	}
	else if (current_word.isReserved()) {
		if (current_word.type == Word::KW_IF) {
			CONDITIONAL_STATEMENT();
		}
		else if (current_word.type == Word::KW_WHILE) {
			WHILE_STATEMENT();
		}
		else if (current_word.type == Word::KW_CALL) {
			CALL_STATEMENT();
		}
		else if (current_word.type == Word::KW_BEGIN) {
			COMPOUND_STATEMENT();
		}
		else if (current_word.type == Word::KW_READ) {
			READ_STATEMENT();
		}
		else if (current_word.type == Word::KW_WRITE) {
			WRITE_STATEMENT();
		}
		else if (current_word.type == Word::KW_REPEAT) {
			REPEAT_STATEMENT();
		}
	}
}

//<表达式>::=[+|-]<项>{<加法运算符><项>}
void GrammarAnalyzer::EXPRESSION()
{
	std::set<Word::WordType> expression_first = { Word::OP_PLUS, Word::OP_MINUS, Word::SP_LEFT_PAR, Word::IDENTIFIER, Word::CONST };

	if (expression_first.count(current_word.type) == 0) {
		raiseWrapper(current_word.line, Error::INCORECT_SYMBOL_LEADS_EXPRESSION);
	}

	bool neg_start = false;
	if (current_word.name == "+" || current_word.name == "-") {
		neg_start = checkType(Word::OP_MINUS);
		read();
	}
	TERM();
	if (neg_start) {
		gen(Instruction::OPR, 0, Instruction::OT_NEG);
	}

	while (current_word.name == "+" || current_word.name == "-")
	{
		bool is_plus = checkType(Word::OP_PLUS);
		read();
		TERM();
		if (is_plus) {
			gen(Instruction::OPR, 0, Instruction::OT_ADD);
		}
		else {
			gen(Instruction::OPR, 0, Instruction::OT_SUB);
		}
	}
}

//<条件>::=<表达式><关系运算符><表达式> | odd<表达式>
void GrammarAnalyzer::CONDITION()
{
	if (current_word.type == Word::KW_ODD) {
		read();
		EXPRESSION();
		gen(Instruction::OPR, 0, Instruction::OT_ODD);
	}
	else {
		EXPRESSION();
		Word::WordType opr_type = current_word.type;
		if (opr_type == Word::OP_ASSIGN) { // auto modification
			opr_type = Word::OP_EQUAL;
		}
		read();
		EXPRESSION();
		switch (opr_type)
		{
		case Word::OP_EQUAL:
			gen(Instruction::OPR, 0, Instruction::OT_EQL);
			break;
		case Word::OP_NOT_EQUAL:
			gen(Instruction::OPR, 0, Instruction::OT_NEQ);
			break;
		case Word::OP_LESS:
			gen(Instruction::OPR, 0, Instruction::OT_LSS);
			break;
		case Word::OP_LESS_EQUAL:
			gen(Instruction::OPR, 0, Instruction::OT_LEQ);
			break;
		case Word::OP_ABOVE:
			gen(Instruction::OPR, 0, Instruction::OT_GTR);
			break;
		case Word::OP_ABOVE_EQUAL:
			gen(Instruction::OPR, 0, Instruction::OT_GEQ);
			break;
		default:
			if (!current_word.isRetionalOperator()) {
				if (checkType(Word::OP_ASSIGN)) {
					raiseWrapper(current_word.line, Error::USE_EQUAL_INSTEAD_OF_ASSIGN);
				}
				else {
					raiseWrapper(current_word.line, Error::EXPECT_RATIONAL_OPERATOR);
				}
			}
			break;
		}
	}
}

//<因子>::=<标识符> | <无符号整数> | '('<表达式>')'
void GrammarAnalyzer::FACTOR()
{
	if (current_word.type == Word::IDENTIFIER) {

		int pos = position(current_word.name, lev);

		if (pos == NOT_FOUND) {
			raiseWrapper(current_word.line, Error::UNDECLARED_IDENTIFIER);
			if (DEBUG) {
				std::cout << "\t " << current_word.name << " not found\n";
			}
		}
		else {
			if (table[pos].type == Symbol::CONST) {
				gen(Instruction::LIT, 0, table[pos].val);
			}
			else if (table[pos].type == Symbol::VAR) {
				gen(Instruction::LOD, lev - table[pos].level, table[pos].address);
			}
			else {
				raiseWrapper(current_word.line, Error::EXPRESSION_CANNOT_CONTAIN_PROC);
			}
		}

		read();
	}
	else if (current_word.type == Word::CONST) {

		gen(Instruction::LIT, 0, current_word.val);

		read();

	}
	else if (current_word.name == "(") {
		read();
		EXPRESSION();
		test(current_word.line, Word::SP_RIGHT_PAR, Error::MISSING_RIGHT_PARENTHESIS);
	}
	else {
		raiseWrapper(current_word.line, Error::UNEXPECTED);
	}
}

//<项>::=<因子>{<乘法运算符><因子>}
void GrammarAnalyzer::TERM()
{
	FACTOR();
	while (current_word.name == "*" || current_word.name == "/") {
		bool is_multiply = checkType(Word::OP_MULTIPLY);

		read();
		FACTOR();

		if (is_multiply) {
			gen(Instruction::OPR, 0, Instruction::OT_MUL);
		}
		else {
			gen(Instruction::OPR, 0, Instruction::OT_DIV);
		}
	}
}

//<常量说明部分>::=const <常量定义>{,<常量定义>}
void GrammarAnalyzer::CONST_DECLARATION()
{

	test(current_word.line, Word::KW_CONST, Error::EXPECT);
	CONST_DEFINITION();
	while (current_word.name == ",")
	{
		read();
		CONST_DEFINITION();
	}

	test(current_word.line, Word::SP_SEMICOLON, Error::MISSING_COMMA_OR_SEMICOLON);

}

//<常量定义>::=<标识符>=<无符号整数>
void GrammarAnalyzer::CONST_DEFINITION()
{
	confirm(Word::IDENTIFIER);
	std::string const_name = current_word.name;
	read();

	if (checkType(Word::OP_ASSIGN)) {
		raiseWrapper(current_word.line, Error::USE_EQUAL_INSTEAD_OF_ASSIGN);
		read();
	}
	else {
		confirm(Word::OP_EQUAL);
		read();
	}

	confirm(Word::CONST);

	enter(Symbol::CONST, const_name, current_word.val);
	read();
}
//<变量说明部分>::=var<标识符>{,<标识符>}
void GrammarAnalyzer::VAR_DECLARATION(int& dx)
{
	confirm(Word::KW_VAR);
	read();
	//// confirm(Word::IDENTIFIER);
	//if (!checkType(Word::IDENTIFIER)) {
	//	raiseWrapper(current_word.line, Error::SHOULD_IDENTIFIRE);
	//}
	//read();
	while (current_word.type == Word::IDENTIFIER)
	{
		enter(Symbol::VAR, current_word.name, lev, dx);
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
		raiseWrapper(current_word.line, Error::SHOULD_BE_FOLLOWED_BY_IDENTIFIRE);
	}

	if (checkType(Word::SP_SEMICOLON)) {
		read();
	}
	else {
		raiseWrapper(current_word.line, Error::MISSING_COMMA_OR_SEMICOLON);
	}

}

//<过程说明部分> ::= procedure<标识符>;<分程序>;{<过程说明部分>}
void GrammarAnalyzer::PROCEDURE_DECLARATION()
{
	confirm(Word::KW_PROCEDURE);
	read();
	//confirm(Word::IDENTIFIER);
	if (checkType(Word::IDENTIFIER)) {
		enter(Symbol::PROC, current_word.name, lev, DUMMY_PROC_ADDRESS); //TODO: fill address
	}
	else {
		raiseWrapper(current_word.line, Error::SHOULD_BE_FOLLOWED_BY_IDENTIFIRE);
	}
	read();

	if (checkType(Word::SP_SEMICOLON)) {
		read();
	}
	else {
		raiseWrapper(current_word.line, Error::MISSING_COMMA_OR_SEMICOLON);
	}

	BLOCK();

	if (checkType(Word::SP_SEMICOLON)) {
		read();
	}
	else {
		raiseWrapper(current_word.line, Error::MISSING_COMMA_OR_SEMICOLON);
	}

	while (!current_word.isEmptyWord() && current_word.type == Word::KW_PROCEDURE)
	{
		PROCEDURE_DECLARATION();
	}
}

// <赋值语句>::=<标识符>:=<表达式>
void GrammarAnalyzer::ASSIGNMENT_STATEMENT()
{

	confirm(Word::IDENTIFIER);

	int pos = position(current_word.name, lev);

	if (pos == NOT_FOUND) {
		raiseWrapper(current_word.line, Error::UNDECLARED_IDENTIFIER);
	}
	else if (table[pos].type == Symbol::PROC || table[pos].type == Symbol::CONST) {
		raiseWrapper(current_word.line, Error::ASSIGNED_TO_CONST_OR_PROC);
	}

	read();

	//if (checkType(Word::OP_EQUAL)) {
	//	raiseWrapper(current_word.line, Error::USE_ASSIGN_INSTEAD_OF_EQUAL);
	//	read();
	//}
	//else if(checkType(Word::OP_ASSIGN)){
	//	read();
	//}
	//else {
	//	raiseWrapper(current_word.line, Error::EXPECT_ASSIGN);
	//}
	confirm(Word::OP_ASSIGN);
	read();
	if (DEBUG && VERBOSE) {
		cout << "!!!! cur.type is " << Word::translator[current_word.type] << " name is " << current_word.name << endl;
	}

	EXPRESSION();
	if (pos != NOT_FOUND && table[pos].type == Symbol::VAR) {
		gen(Instruction::STO, lev - table[pos].level, table[pos].address);
	}
}

// <复合语句> :: = begin<语句>{ ; <语句> }end
void GrammarAnalyzer::COMPOUND_STATEMENT()
{
	confirm(Word::KW_BEGIN);
	read();
	STATEMENT();
	while (current_word.name == ";")
	{
		read();
		STATEMENT();
	}

	//confirm(Word::KW_END);
	test(current_word.line, Word::KW_END, Error::EXPECT_SEMICOLON_OR_END);

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
		raiseWrapper(current_word.line, Error::EXPECT_THEN);
	}

	int jpc_cx = getCx();

	gen(Instruction::JPC, 0, 0);

	STATEMENT();
	if (!current_word.isEmptyWord() && current_word.type == Word::KW_ELSE) {
		read();

		pcodes[jpc_cx].m = getCx() + 1; //jumps past if

		jpc_cx = getCx(); // change to jmp_cx

		gen(Instruction::JMP, 0, 0);

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

	gen(Instruction::JPC, 0, 0);

	//confirm(Word::KW_DO);
	test(current_word.line, Word::KW_DO, Error::EXPECT_DO);

	if (DEBUG && VERBOSE) {
		std::cout << "before do  cur is " << current_word.name << std::endl;
	}

	STATEMENT();

	gen(Instruction::JMP, 0, condition_cx);
	pcodes[jpc_cx].m = getCx(); // points to code after statement
}

// <过程调用语句>::=call<标识符>
void GrammarAnalyzer::CALL_STATEMENT()
{
	confirm(Word::KW_CALL);
	read();

	if (!checkType(Word::IDENTIFIER)) {
		raiseWrapper(current_word.line, Error::EXPECT_IDENTIFIER_AFTER_CALL);
	}
	else {
		int pos = position(current_word.name, lev);

		if (pos == NOT_FOUND) {
			raiseWrapper(current_word.line, Error::UNDECLARED_IDENTIFIER);
		}
		else if (table[pos].type == Symbol::PROC) {
			gen(Instruction::CALL, lev - table[pos].level, table[pos].address);
		}
		else {
			raiseWrapper(current_word.line, Error::CANNOT_CALL_VAR_OR_CONST);
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
	//while (current_word.name == ",") {
	//	read();
	//	confirm(Word::IDENTIFIER);
	//	read();
	//}

	while (checkType(Word::IDENTIFIER))
	{
		gen(Instruction::RED, 0, 0); //读取元素置于栈顶

		int pos = position(current_word.name, lev);

		if (pos == NOT_FOUND) {
			raiseWrapper(current_word.line, Error::UNDECLARED_IDENTIFIER);
		}
		else if (table[pos].type == Symbol::CONST || table[pos].type == Symbol::PROC) {
			raiseWrapper(current_word.line, Error::ASSIGNED_TO_CONST_OR_PROC);
		}
		else {
			gen(Instruction::STO, lev - table[pos].level, table[pos].address); //把栈顶元素写入指定位置
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
	gen(Instruction::WRT, 0, 1);
	while (current_word.name == ",") {
		read();
		EXPRESSION();
		gen(Instruction::WRT, 0, 1);
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
	while (current_word.name == ";") {
		read();
		STATEMENT();
	}
	confirm(Word::KW_UNTIL);
	read();
	CONDITION();

	gen(Instruction::JPC, 0, statemetn_cx);

}

bool GrammarAnalyzer::confirm(Word::WordType expectedType)
{
	if (!current_word.isEmptyWord()) {
		if (current_word.type == expectedType) {
			return true;
		}
		else {
			//Error::raiseMissingError(current_word.line, Word::translator[expectedType]);
			raiseWrapper(current_word.line, Error::EXPECT, Word::translator[expectedType]);
		}
	}
	return false;
}

bool GrammarAnalyzer::confirmName(std::string expectedVal)
{
	if (!current_word.isEmptyWord() && current_word.name == expectedVal) {
		return true;
	}
	//Error::raiseMissingError(current_word.line, expectedVal);
	raiseWrapper(current_word.line, Error::EXPECT, expectedVal);
	return false;
}

bool GrammarAnalyzer::checkType(Word::WordType expectedType)
{
	if (current_word.isEmptyWord()) {
		return false;
	}
	return current_word.type == expectedType;
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
	for (int i = table.size() - 1; i >= 0; i--) {
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

void GrammarAnalyzer::printSymbolTable(std::ostream& out)
{
	out << "\nType\tName\tValue\tLevel\tAddress" << std::endl;
	for (int i = 0; i < (int)table.size(); i++) {
		out.width(8);
		out << std::left << Symbol::translator[table[i].type];
		out.width(8);
		out << std::left << table[i].name;
		out.width(8);
		if (table[i].type == Symbol::CONST) {
			out << table[i].val;
			out.width(8);
			out << std::left << "-";
			out.width(8);
			out << std::left << "-" << std::endl;
		}
		else {
			out << std::left << "-";
			out.width(8);
			out << std::left << table[i].level;
			out.width(8);
			out << std::left << table[i].address << std::endl;
		}
	}
}

void GrammarAnalyzer::printPcodes(std::ostream& out)
{
	out << endl;
	for (int i = 0; i < (int)pcodes.size(); i++) {
		out << i << "\t" << Instruction::translator[pcodes[i].op] << "\t" << pcodes[i].l << "\t";
		if (pcodes[i].op == Instruction::OPR) {
			out << Instruction::op_translator[(Instruction::OperationType)pcodes[i].m] << std::endl;
		}
		else {
			out << pcodes[i].m << std::endl;
		}
	}
}

int GrammarAnalyzer::getErrorCount()
{
	return error_count;
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

int GrammarAnalyzer::position_debug(std::string identifier, int level)
{
	std::cout << "ID:  " << identifier << "; LEVEL: " << level << std::endl;
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
		raiseWrapper(line, error_type);
		//std::cout << "\t " << Word::translator[word_type] << " expected" << std::endl;
		return false;
	}
}

void GrammarAnalyzer::raiseWrapper(int line, Error::ErrorType errorType)
{
	error_count++;
	Error::raise(line, errorType);
	if (errorType == Error::UNDECLARED_IDENTIFIER) {
		std::cout << "undeclared identifier " << current_word.name << std::endl;
	}
}

void GrammarAnalyzer::raiseWrapper(int line, Error::ErrorType errorType, std::string expectation)
{
	error_count++;
	Error::raise(line, errorType);
	std::cerr << current_word.name << " read, expect " << expectation << std::endl;
}

bool GrammarAnalyzer::errorHappened()
{
	return error_count != 0;
}

std::vector<Instruction> GrammarAnalyzer::getResults()
{
	return pcodes;
}



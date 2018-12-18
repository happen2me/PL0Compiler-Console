#include "pch.h"
#include "GrammarAnalyzer.h"
#include <string>
#include <iostream>
#include "Error.h"
using namespace std;

#define DUMMY_PROC_ADDRESS -1

//"begin", "end", "if", "then", "else", "const", "procedure", "var", "do", "while", "call", "read", "write", "odd", "repeat", "until"

//std::string KW_BEGIN = "begin";
//std::string KW_END = "end";
//std::string KW_IF = "if";
//std::string KW_THEN = "then";
//std::string KW_ELSE = "else";
//std::string KW_CONST = "const";
//std::string KW_PROCEDURE = "procedure";
//std::string KW_VAR = "var";
//std::string KW_DO = "do";
//std::string KW_WHILE = "while";
//std::string KW_CALL = "call";
//std::string KW_READ = "read";
//std::string KW_WRITE = "write";
//std::string KW_ODD = "odd";
//std::string KW_REPEAT = "repeat";
//std::string KW_UNTIL = "until";

GrammarAnalyzer::GrammarAnalyzer()
{
}

GrammarAnalyzer::GrammarAnalyzer(const std::vector<Word>& wordList):
	wordStack()
{
	for (int i = wordList.size() - 1; i >= 0; i--) {
		wordStack.push(wordList[i]);
		std::cout << "pushed " << wordStack.top().name << std::endl;
	}
}


GrammarAnalyzer::~GrammarAnalyzer()
{
}

bool GrammarAnalyzer::read()
{
	if (!wordStack.empty()) {
		cur = wordStack.top();
		std::cout << "Read word " << cur.name << ": " << cur.val << std::endl;
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

//<������>::=<�ֳ���>.
void GrammarAnalyzer::MAIN_PROC()
{
	lev = -1;
	try {
		SUB_PROC();
		//confirmName(".");
		confirm(Word::SP_DOT);
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
	}

	if (wordStack.empty()) {
		std::cout << "Grammar Analyze Succeed" << std::endl;
	}
	else {
		std::cerr << "There are still " << wordStack.size() << " words remains in statck" << std::endl;
		std::cerr << "cur is in line " << cur.line << std::endl;
	}
	printSymbolTable();
}

//<�ֳ���>::=[<����˵������>][<����˵������>][<����˵������>]<���>
void GrammarAnalyzer::SUB_PROC()
{
	int stored_lev = lev;
	lev += 1;
	if (cur.type == Word::KW_CONST) {
		CONST_DECLARATION();
	}
	if (cur.type == Word::KW_VAR) {
		VAR_DECLARATION();
	}
	if (cur.type == Word::KW_PROCEDURE) {
		PROCEDURE_DECLARATION();
	}
	STATEMENT();
	lev = stored_lev;
}

//<���>::=<��ֵ���> | <�������> | <��ѭ�����> | <���̵������> | <�������> | <�����> | <д���> | <�ظ����> | <��>
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

//<���ʽ>::=[+|-]<��>{<�ӷ������><��>}
void GrammarAnalyzer::EXPRESSION()
{
	if (cur.name == "+" || cur.name == "-") {
		read();

	}
	TERM();
	while (cur.name == "+" || cur.name == "-")
	{
		read();
		TERM();
	}
}

//<����>::=<���ʽ><��ϵ�����><���ʽ> | odd<���ʽ>
void GrammarAnalyzer::CONDITION()
{
	if (cur.type == Word::KW_ODD) {
		read();
		EXPRESSION();
	}
	else {
		EXPRESSION();
		//TODO shori
		//confirmType(Word::BINARY_OPERATOR);
		if (!cur.isOperator()) {
			Error::raiseUnexpectedError(cur.line, cur.name);
		}
		read();
		EXPRESSION();
	}
}

//<����>::=<��ʶ��> | <�޷�������> | '('<���ʽ>')'
void GrammarAnalyzer::FACTOR()
{
	if (cur.type == Word::IDENTIFIER) {
		read();

	}
	else if (cur.type == Word::CONST) {
		read();

	}
	else if (cur.name == "(") {
		read();
		EXPRESSION();
		//confirmName(")");
		confirm(Word::SP_RIGHT_PAR);
		read();
	}
	else {
		//TODO: 
		Error::raiseUnexpectedError(cur.line, cur.name);
	}
}

//<��>::=<����>{<�˷������><����>}
void GrammarAnalyzer::TERM()
{
	FACTOR();
	while (cur.name == "*" || cur.name == "/") {
		read();
		FACTOR();
	}
}

//<����˵������>::=const <��������>{,<��������>}
void GrammarAnalyzer::CONST_DECLARATION()
{
	if (cur.type == Word::KW_CONST) {
		read();
		CONST_DEFINITION();
		while (cur.name == ",")
		{
			read();
			CONST_DEFINITION();
		}

		//confirm(Word::SP_SEMICOLON);
		confirm(Word::SP_SEMICOLON);
		read();
	}
	else {
		//TODO: error: expect const
		Error::raiseMissingError(cur.line, Word::translator[Word::KW_CONST]);
	}
}

//<��������>::=<��ʶ��>=<�޷�������>
void GrammarAnalyzer::CONST_DEFINITION()
{
	confirm(Word::IDENTIFIER);
	read();
	//confirmName("=");
	if (checkType(Word::OP_ASSIGN)) {
		Error::raiseError(cur.line, Error::SHOULD_USE_EQUAL);
	}
	else {
		confirm(Word::OP_EQUAL);
	}
	read();
	confirm(Word::CONST);
	enter(Symbol::CONST, cur.name, cur.val);
	read();
}
//<����˵������>::=var<��ʶ��>{,<��ʶ��>}
void GrammarAnalyzer::VAR_DECLARATION()
{
	int dx = 3;
	confirm(Word::KW_VAR);
	read();
	//// confirm(Word::IDENTIFIER);
	//if (!checkType(Word::IDENTIFIER)) {
	//	Error::raiseError(cur.line, Error::SHOULD_IDENTIFIRE);
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
		Error::raiseError(cur.line, Error::SHOULD_IDENTIFIRE);
	}
	confirm(Word::SP_SEMICOLON);
	read();
}

//<����˵������> ::= procedure<��ʶ��>;<�ֳ���>;{<����˵������>}
void GrammarAnalyzer::PROCEDURE_DECLARATION()
{
	confirm(Word::KW_PROCEDURE);
	read();
	//confirm(Word::IDENTIFIER);
	if (checkType(Word::IDENTIFIER)) {
		enter(Symbol::PROC, cur.name, lev, DUMMY_PROC_ADDRESS); //TODO: fill address
	}
	else {
		Error::raiseError(cur.line, Error::SHOULD_IDENTIFIRE);
	}
	read();
	confirm(Word::SP_SEMICOLON);
	read();
	SUB_PROC();
	confirm(Word::SP_SEMICOLON);
	read();
	while (!cur.isEmptyWord() && cur.type == Word::KW_PROCEDURE)
	{
		PROCEDURE_DECLARATION();
	}
}

// <��ֵ���>::=<��ʶ��>:=<���ʽ>
void GrammarAnalyzer::ASSIGNMENT_STATEMENT()
{
	confirm(Word::IDENTIFIER);
	read();
	confirmName(":=");
	read();
	EXPRESSION();
}

// <�������> :: = begin<���>{ ; <���> }end
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
	confirm(Word::KW_END);
	read();
}

// <�������> ::= if<����>then<���>[else<���>]
void GrammarAnalyzer::CONDITIONAL_STATEMENT()
{
	confirm(Word::KW_IF);
	read();
	CONDITION();
	confirm(Word::KW_THEN);
	read();
	STATEMENT();
	if (!cur.isEmptyWord() && cur.type == Word::KW_ELSE) {
		read();
		STATEMENT();
	}
}
//<��ѭ�����>::=while<����>do<���>
void GrammarAnalyzer::WHILE_STATEMENT()
{
	confirm(Word::KW_WHILE);
	read();
	CONDITION();
	confirm(Word::KW_DO);
	read();
	STATEMENT();
}
// <���̵������>::=call<��ʶ��>
void GrammarAnalyzer::CALL_STATEMENT()
{
	confirm(Word::KW_CALL);
	read();
	confirm(Word::IDENTIFIER);
	read();
}

//<�����>::=read'('<��ʶ��>{,<��ʶ��>}')'
void GrammarAnalyzer::READ_STATEMENT()
{
	confirm(Word::KW_READ);
	read();
	confirmName("(");
	read();
	confirm(Word::IDENTIFIER);
	read();
	while (cur.name == ",") {
		read();
		confirm(Word::IDENTIFIER);
		read();
	}
	confirmName(")");
	read();
}

// <д���>::=write '('<���ʽ>{,<���ʽ>}')'
void GrammarAnalyzer::WRITE_STATEMENT()
{
	confirm(Word::KW_WRITE);
	read();
	confirmName("(");
	read();
	EXPRESSION();
	while (cur.name == ",") {
		read();
		EXPRESSION();
	}
	confirmName(")");
	read();
}

//<�ظ����> :: = repeat<���>{ ; <���> }until<����>
void GrammarAnalyzer::REPEAT_STATEMENT()
{
	confirm(Word::KW_REPEAT);
	read();
	STATEMENT();
	while (cur.name == ";") {
		read();
		STATEMENT();		
	}
	confirm(Word::KW_UNTIL);
	read();
	CONDITION();
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
	std::cout << "Type\tName\tValue\tLevel\tAddress" << std::endl;
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

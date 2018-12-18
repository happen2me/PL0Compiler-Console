#include "pch.h"
#include "GrammarAnalyzer.h"
#include <string>
#include <iostream>
#include "Error.h"
using namespace std;

//"begin", "end", "if", "then", "else", "const", "procedure", "var", "do", "while", "call", "read", "write", "odd", "repeat", "until"

std::string KW_BEGIN = "begin";
std::string KW_END = "end";
std::string KW_IF = "if";
std::string KW_THEN = "then";
std::string KW_ELSE = "else";
std::string KW_CONST = "const";
std::string KW_PROCEDURE = "procedure";
std::string KW_VAR = "var";
std::string KW_DO = "do";
std::string KW_WHILE = "while";
std::string KW_CALL = "call";
std::string KW_READ = "read";
std::string KW_WRITE = "write";
std::string KW_ODD = "odd";
std::string KW_REPEAT = "repeat";
std::string KW_UNTIL = "until";

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

//<主程序>::=<分程序>.
void GrammarAnalyzer::MAIN_PROC()
{
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
}

//<分程序>::=[<常量说明部分>][<变量说明部分>][<过程说明部分>]<语句>
void GrammarAnalyzer::SUB_PROC()
{
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

//<条件>::=<表达式><关系运算符><表达式> | odd<表达式>
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

//<因子>::=<标识符> | <无符号整数> | '('<表达式>')'
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

//<项>::=<因子>{<乘法运算符><因子>}
void GrammarAnalyzer::TERM()
{
	FACTOR();
	while (cur.name == "*" || cur.name == "/") {
		read();
		FACTOR();
	}
}

//<常量说明部分>::=const <常量定义>{,<常量定义>}
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
		Error::raiseMissingError(cur.line, KW_CONST);
	}
}

//<常量定义>::=<标识符>=<无符号整数>
void GrammarAnalyzer::CONST_DEFINITION()
{
	confirm(Word::IDENTIFIER);
	read();
	//confirmName("=");
	confirm(Word::OP_EQUAL);
	read();
	confirm(Word::CONST);
	read();
}
//<变量说明部分>::=var<标识符>{,<标识符>}
void GrammarAnalyzer::VAR_DECLARATION()
{
	confirm(Word::KW_VAR);
	read();
	confirm(Word::IDENTIFIER);
	read();
	while (cur.name == ",")
	{
		read();
		confirm(Word::IDENTIFIER);
		read();
	}
	confirm(Word::SP_SEMICOLON);
	read();
}

//<过程说明部分> ::= procedure<标识符>;<分程序>;{<过程说明部分>}
void GrammarAnalyzer::PROCEDURE_DECLARATION()
{
	confirm(Word::KW_PROCEDURE);
	read();
	confirm(Word::IDENTIFIER);
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

// <赋值语句>::=<标识符>:=<表达式>
void GrammarAnalyzer::ASSIGNMENT_STATEMENT()
{
	confirm(Word::IDENTIFIER);
	read();
	confirmName(":=");
	read();
	EXPRESSION();
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
	confirm(Word::KW_END);
	read();
}

// <条件语句> ::= if<条件>then<语句>[else<语句>]
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
//<当循环语句>::=while<条件>do<语句>
void GrammarAnalyzer::WHILE_STATEMENT()
{
	confirm(Word::KW_WHILE);
	read();
	CONDITION();
	confirm(Word::KW_DO);
	read();
	STATEMENT();
}
// <过程调用语句>::=call<标识符>
void GrammarAnalyzer::CALL_STATEMENT()
{
	confirm(Word::KW_CALL);
	read();
	confirm(Word::IDENTIFIER);
	read();
}

//<读语句>::=read'('<标识符>{,<标识符>}')'
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

// <写语句>::=write '('<表达式>{,<表达式>}')'
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

//<重复语句> :: = repeat<语句>{ ; <语句> }until<条件>
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

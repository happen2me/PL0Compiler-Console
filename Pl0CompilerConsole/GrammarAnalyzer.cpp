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
			nextWord = Word::CreateCommentWord();
		}
		return true;
	}
	else {
		cur = Word::CreateCommentWord();
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
		confirmValue(".");
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
	if (cur.name == KW_CONST) {
		CONST_DECLARATION();
	}
	if (cur.name == KW_VAR) {
		VAR_DECLARATION();
	}
	if (cur.name == KW_PROCEDURE) {
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
	else if (cur.type == Word::RESERVED) {
		if (cur.name == KW_IF) {
			CONDITIONAL_STATEMENT();
		}
		else if (cur.name == KW_WHILE) {
			WHILE_STATEMENT();
		}
		else if (cur.name == KW_CALL) {
			CALL_STATEMENT();
		}
		else if (cur.name == KW_BEGIN) {
			COMPOUND_STATEMENT();
		}
		else if (cur.name == KW_READ) {
			READ_STATEMENT();
		}
		else if (cur.name == KW_WRITE) {
			WRITE_STATEMENT();
		}
		else if (cur.name == KW_REPEAT) {
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
	if (cur.name == KW_ODD) {
		read();
		EXPRESSION();
	}
	else {
		EXPRESSION();
		//TODO shori
		confirmType(Word::BINARY_OPERATOR);
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
		confirmValue(")");
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
	if (cur.name == KW_CONST) {
		read();
		CONST_DEFINITION();
		while (cur.name == ",")
		{
			read();
			CONST_DEFINITION();
		}

		confirmValue(";");
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
	confirmType(Word::IDENTIFIER);
	read();
	confirmValue("=");
	read();
	confirmType(Word::CONST);
	read();
}
//<变量说明部分>::=var<标识符>{,<标识符>}
void GrammarAnalyzer::VAR_DECLARATION()
{
	confirmValue(KW_VAR);
	read();
	confirmType(Word::IDENTIFIER);
	read();
	while (cur.name == ",")
	{
		read();
		confirmType(Word::IDENTIFIER);
		read();
	}
	confirmValue(";");
	read();
}

//<过程说明部分> ::= procedure<标识符>;<分程序>;{<过程说明部分>}
void GrammarAnalyzer::PROCEDURE_DECLARATION()
{
	confirmValue(KW_PROCEDURE);
	read();
	confirmType(Word::IDENTIFIER);
	read();
	confirmValue(";");
	read();
	SUB_PROC();
	confirmValue(";");
	read();
	while (!cur.isEmptyWord() && cur.name == KW_PROCEDURE)
	{
		PROCEDURE_DECLARATION();
	}
}

// <赋值语句>::=<标识符>:=<表达式>
void GrammarAnalyzer::ASSIGNMENT_STATEMENT()
{
	confirmType(Word::IDENTIFIER);
	read();
	confirmValue(":=");
	read();
	EXPRESSION();
}

// <复合语句> :: = begin<语句>{ ; <语句> }end
void GrammarAnalyzer::COMPOUND_STATEMENT()
{
	confirmValue(KW_BEGIN);
	read();
	STATEMENT();
	while (cur.name == ";")
	{
		read();
		STATEMENT();
	}
	confirmValue(KW_END);
	read();
}

// <条件语句> ::= if<条件>then<语句>[else<语句>]
void GrammarAnalyzer::CONDITIONAL_STATEMENT()
{
	confirmValue(KW_IF);
	read();
	CONDITION();
	confirmValue(KW_THEN);
	read();
	STATEMENT();
	if (!cur.isEmptyWord() && cur.name == KW_ELSE) {
		read();
		STATEMENT();
	}
}
//<当循环语句>::=while<条件>do<语句>
void GrammarAnalyzer::WHILE_STATEMENT()
{
	confirmValue(KW_WHILE);
	read();
	CONDITION();
	confirmValue(KW_DO);
	read();
	STATEMENT();
}
// <过程调用语句>::=call<标识符>
void GrammarAnalyzer::CALL_STATEMENT()
{
	confirmValue(KW_CALL);
	read();
	confirmType(Word::IDENTIFIER);
	read();
}

//<读语句>::=read'('<标识符>{,<标识符>}')'
void GrammarAnalyzer::READ_STATEMENT()
{
	confirmValue(KW_READ);
	read();
	confirmValue("(");
	read();
	confirmType(Word::IDENTIFIER);
	read();
	while (cur.name == ",") {
		read();
		confirmType(Word::IDENTIFIER);
		read();
	}
	confirmValue(")");
	read();
}

// <写语句>::=write '('<表达式>{,<表达式>}')'
void GrammarAnalyzer::WRITE_STATEMENT()
{
	confirmValue(KW_WRITE);
	read();
	confirmValue("(");
	read();
	EXPRESSION();
	while (cur.name == ",") {
		read();
		EXPRESSION();
	}
	confirmValue(")");
	read();
}

//<重复语句> :: = repeat<语句>{ ; <语句> }until<条件>
void GrammarAnalyzer::REPEAT_STATEMENT()
{
	confirmValue(KW_REPEAT);
	read();
	STATEMENT();
	while (cur.name == ";") {
		read();
		STATEMENT();		
	}
	confirmValue(KW_UNTIL);
	read();
	CONDITION();
}

bool GrammarAnalyzer::confirmType(Word::WordType expectedType)
{
	if (!cur.isEmptyWord()) {
		if (cur.type == expectedType) {
			return true;
		}
		else {
			Error::raiseMissingError(cur.line, ""+expectedType);
		}
	}
	return false;
}

bool GrammarAnalyzer::confirmValue(std::string expectedVal)
{
	if (!cur.isEmptyWord() && cur.name == expectedVal) {
		return true;
	}
	Error::raiseMissingError(cur.line, expectedVal);
	return false;
}

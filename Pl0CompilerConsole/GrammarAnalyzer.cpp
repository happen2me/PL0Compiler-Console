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

//<������>::=<�ֳ���>.
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

//<�ֳ���>::=[<����˵������>][<����˵������>][<����˵������>]<���>
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

//<���>::=<��ֵ���> | <�������> | <��ѭ�����> | <���̵������> | <�������> | <�����> | <д���> | <�ظ����> | <��>
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
		confirmValue(")");
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

//<��������>::=<��ʶ��>=<�޷�������>
void GrammarAnalyzer::CONST_DEFINITION()
{
	confirmType(Word::IDENTIFIER);
	read();
	confirmValue("=");
	read();
	confirmType(Word::CONST);
	read();
}
//<����˵������>::=var<��ʶ��>{,<��ʶ��>}
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

//<����˵������> ::= procedure<��ʶ��>;<�ֳ���>;{<����˵������>}
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

// <��ֵ���>::=<��ʶ��>:=<���ʽ>
void GrammarAnalyzer::ASSIGNMENT_STATEMENT()
{
	confirmType(Word::IDENTIFIER);
	read();
	confirmValue(":=");
	read();
	EXPRESSION();
}

// <�������> :: = begin<���>{ ; <���> }end
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

// <�������> ::= if<����>then<���>[else<���>]
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
//<��ѭ�����>::=while<����>do<���>
void GrammarAnalyzer::WHILE_STATEMENT()
{
	confirmValue(KW_WHILE);
	read();
	CONDITION();
	confirmValue(KW_DO);
	read();
	STATEMENT();
}
// <���̵������>::=call<��ʶ��>
void GrammarAnalyzer::CALL_STATEMENT()
{
	confirmValue(KW_CALL);
	read();
	confirmType(Word::IDENTIFIER);
	read();
}

//<�����>::=read'('<��ʶ��>{,<��ʶ��>}')'
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

// <д���>::=write '('<���ʽ>{,<���ʽ>}')'
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

//<�ظ����> :: = repeat<���>{ ; <���> }until<����>
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

#include "pch.h"
#include "GrammarAnalyzer.h"
#include <string>
#include <iostream>
#include "Error.h"
using namespace std;

#define VERBOSE 0

#define DUMMY_PROC_ADDRESS -1
#define INVALID_ADDRESS -1

#define OT_RET 0
#define OT_NEG 1
#define OT_ADD 2
#define OT_SUB 2
#define OT_MUL 4
#define OT_DIV 5
#define OT_ODD 6
#define OT_MOD 7
#define OT_EQL 8
#define OT_NEQ 9
#define OT_LSS 10
#define OT_LEQ 11
#define OT_GTR 12
#define OT_GEQ 13

#define NOT_FOUNT -1

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
		if (VERBOSE) {
			std::cout << "pushed " << wordStack.top().name << std::endl;
		}		
	}
}


GrammarAnalyzer::~GrammarAnalyzer()
{
}

void GrammarAnalyzer::emit(inst::InstructionType type, int l, int m)
{
	pcodes.push_back(inst(type, l, m));
	cx++;
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

//<������>::=<�ֳ���>.
void GrammarAnalyzer::MAIN_PROC()
{
	lev = -1;
	cx = 0;
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
	printPcodes();
}

//<�ֳ���>::=[<����˵������>][<����˵������>][<����˵������>]<���>
void GrammarAnalyzer::SUB_PROC()
{
	int stored_lev = lev;
	lev += 1;
	int dx = 3;
	int stored_tx = getTx();
	int stored_cx = cx;

	emit(inst::JMP, 0, 0);

	if (cur.type == Word::KW_CONST) {
		CONST_DECLARATION();
	}
	if (cur.type == Word::KW_VAR) {
		VAR_DECLARATION(dx);
	}
	if (cur.type == Word::KW_PROCEDURE) {
		PROCEDURE_DECLARATION();
	}

	pcodes[stored_cx].m = cx;

	if (lev > 0) {
		table[stored_tx-1].address = cx;
	}
	

	emit(inst::INC, 0, dx); // space for var

	STATEMENT();

	emit(inst::OPR, 0, OT_RET);

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
	bool neg_start = false;
	if (cur.name == "+" || cur.name == "-") {
		neg_start = checkType(Word::OP_MINUS);
		read();
	}
	TERM();
	if (neg_start) {
		emit(inst::OPR, 0, OT_NEG);
	}

	while (cur.name == "+" || cur.name == "-")
	{
		bool is_plus = checkType(Word::OP_PLUS);
		read();
		TERM();
		if (is_plus) {
			emit(inst::OPR, 0, OT_ADD);
		}
		else {
			emit(inst::OPR, 0, OT_SUB);
		}
	}
}

//<����>::=<���ʽ><��ϵ�����><���ʽ> | odd<���ʽ>
void GrammarAnalyzer::CONDITION()
{
	if (cur.type == Word::KW_ODD) {
		read();
		EXPRESSION();
		emit(inst::OPR, 0, OT_ODD);
	}
	else {
		EXPRESSION();
		//TODO shori
		//confirmType(Word::BINARY_OPERATOR);
		if (!cur.isOperator()) {
			Error::raiseUnexpectedError(cur.line, cur.name);
		}
		Word::WordType opr_type = cur.type;
		read();
		EXPRESSION();
		switch (opr_type)
		{
		case Word::OP_EQUAL:
			emit(inst::OPR, 0, OT_EQL);
			break;
		case Word::OP_NOT_EQUAL:
			emit(inst::OPR, 0, OT_NEQ);
			break;
		case Word::OP_LESS:
			emit(inst::OPR, 0, OT_LSS);
			break;
		case Word::OP_LESS_EQUAL:
			emit(inst::OPR, 0, OT_LEQ);
			break;
		case Word::OP_ABOVE:
			emit(inst::OPR, 0, OT_GTR);
			break;
		case Word::OP_ABOVE_EQUAL:
			emit(inst::OPR, 0, OT_GEQ);
			break;
		default:
			//TODO unidentified opr
			break;
		}
	}
}

//<����>::=<��ʶ��> | <�޷�������> | '('<���ʽ>')'
void GrammarAnalyzer::FACTOR()
{
	if (cur.type == Word::IDENTIFIER) {

		int pos = position(cur.name, lev);

		if (pos == INVALID_ADDRESS) {
			//error
		}
		else {
			if (table[pos].type == Symbol::CONST) {
				emit(inst::LIT, 0, table[pos].val);
			}
			else if(table[pos].type == Symbol::VAR){
				emit(inst::LOD, lev - table[pos].level, table[pos].address);
			}
			else {
				//TODO: error
			}
		}

		read();
	}
	else if (cur.type == Word::CONST) {

		emit(inst::LIT, 0, cur.val);

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
		bool is_multiply = checkType(Word::OP_MULTIPLY);

		read();
		FACTOR();
		
		if (is_multiply) {
			emit(inst::OPR, 0, OT_MUL);
		}
		else {
			emit(inst::OPR, 0, OT_DIV);
		}
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
void GrammarAnalyzer::VAR_DECLARATION(int& dx)
{
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
	int pos = position(cur.name, lev);
	confirm(Word::IDENTIFIER);
	// TODO: undefined error
	read();
	confirmName(":=");
	read();
	EXPRESSION();
	if (pos != NOT_FOUNT) {
		emit(inst::STO, lev - table[pos].level, table[pos].address);
	}
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

	int jpc_cx = cx;

	emit(inst::JPC, 0, 0);

	STATEMENT();
	if (!cur.isEmptyWord() && cur.type == Word::KW_ELSE) {
		read();

		pcodes[jpc_cx].m = cx + 1; //jumps past if

		jpc_cx = cx; // change to jmp_cx

		emit(inst::JMP, 0, 0);

		STATEMENT();
	}
	pcodes[jpc_cx].m = cx;
}
//<��ѭ�����>::=while<����>do<���>
void GrammarAnalyzer::WHILE_STATEMENT()
{
	confirm(Word::KW_WHILE);
	read();

	int condition_cx = cx;

	CONDITION();

	int jpc_cx = cx;

	emit(inst::JPC, 0, 0);

	confirm(Word::KW_DO);
	read();
	STATEMENT();

	emit(inst::JMP, 0, condition_cx);
	pcodes[jpc_cx].m = cx; // points to code after statement
}

// <���̵������>::=call<��ʶ��>
void GrammarAnalyzer::CALL_STATEMENT()
{
	confirm(Word::KW_CALL);
	read();
	confirm(Word::IDENTIFIER);

	int pos = position(cur.name, lev);
	if (table[pos].type == Symbol::PROC) {
		emit(inst::CALL, lev - table[pos].level, table[pos].address);
	}

	read();
}

//<�����>::=read'('<��ʶ��>{,<��ʶ��>}')'
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
		emit(inst::RED, 0, 0); //��ȡԪ������ջ��

		int pos = position(cur.name, lev);

		if (pos != INVALID_ADDRESS) {
			emit(inst::WRT, lev - table[pos].level, table[pos].address); //��ջ��Ԫ��д��ָ��λ��
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
		emit(inst::WRT, 0, 1);
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

void GrammarAnalyzer::printPcodes()
{
	std::cout << endl;
	for (int i = 0; i < pcodes.size(); i++) {
		std::cout << i+1 << "\t" << inst::translator[pcodes[i].op] << "\t" << pcodes[i].l << "\t" << pcodes[i].m << std::endl;
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

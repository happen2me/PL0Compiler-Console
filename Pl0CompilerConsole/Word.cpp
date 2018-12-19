#include "pch.h"
#include "Word.h"
#include <bitset>


std::map<Word::WordType, std::string> Word::translator = {
		{Word::IDENTIFIER, 	"identifier"},
		{Word::CONST, 	"int"},
		{Word::KW_BEGIN, 	"begin"},
		{Word::KW_END, 	"end"},
		{Word::KW_IF, 	"if"},
		{Word::KW_THEN, 	"then"},
		{Word::KW_ELSE, 	"else"},
		{Word::KW_CONST, 	"const"},
		{Word::KW_PROCEDURE, 	"procedure"},
		{Word::KW_VAR, 	"var"},
		{Word::KW_DO, 	"do"},
		{Word::KW_WHILE, 	"while"},
		{Word::KW_CALL, 	"call"},
		{Word::KW_READ, 	"read"},
		{Word::KW_WRITE, 	"write"},
		{Word::KW_ODD, 	"odd"},
		{Word::KW_REPEAT, 	"repeat"},
		{Word::KW_UNTIL, 	"until"},
		{Word::OP_ASSIGN,	"OP_ASSIGN"},
		{Word::OP_PLUS,	"OP_PLUS"},
		{Word::OP_MINUS,	"OP_MINUS"},
		{Word::OP_MULTIPLY,	"OP_MULTIPLY"},
		{Word::OP_DIVIDE,	"OP_DIVIDE"},
		{Word::OP_EQUAL, "OP_EQUAL"},
		{Word::OP_NOT_EQUAL,	"OP_EQUAL"},
		{Word::OP_LESS,	"OP_NOT_EQUAL"},
		{Word::OP_LESS_EQUAL,	"OP_LESS"},
		{Word::OP_ABOVE,	"OP_LESS_EQUAL"},
		{Word::OP_ABOVE_EQUAL, 	"OP_ABOVE"},
		{Word::SP_LEFT_PAR,	"SP_LEFT_PAR"}, // (
		{Word::SP_RIGHT_PAR,	"SP_RIGHT_PAR"}, // )
		{Word::SP_COMMA, 	"SP_COMMA"}, // ,
		{Word::SP_COLON, 	"SP_COLON"}, // :
		{Word::SP_SEMICOLON,	"SP_SEMICOLON"}, //;
		{Word::SP_DOT,  	"SP_DOT"}
};


std::set<Word::WordType> Word::operaterSet = {
	OP_ASSIGN,
	OP_PLUS,
	OP_MINUS,
	OP_MULTIPLY,
	OP_DIVIDE,

	OP_EQUAL,
	OP_NOT_EQUAL,
	OP_LESS,
	OP_LESS_EQUAL,
	OP_ABOVE,
	OP_ABOVE_EQUAL,
};

std::set<Word::WordType> Word::separatorSet = {
	SP_LEFT_PAR, // (
	SP_RIGHT_PAR, // )
	SP_COMMA, // ,
	SP_COLON, // :
	SP_SEMICOLON, //;
	SP_DOT // .
};

std::set<Word::WordType> Word::reservedSet = {
	KW_BEGIN,
	KW_END,
	KW_IF,
	KW_THEN,
	KW_ELSE,
	KW_CONST,
	KW_PROCEDURE,
	KW_VAR,
	KW_DO,
	KW_WHILE,
	KW_CALL,
	KW_READ,
	KW_WRITE,
	KW_ODD,
	KW_REPEAT,
	KW_UNTIL,
};

Word::Word() :
	emptyWord(true)
{
}

// ³£Êý
Word::Word(int line, std::string name, WordType type, int val) :
	name(name),
	type(type),
	emptyWord(false),
	line(line),
	val(val)
{
}

Word::Word(int line, std::string name, WordType type) :
	name(name),
	type(type),
	emptyWord(false),
	line(line),
	val(0)
{
}

//Word::Word(int line, std::string name, WordType type, std::string val) :
//	name(name),
//	type(type),
//	val(val),
//	line(line)
//{
//}

Word::~Word()
{
}

Word Word::CreateEmptyWord()
{
	return Word();
}

bool Word::isEmptyWord()
{
	return emptyWord;
}

bool Word::isOperator()
{
	return operaterSet.find(type) != operaterSet.end();
}

bool Word::isRetionalOperator()
{
	bool yes = true;
	switch (type)
	{
	case Word::OP_EQUAL:
	case Word::OP_NOT_EQUAL:
	case Word::OP_LESS:
	case Word::OP_LESS_EQUAL:
	case Word::OP_ABOVE:
	case Word::OP_ABOVE_EQUAL:
		break;
	default:
		yes = false;
		break;
	}
	return yes;
}

bool Word::isSeperator()
{
	return separatorSet.count(type) != 0;
}

bool Word::isReserved()
{
	return reservedSet.count(type) != 0;
}

#pragma once
#include<string>
#include<map>
#include<set>

class Word
{
public:
	enum WordType
	{
		IDENTIFIER,
		CONST,

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

		SP_LEFT_PAR, // (
		SP_RIGHT_PAR, // )
		SP_COMMA, // ,
		SP_COLON, // :
		SP_SEMICOLON, //;
		SP_DOT,
	};


	Word(int line, std::string name, WordType type, int val);
	Word(int line, std::string name, WordType type);
	Word();
	~Word();

	static Word CreateEmptyWord();
	bool isEmptyWord();
	bool isOperator();
	bool isRetionalOperator();
	bool isSeperator();
	bool isReserved();

	std::string name;
	WordType type;
	int val;
	int line;

	static std::map<Word::WordType, std::string> translator;

private:
	bool empty_word;
	static std::set<WordType> operater_set;
	static std::set<WordType> separator_set;
	static std::set<WordType> reserved_set;
};


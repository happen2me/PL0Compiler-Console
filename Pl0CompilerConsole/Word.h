#pragma once
#include<string>

class Word
{
public:
	enum WordType
	{
		IDENTIFIER,
		RESERVED,
		CONST,
		UNARY_OPERATOR,
		BINARY_OPERATOR,
		SEPERATOR
	};

	Word(int line, std::string name, WordType type, int val);
	Word(int line, std::string name, WordType type);
	Word(int line, std::string name, WordType type, std::string val);
	Word();
	~Word();

	static Word CreateCommentWord();
	bool isEmptyWord();

	std::string name;
	WordType type;
	std::string val;
	int line;
private:
	bool emptyWord;
};


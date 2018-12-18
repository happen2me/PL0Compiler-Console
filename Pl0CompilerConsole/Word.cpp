#include "pch.h"
#include "Word.h"
#include <bitset>

Word::Word() :
	emptyWord(true)
{
}


Word::Word(int line, std::string name, WordType type, int val) :
	name(name),
	type(type),
	emptyWord(false),
	line(line)
{
	if (val < 256)
	{
		this->val = std::bitset<8>(val).to_string();
	}
	else
	{
		this->val = std::bitset<32>(val).to_string();
	}
}

Word::Word(int line, std::string name, WordType type) :
	name(name),
	type(type),
	emptyWord(false),
	line(line)
{
	val = "-";
}

Word::Word(int line, std::string name, WordType type, std::string val) :
	name(name),
	type(type),
	val(val),
	line(line)
{
}

Word::~Word()
{
}

Word Word::CreateCommentWord()
{
	return Word();
}

bool Word::isEmptyWord()
{
	return emptyWord;
}

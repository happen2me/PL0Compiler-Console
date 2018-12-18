#include "pch.h"
#include "WordAnalyzer.h"
#include <exception>
#include <iostream>

WordAnalyzer::WordAnalyzer()
{
}

WordAnalyzer::WordAnalyzer(std::string filename) :
	ch(' '),
	token(""),
	num(0),
	readPtr(0),
	lineCounter(0)
{
	readFile(filename);
}

char WordAnalyzer::getchar()
{
	if (readPtr < buffer.size())
	{
		ch = buffer[readPtr];
		++readPtr;
	}
	else
	{
		ch = '#';
	}
	return ch;
}

void WordAnalyzer::clearToken()
{
	token.clear();
}

bool WordAnalyzer::isLetter()
{
	if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
		return true;
	return false;
}

bool WordAnalyzer::isDigit()
{
	return ch >= '0' && ch <= '9';
}

bool WordAnalyzer::isSpace()
{
	return ch == ' ';
}

bool WordAnalyzer::isNewLine()
{
	return ch == '\n';
}

bool WordAnalyzer::isTab()
{
	return ch == '\t';
}

bool WordAnalyzer::isColon()
{
	return ch == ':';
}

bool WordAnalyzer::isComma()
{
	return ch == ',';
}

bool WordAnalyzer::isSemi()
{
	return ch == ';';
}

bool WordAnalyzer::isEqual()
{
	return ch == '=';
}

bool WordAnalyzer::isPlus()
{
	return ch == '+';
}

bool WordAnalyzer::isMinus()
{
	return ch == '-';
}

bool WordAnalyzer::isDivide()
{
	return ch == '/';
}

bool WordAnalyzer::isStar()
{
	return ch == '*';
}

bool WordAnalyzer::isLpar()
{
	return ch == '(';
}

bool WordAnalyzer::isRpar()
{
	return ch == ')';
}

bool WordAnalyzer::isLess()
{
	return ch == '<';
}

bool WordAnalyzer::isAbove()
{
	return ch == '>';
}

bool WordAnalyzer::isDot()
{
	return ch == '.';
}

void WordAnalyzer::catToken()
{
	token += ch;
}

void WordAnalyzer::retract()
{
	--readPtr;
	ch = ' ';
}

Word::WordType WordAnalyzer::reserver()
{
	for (size_t i = 0; i < reservedWords.size(); i++) {
		if (token == reservedWords[i])
			return Word::RESERVED;
	}
	return Word::IDENTIFIER;
}

int WordAnalyzer::transNum()
{
	return std::stoi(token);
}

void WordAnalyzer::error(std::string message)
{
	throw std::exception(message.c_str());
}

void WordAnalyzer::analyze()
{
	while (readPtr < buffer.length()) {
		try
		{
			Word word = identifyWord();
			if (!word.isEmptyWord()) {
				results.push_back(word);
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}

	}
}

std::vector<Word> WordAnalyzer::getResult()
{
	return results;
}


WordAnalyzer::~WordAnalyzer()
{
}

void WordAnalyzer::readFile(std::string filename)
{
	try
	{
		std::ifstream in(filename);
		std::string contents((std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>());
		buffer = contents;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

Word WordAnalyzer::identifyWord()
{
	clearToken();
	getchar();
	while (isSpace() || isNewLine() || isTab())
	{
		if (ch == '\n')
			lineCounter++;
		getchar();
	}

	if (isDigit())
	{
		bool isFloat = false;
		while (isDigit())
		{
			catToken();
			getchar();
		}
		if (isDot())
		{
			isFloat = true;
			catToken();
			while (isDigit())
			{
				catToken();
				getchar();
			}
		}
		if (isLetter()) {
			while (isDigit() || isLetter())
			{
				catToken();
				getchar();
			}
			retract();
			error("Identifier should not start with digit; at line " + lineCounter);
			return Word();
		}
		retract();
		if (isFloat) {
			return Word(lineCounter, token, Word::CONST, token);
		}

		int numVal = transNum();
		return Word(lineCounter, token, Word::CONST, numVal);
	}
	else if (isLetter())  //Start with letter
	{
		while (isLetter() || isDigit())
		{
			catToken();
			getchar();
		}
		retract();
		symbol = reserver();
	}
	else if (isColon()) {
		catToken();
		getchar();
		if (isEqual())
		{
			catToken();
			symbol = Word::BINARY_OPERATOR; // :=
		}
		else
		{
			retract();
			symbol = Word::SEPERATOR; // :
		}
	}
	else if (isEqual())
	{
		catToken();
		symbol = Word::BINARY_OPERATOR;	// =
	}
	else if (isPlus())
	{
		catToken();
		symbol = Word::BINARY_OPERATOR;	// +
	}
	else if (isMinus())
	{
		catToken();
		symbol = Word::BINARY_OPERATOR;	// -
	}
	else if (isStar())
	{
		catToken();
		symbol = Word::BINARY_OPERATOR;	// *
	}
	else if (isLpar())
	{
		catToken();
		symbol = Word::SEPERATOR;		// (
	}
	else if (isRpar())
	{
		catToken();
		symbol = Word::SEPERATOR;		// )
	}
	else if (isComma())
	{
		catToken();
		symbol = Word::SEPERATOR;	// ,
	}
	else if (isSemi())
	{
		catToken();
		symbol = Word::SEPERATOR;	// ;
	}
	else if (ch == '<')
	{
		catToken();
		getchar();
		if (ch == '=')
		{
			catToken();
			symbol = Word::BINARY_OPERATOR;	// <=
		}
		else if (ch == '>')
		{
			catToken();
			symbol = Word::BINARY_OPERATOR; // <>
		}
		else
		{
			retract();
			symbol = Word::BINARY_OPERATOR; //<
		}
	}
	else if (ch == '>')
	{
		catToken();
		getchar();
		if (ch == '=')
		{
			catToken();
			symbol = Word::BINARY_OPERATOR; // >=
		}
		else
		{
			retract();
			symbol = Word::BINARY_OPERATOR; // >
		}
	}
	else if (isDivide())
	{
		catToken();
		getchar();
		if (isStar())//comments
		{
			catToken();
			do
			{
				do
				{
					getchar();
				} while (!isStar());

				do
				{
					getchar();
					if (isDivide())
					{
						return Word::CreateCommentWord(); //processed comments
					}
				} while (isStar());
			} while (!isStar());
		}
		else if (ch == '#')
		{
			return Word();
		}
		else
		{
			retract();
			symbol = Word::BINARY_OPERATOR;	//"/"
		}
	}
	else if (isDot())
	{
		catToken();
		symbol = Word::BINARY_OPERATOR;

	}
	else
	{
		error("Not match at line " + lineCounter);
	}

	return Word(lineCounter, token, symbol);
}


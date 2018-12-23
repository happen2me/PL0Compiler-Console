#include "pch.h"
#include "WordAnalyzer.h"
#include <exception>
#include <iostream>

std::map<std::string, Word::WordType> WordAnalyzer::reserved_word_map = {
	{"begin", Word::KW_BEGIN},
	{"end", Word::KW_END},
	{"if", Word::KW_IF},
	{"then", Word::KW_THEN},
	{"else", Word::KW_ELSE},
	{"const", Word::KW_CONST},
	{"procedure", Word::KW_PROCEDURE},
	{"var", Word::KW_VAR},
	{"do", Word::KW_DO},
	{"while", Word::KW_WHILE},
	{"call", Word::KW_CALL},
	{"read", Word::KW_READ},
	{"write", Word::KW_WRITE},
	{"odd", Word::KW_ODD},
	{"repeat", Word::KW_REPEAT},
	{"until", Word::KW_UNTIL}
};

std::vector<std::string> WordAnalyzer::reserved_words = { "begin", "end", "if", "then", "else", "const", "procedure", "var", "do", "while", "call", "read", "write", "odd", "repeat", "until" };

std::vector<std::string> WordAnalyzer::types_name = { "IDENTIFIER",	"RESERVED",	"CONST", "UNARY_OPERATOR", "BINARY_OPERATOR", "SEPERATOR" };

WordAnalyzer::WordAnalyzer()
{
}

WordAnalyzer::WordAnalyzer(std::string filename) :
	ch(' '),
	token(""),
	num(0),
	readPtr(0),
	line_counter(1)
{
	readFile(filename);
}

char WordAnalyzer::getchar()
{
	if (readPtr < (int)(buffer.size()))
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
	/*for (size_t i = 0; i < reserved_words.size(); i++) {
		if (token == reserved_words[i])
			return Word::RESERVED;
	}*/
	std::map<std::string, Word::WordType>::iterator it = reserved_word_map.find(token);
	if (it != reserved_word_map.end()) {
		return it->second;
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
	while (readPtr < (int)buffer.length()) {
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

std::string WordAnalyzer::getBuffer()
{
	return buffer;
}

void WordAnalyzer::printResult(std::ostream& out) {
	out << std::endl;
	out.width(12);
	out << std::left << "Name";
	out.width(15);
	out << std::left << "Type";
	out << std::left << "Value" << std::endl;
	for (int i = 0; i < (int)results.size(); i++) {
		out.width(12);
		out << std::left << results[i].name;
		out.width(15);
		out << Word::translator[results[i].type];
		if (results[i].type == Word::CONST) {
			out << std::left << results[i].val << std::endl;
		}
		else {
			out << std::left << "-" << std::endl;
		}
	}
}

void WordAnalyzer::readFile(std::string filename)
{
	if (!fileExits(filename)) {
		throw std::exception("Failed to open file, probably you input wrong file name");
	}
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
			line_counter++;
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
			error("Identifier should not start with digit; at line " + line_counter);
			return Word();
		}
		retract();

		int numVal = transNum();

		// TODO float
		if (isFloat) {
			return Word(line_counter, token, Word::CONST, numVal);
		}
		return Word(line_counter, token, Word::CONST, numVal);
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
			symbol = Word::OP_ASSIGN; // :=
		}
		else
		{
			retract();
			symbol = Word::SP_COLON; // :
		}
	}
	else if (isEqual())
	{
		catToken();
		symbol = Word::OP_EQUAL;	// =
	}
	else if (isPlus())
	{
		catToken();
		symbol = Word::OP_PLUS;	// +
	}
	else if (isMinus())
	{
		catToken();
		symbol = Word::OP_MINUS;	// -
	}
	else if (isStar())
	{
		catToken();
		symbol = Word::OP_MULTIPLY;	// *
	}
	else if (isLpar())
	{
		catToken();
		symbol = Word::SP_LEFT_PAR;		// (
	}
	else if (isRpar())
	{
		catToken();
		symbol = Word::SP_RIGHT_PAR;		// )
	}
	else if (isComma())
	{
		catToken();
		symbol = Word::SP_COMMA;	// ,
	}
	else if (isSemi())
	{
		catToken();
		symbol = Word::SP_SEMICOLON;	// ;
	}
	else if (ch == '<')
	{
		catToken();
		getchar();
		if (ch == '=')
		{
			catToken();
			symbol = Word::OP_LESS_EQUAL;	// <=
		}
		else if (ch == '>')
		{
			catToken();
			symbol = Word::OP_NOT_EQUAL; // <>
		}
		else
		{
			retract();
			symbol = Word::OP_LESS; //<
		}
	}
	else if (ch == '>')
	{
		catToken();
		getchar();
		if (ch == '=')
		{
			catToken();
			symbol = Word::OP_ABOVE_EQUAL; // >=
		}
		else
		{
			retract();
			symbol = Word::OP_ABOVE; // >
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
						return Word::CreateEmptyWord(); //processed comments
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
			symbol = Word::OP_DIVIDE;	//"/"
		}
	}
	else if (isDot())
	{
		catToken();
		symbol = Word::SP_DOT; // .

	}
	else
	{
		error("Not match at line " + line_counter);
	}

	return Word(line_counter, token, symbol);
}

bool WordAnalyzer::fileExits(std::string filename)
{
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);
}



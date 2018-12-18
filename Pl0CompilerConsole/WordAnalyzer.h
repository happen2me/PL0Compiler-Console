#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include "Word.h"

class WordAnalyzer
{
public:
	WordAnalyzer();
	WordAnalyzer(std::string filename);

	char getchar();
	void clearToken();

	bool isLetter();
	bool isDigit();
	bool isSpace();
	bool isNewLine();
	bool isTab();
	bool isColon(); //ð��
	bool isComma();
	bool isSemi();	//�ֺ�
	bool isEqual();
	bool isPlus();
	bool isMinus();
	bool isDivide();
	bool isStar();
	bool isLpar();
	bool isRpar();
	bool isLess();
	bool isAbove();
	bool isDot();

	void catToken(); //���ӵ�ǰ�ַ�ch�뵥�ʴ�token
	void retract(); //���˶��ַ�
	Word::WordType reserver(); //��ʶ������0�������ַ��ر����������
	int transNum();	//�����ʴ�tokenת��Ϊ������ֵ
	void error(std::string);
	void analyze();

	std::vector<Word> getResult();

	std::vector<std::string> reservedWords = { "begin", "end", "if", "then", "else", "const", "procedure", "var", "do", "while", "call", "read", "write", "odd", "repeat", "until" };
	std::map<std::string, Word::WordType> reservedWordMap = {
		{	"begin", 	Word::KW_BEGIN},
		{	"end", 	Word::KW_END},
		{	"if", 	Word::KW_IF},
		{	"then", 	Word::KW_THEN},
		{	"else", 	Word::KW_ELSE},
		{	"const", 	Word::KW_CONST},
		{	"procedure", 	Word::KW_PROCEDURE},
		{	"var", 	Word::KW_VAR},
		{	"do", 	Word::KW_DO},
		{	"while", 	Word::KW_WHILE},
		{	"call", 	Word::KW_CALL},
		{	"read", 	Word::KW_READ},
		{	"write", 	Word::KW_WRITE},
		{	"odd", 	Word::KW_ODD},
		{	"repeat", 	Word::KW_REPEAT},
		{	"until",	Word::KW_UNTIL}
	};

	~WordAnalyzer();

	std::string getBuffer() {
		return buffer;
	}

	void printResult();

	std::vector<std::string> typesName = { "IDENTIFIER",	"RESERVED",	"CONST", "UNARY_OPERATOR", "BINARY_OPERATOR", "SEPERATOR" };
private:
	char ch;
	std::string token; //���ʴ�
	int num;
	Word::WordType symbol;
	std::string buffer;
	int readPtr;
	std::vector<Word> results;
	int lineCounter;

	void readFile(std::string filename);
	Word identifyWord();

};


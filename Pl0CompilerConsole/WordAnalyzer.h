#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
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
	bool isColon(); //冒号
	bool isComma();
	bool isSemi();	//分号
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

	void catToken(); //连接当前字符ch与单词串token
	void retract(); //后退读字符
	Word::WordType reserver(); //标识符返回0，保留字返回保留字类编码
	int transNum();	//将单词串token转换为整形数值
	void error(std::string);
	void analyze();

	std::vector<Word> getResult();

	std::vector<std::string> reservedWords = { "begin", "end", "if", "then", "else", "const", "procedure", "var", "do", "while", "call", "read", "write", "odd", "repeat", "until" };
	~WordAnalyzer();

	std::string getBuffer() {
		return buffer;
	}

	void printResult() {
		std::cout << "Name" << "\t\t" << "Type" << "\t\t" << "Value" << std::endl;
		for (int i = 0; i < results.size(); i++) {
			std::cout << results[i].name << "\t\t" << typesName[(int)results[i].type] << "\t\t" << results[i].val << std::endl;
		}
	}

	std::vector<std::string> typesName = { "IDENTIFIER",	"RESERVED",	"CONST", "UNARY_OPERATOR", "BINARY_OPERATOR", "SEPERATOR" };
private:
	char ch;
	std::string token; //单词串
	int num;
	Word::WordType symbol;
	std::string buffer;
	int readPtr;
	std::vector<Word> results;
	int lineCounter;

	void readFile(std::string filename);
	Word identifyWord();

};


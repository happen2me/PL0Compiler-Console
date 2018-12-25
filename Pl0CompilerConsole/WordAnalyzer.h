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
	WordAnalyzer(std::string filename, std::ostream &out);
	~WordAnalyzer();
	
	void error(std::string, bool fatal = false);
	void analyze();
	std::vector<Word> getResult();
	std::string getBuffer();
	void printResult(std::ostream& out);
	int getErrorCount();

	static std::vector<std::string> reserved_words;
	static std::map<std::string, Word::WordType> reserved_word_map;
	static std::vector<std::string> types_name;

private:
	char ch;
	std::string token; //单词串
	int num;
	Word::WordType symbol;
	std::string buffer;
	int readPtr;
	std::vector<Word> results;
	int line_counter;
	std::ostream& out;
	int errorCnt;

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

	void readFile(std::string filename);
	Word identifyWord();
	bool fileExits(std::string filename);

};


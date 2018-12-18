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


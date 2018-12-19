#pragma once
#include <vector>
#include <stack>
#include "WordAnalyzer.h"
#include "Symbol.h"
#include "Instruction.h"
#include "Error.h"

class GrammarAnalyzer
{
public:
	GrammarAnalyzer();
	GrammarAnalyzer(const std::vector<Word>& wordList);
	virtual ~GrammarAnalyzer();
	void runCompile();

private:
	std::stack<Word> wordStack;
	Word cur;
	Word nextWord;
	/*For Translate*/
	std::vector<Symbol> table; //���ű�
	std::vector<inst> pcodes;
	int lev;
	int cx;
	int errorCnt;
	void emit(inst::InstructionType type, int l, int m);

	bool read();

	void MAIN_PROC();
	void SUB_PROC();
	void STATEMENT();
	void EXPRESSION(); //<���ʽ>::=[+|-]<��>{<�ӷ������><��>}
	void CONDITION();
	void FACTOR(); //<����>::=<��ʶ��> | <�޷�������> | '('<���ʽ>')'
	void TERM(); //<��>::=<����>{<�˷������><����>}
	void CONST_DECLARATION();
	void CONST_DEFINITION();
	void VAR_DECLARATION(int& dx);
	void PROCEDURE_DECLARATION();
	void ASSIGNMENT_STATEMENT(); //<��ֵ���>::=<��ʶ��>:=<���ʽ>
	void COMPOUND_STATEMENT(); //<�������>::=begin<���>{;<���>}end
	void CONDITIONAL_STATEMENT(); //<�������>::=if<����>then<���>else<���>
	void WHILE_STATEMENT(); //<��ѭ�����>::=while<����>do<���>
	void CALL_STATEMENT(); // <���̵������>::=call<��ʶ��>
	void READ_STATEMENT(); //<�����>::=read'('<��ʶ��>{,<��ʶ��>}')'
	void WRITE_STATEMENT(); //<д���>::=write '('<���ʽ>{,<���ʽ>}')'
	void REPEAT_STATEMENT(); //<�ظ����> :: = repeat<���>{ ; <���> }until<����>


	bool confirm(Word::WordType expectedType);
	bool confirmName(std::string expectedVal);
	bool checkType(Word::WordType expectedType);

	/*For translate*/
	void enter(Symbol::SymbolType type, std::string name, int value); // enter const
	void enter(Symbol::SymbolType type, std::string name, int level, int address); // enter var and proc
	bool checkDup(std::string name, int level);
	int find(std::string name); // return -1 if not find

	void printSymbolTable();
	void printPcodes();
	int position(std::string identifier, int level);
	int getTx();
	bool test(int line, Word::WordType word_type, Error::ErrorType error_type);
	void my_raise(int line, Error::ErrorType errorType){
		std::cout << "Error " << errorType << " happened in line : " << line << std::endl;
		std::cout << "\t" << cur.name << " read" << std::endl;
	}
};


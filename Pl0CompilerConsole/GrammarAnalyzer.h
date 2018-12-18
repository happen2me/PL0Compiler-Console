#pragma once
#include <vector>
#include <stack>
#include "WordAnalyzer.h"

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
	void VAR_DECLARATION();
	void PROCEDURE_DECLARATION();
	void ASSIGNMENT_STATEMENT(); //<��ֵ���>::=<��ʶ��>:=<���ʽ>
	void COMPOUND_STATEMENT(); //<�������>::=begin<���>{;<���>}end
	void CONDITIONAL_STATEMENT(); //<�������>::=if<����>then<���>else<���>
	void WHILE_STATEMENT(); //<��ѭ�����>::=while<����>do<���>
	void CALL_STATEMENT(); // <���̵������>::=call<��ʶ��>
	void READ_STATEMENT(); //<�����>::=read'('<��ʶ��>{,<��ʶ��>}')'
	void WRITE_STATEMENT(); //<д���>::=write '('<���ʽ>{,<���ʽ>}')'
	void REPEAT_STATEMENT(); //<�ظ����> :: = repeat<���>{ ; <���> }until<����>


	bool confirmType(Word::WordType expectedType);
	bool confirmValue(std::string expectedVal);
};


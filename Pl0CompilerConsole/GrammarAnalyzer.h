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
	std::vector<Instruction> getResults();
	bool errorHappened();
	void printSymbolTable(std::ostream& out);
	void printPcodes(std::ostream& out);

private:
	std::stack<Word> word_stack;
	Word current_word;
	Word next_word; // In fact it's never been used
	/*For parsing stage*/
	std::vector<Symbol> table; //符号表
	std::vector<Instruction> pcodes;
	int lev;
	int error_count;
	void emit(Instruction::InstructionType type, int l, int m);

	bool read();

	void MAIN_PROC();
	void SUB_PROC();
	void STATEMENT();
	void EXPRESSION(); //<表达式>::=[+|-]<项>{<加法运算符><项>}
	void CONDITION();
	void FACTOR(); //<因子>::=<标识符> | <无符号整数> | '('<表达式>')'
	void TERM(); //<项>::=<因子>{<乘法运算符><因子>}
	void CONST_DECLARATION();
	void CONST_DEFINITION();
	void VAR_DECLARATION(int& dx);
	void PROCEDURE_DECLARATION();
	void ASSIGNMENT_STATEMENT(); //<赋值语句>::=<标识符>:=<表达式>
	void COMPOUND_STATEMENT(); //<复合语句>::=begin<语句>{;<语句>}end
	void CONDITIONAL_STATEMENT(); //<条件语句>::=if<条件>then<语句>else<语句>
	void WHILE_STATEMENT(); //<当循环语句>::=while<条件>do<语句>
	void CALL_STATEMENT(); // <过程调用语句>::=call<标识符>
	void READ_STATEMENT(); //<读语句>::=read'('<标识符>{,<标识符>}')'
	void WRITE_STATEMENT(); //<写语句>::=write '('<表达式>{,<表达式>}')'
	void REPEAT_STATEMENT(); //<重复语句> :: = repeat<语句>{ ; <语句> }until<条件>


	bool confirm(Word::WordType expectedType);
	bool confirmName(std::string expectedVal);
	bool checkType(Word::WordType expectedType);

	/*For translate*/
	void enter(Symbol::SymbolType type, std::string name, int value); // enter const
	void enter(Symbol::SymbolType type, std::string name, int level, int address); // enter var and proc
	bool checkDup(std::string name, int level);
	int find(std::string name); // return -1 if not find

	int position(std::string identifier, int level);
	int getTx();
	int getCx();
	bool test(int line, Word::WordType word_type, Error::ErrorType error_type);
	void raiseWrapper(int line, Error::ErrorType errorType);
	void raiseWrapper(int line, Error::ErrorType errorType, std::string expectation);
};


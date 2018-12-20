#pragma once
#include<string>
#include<map>
/*
	Item of Symbol Table
*/
class Symbol
{
public:
	enum SymbolType {
		CONST = 1,
		VAR,
		PROC
	};

	enum OperationType {
		OT_RET,
		OT_NEG,
		OT_ADD,
		OT_SUB,
		OT_MUL,
		OT_DIV,
		OT_ODD,
		OT_MOD,
		OT_EQL,
		OT_NEQ,
		OT_LSS,
		OT_LEQ,
		OT_GTR,
		OT_GEQ,
	};

	static std::map<SymbolType, std::string> translator;

	Symbol();
	Symbol(SymbolType type, std::string name, int val);
	Symbol(SymbolType type, std::string name, int level, int address);
	virtual ~Symbol();

	/*
	 For constants: TYPE, NAME and VALUE
	 For variables: TYPE, NAME, LEVEL and ADDRESS
	 For procedures: TYPE, NAME, LEVEL and ADDRESS
	*/
	SymbolType type;
	std::string name;
	int val;
	int level;
	int address;
};


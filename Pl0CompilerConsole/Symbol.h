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


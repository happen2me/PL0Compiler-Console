#include "pch.h"
#include "Symbol.h"

constexpr auto INVALID_VAL = -1;
constexpr auto INVALID_LEVEL = -1;
constexpr auto INVALID_ADDRESS = -1;

std::map<Symbol::SymbolType, std::string> Symbol::translator = {
	{Symbol::CONST, "CONST"},
	{Symbol::PROC, "PROC"},
	{Symbol::VAR, "VAR"}
};

Symbol::Symbol()
{
}

Symbol::Symbol(SymbolType type, std::string name, int val):
	type(type),
	name(name),
	val(val),
	level(INVALID_LEVEL),
	address(INVALID_ADDRESS)
{
}

Symbol::Symbol(SymbolType type, std::string name, int level, int address):
	type(type),
	name(name),
	level(level),
	address(address),
	val(INVALID_VAL)
{
}

Symbol::~Symbol()
{
}

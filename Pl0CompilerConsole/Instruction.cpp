#include "pch.h"
#include "Instruction.h"

std::map<Instruction::InstructionType, std::string> Instruction::translator = {
	{Instruction::LIT, "LIT"},// LIT	0, M	Push constant value (literal) M onto the stack
	{Instruction::OPR, "OPR"}, // OPR	0, M	Operation to be performed on the data at the top of the stack
	{Instruction::LOD, "LOD"},// LOD	L, M	Load value to top of stack from the stack location at offset M from L lexicographical levels down
	{Instruction::STO, "STO"}, // STO	L, M	Store value at top of stack in the stack location at offset M from
	{Instruction::CALL, "CALL"},//CAL	L, M	Call procedure at code index M (generates new Activation Record and pc <- M)
	{Instruction::INC, "INC"},// INC	0, M	Allocate M locals (increment sp by M). First four are: 
			//Functional value(FL), Static Link(SL),
			//Dynamic Link(DL), and Return Address(RA)
	{Instruction::JMP, "JMP"},// JMP	0, M	Jump to instruction M
	{Instruction::JPC, "JPC"},// JPC	0, M	Jump to instruction M if top stack element is 0
	{Instruction::WRT, "WRT"},// WRT	0, 1	Write the top stack element to the screen
	{Instruction::RED, "RED"},

};

std::map<Instruction::OperationType, std::string> Instruction::op_translator = {
	{Instruction::OT_RET, "RET"},
	{Instruction::OT_NEG, "NEG"},
	{Instruction::OT_ADD, "ADD"},
	{Instruction::OT_SUB, "SUB"},
	{Instruction::OT_MUL, "MUL"},
	{Instruction::OT_DIV, "DIV"},
	{Instruction::OT_ODD, "ODD"},
	{Instruction::OT_MOD, "MOD"},
	{Instruction::OT_EQL, "EQL"},
	{Instruction::OT_NEQ, "NEQ"},
	{Instruction::OT_LSS, "LSS"},
	{Instruction::OT_LEQ, "LEG"},
	{Instruction::OT_GTR, "GTR"},
	{Instruction::OT_GEQ, "GEQ"}
};

Instruction::Instruction()
{
}

Instruction::Instruction(InstructionType type, int l, int m):
	op(type),
	l(l),
	m(m)
{
}


Instruction::~Instruction()
{
}

#include "pch.h"
#include "Instruction.h"

std::map<inst::InstructionType, std::string> inst::translator = {
	{inst::LIT, "LIT"},// LIT	0, M	Push constant value (literal) M onto the stack
	{inst::OPR, "OPR"}, // OPR	0, M	Operation to be performed on the data at the top of the stack
	{inst::LOD, "LOD"},// LOD	L, M	Load value to top of stack from the stack location at offset M from L lexicographical levels down
	{inst::STO, "STO"}, // STO	L, M	Store value at top of stack in the stack location at offset M from
	{inst::CALL, "CALL"},//CAL	L, M	Call procedure at code index M (generates new Activation Record and pc <- M)
	{inst::INC, "INC"},// INC	0, M	Allocate M locals (increment sp by M). First four are: 
			//Functional value(FL), Static Link(SL),
			//Dynamic Link(DL), and Return Address(RA)
	{inst::JMP, "JMP"},// JMP	0, M	Jump to instruction M
	{inst::JPC, "JPC"},// JPC	0, M	Jump to instruction M if top stack element is 0
	{inst::WRT, "WRT"},// WRT	0, 1	Write the top stack element to the screen
	{inst::RED, "RED"}
};

inst::inst()
{
}

inst::inst(InstructionType type, int l, int m):
	op(type),
	l(l),
	m(m)
{
}


inst::~inst()
{
}

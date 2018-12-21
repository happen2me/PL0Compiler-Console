#pragma once
#include<map>
#include<string>

class Instruction
{
public:
	enum InstructionType {
		LIT, // LIT	0, M	Push constant value (literal) M onto the stack
		OPR, // OPR	0, M	Operation to be performed on the data at the top of the stack
		LOD, // LOD	L, M	Load value to top of stack from the stack location at offset M from L lexicographical levels down
		STO, // STO	L, M	Store value at top of stack in the stack location at offset M from
		CALL, //CAL	L, M	Call procedure at code index M (generates new Activation Record and pc <- M)
		INC, // INC	0, M	Allocate M locals (increment sp by M). First four are: 
				//Functional value(FL), Static Link(SL),
				//Dynamic Link(DL), and Return Address(RA)
		JMP, // JMP	0, M	Jump to instruction M
		JPC, // JPC	0, M	Jump to instruction M if top stack element is 0
		WRT, // WRT	0, 1	Write the top stack element to the screen
		RED  // RED	0, 2	Read in input from the user and store it at the top of the stack
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

	static std::map<InstructionType, std::string> translator;
	static std::map<OperationType, std::string> op_translator;

	Instruction();
	Instruction(InstructionType type, int l, int m);
	~Instruction();

	InstructionType op;
	int l;
	int m;
};


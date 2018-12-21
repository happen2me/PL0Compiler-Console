#pragma once
#include <vector>
#include <string>
#include "Instruction.h"

#define MAX_RUN_STACK_HEIGHT 1000

class Interpreter
{
public:
	Interpreter();
	Interpreter(std::vector<Instruction> pcodes);
	~Interpreter();

	void run();

private:
	std::vector<Instruction> instructions;
	int data[MAX_RUN_STACK_HEIGHT] = { 0, };
	int pc; // program vounter
	int bp; // base pointer
	int sp; // stack pointer
	Instruction ir; // instruction register

	void exe();
	void opr(Instruction::OperationType opr_type);
	int base(int b_ptr, int level);
	void push(int x);
	int pop();
	bool fetch(); // fetch an instruction into ir
	void snapshotStack(std::ostream& out);
};


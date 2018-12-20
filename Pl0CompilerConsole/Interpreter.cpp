#include "pch.h"

#include <iostream>
#include "Interpreter.h"

Interpreter::Interpreter()
{
}

Interpreter::Interpreter(std::vector<Instruction> pcodes):
	instructions(pcodes),
	pc(0),
	bp(1),
	sp(0)
{
}


Interpreter::~Interpreter()
{
}

void Interpreter::run()
{
	while (bp != 0 && fetch()) {
		exe();
	}
}

void Interpreter::exe()
{
	switch (ir.op)
	{
	case Instruction::LIT:
		push(ir.m);
		break;
	case Instruction::OPR:
		opr((Instruction::OperationType)ir.m);
		break;
	case Instruction::LOD:
		push(data[base(bp, ir.l)+ir.m]);
		break;
	case Instruction::STO:
		data[base(bp, ir.l) + ir.m] = pop();
		break;
	case Instruction::CALL:
		push(base(bp, ir.l)); // static link
		push(bp); // dynamic link
		push(pc); // pc points to next instruction to execute
		break;
	case Instruction::INC:
		sp += ir.m;
		break;
	case Instruction::JMP:
		pc = ir.m;
		break;
	case Instruction::JPC:
		if (pop() == 0) {
			pc = ir.m;
		}
		break;
	case Instruction::WRT:
		std::cout << pop() << std::endl;
		break;
	case Instruction::RED:
		int x;
		std::cin >> x;
		push(x);
		break;
	default:
		break;
	}
}

void Interpreter::opr(Instruction::OperationType opr_type)
{
	int a, b, stored_bp;
	switch (opr_type)
	{
	case Instruction::OT_RET:
		pc = data[bp + 2];
		stored_bp = bp;
		bp = bp + 1;
		sp = stored_bp - 1;
		break;
	case Instruction::OT_NEG:
		push(-pop());
		break;
	case Instruction::OT_ADD:
		a = pop();
		b = pop();
		push(a + b);
		break;
	case Instruction::OT_SUB:
		a = pop();
		b = pop();
		push(b - a);
		break;
	case Instruction::OT_MUL:
		a = pop();
		b = pop();
		push(a*b);
		break;
	case Instruction::OT_DIV:
		a = pop();
		b = pop();
		push(b / a);
		break;
	case Instruction::OT_ODD:
		push(pop() % 2);
		break;
	case Instruction::OT_MOD:
		a = pop();
		b = pop();
		push(b % a);
		break;
	case Instruction::OT_EQL:
		a = pop();
		b = pop();
		push(a == b);
		break;
	case Instruction::OT_NEQ:
		a = pop();
		b = pop();
		push(a != b);
		break;
	case Instruction::OT_LSS:
		a = pop();
		b = pop();
		push(b < a);
		break;
	case Instruction::OT_LEQ:
		a = pop();
		b = pop();
		push(b <= a);
		break;
	case Instruction::OT_GTR:
		a = pop();
		b = pop();
		push(b > a);
		break;
	case Instruction::OT_GEQ:
		a = pop();
		b = pop();
		push(b >= a);
		break;
	default:
		break;
	}
}

int Interpreter::base(int b_ptr, int level)
{
	for (int i = 0; i < level; i++) {
		b_ptr = data[b_ptr]; // static link is stored at base pointer
	}
	return b_ptr;
}

void Interpreter::push(int x)
{
	sp += 1;
	data[sp] = x;
}

int Interpreter::pop()
{
	int x = data[sp];
	sp -= 1;
	return x;
}

bool Interpreter::fetch()
{
	if (pc >= (int)instructions.size()) {
		return false;
	}
	ir = instructions[pc];
	pc++;
	return true;
}

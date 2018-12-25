#include "pch.h"
#include "Error.h"
#include <iostream>
#include <exception>
#include "Word.h"

Error::Error()
{
}


Error::~Error()
{
}

void Error::raise(int line, ErrorType errorType)
{
	std::cout << "\033[1;31m" << "Error " << errorType << " happened in line : " << line << "\033[0m" << std::endl;
	std::cerr << "\t\033[1;31m";
	switch (errorType)
	{
	case Error::USE_EQUAL_INSTEAD_OF_ASSIGN:
		std::cerr << "use = instead of :=" << "\033[0m" << std::endl;
		break;
	case Error::EQUAL_MUST_BE_FOLLOWED_BY_NUMBER:
		std::cerr << "equal must be followed by number" << "\033[0m" << std::endl;
		break;
	case Error::IDENTIFIER_MUST_BE_FOLLOWED_BY_NUMBER:
		std::cerr << "identifier must be followed by number" << "\033[0m" << std::endl;
		break;
	case Error::SHOULD_BE_FOLLOWED_BY_IDENTIFIRE:
		std::cerr << "should be followed by identifier" << "\033[0m" << std::endl;
		break;
	case Error::MISSING_COMMA_OR_SEMICOLON:
		std::cerr << "missing , or ;" << "\033[0m" << std::endl;
		break;
	case Error::INCORRECT_SYMBOL_AFTER_PROC_DECLARATION:
		std::cerr << "Incorrect symbol after procedure declaration" << "\033[0m" << std::endl;
		break;
	case Error::EXPECT_STATEMENT:
		std::cerr << "expect statement" << "\033[0m" << std::endl;
		break;
	case Error::INCORECT_SYMBOL_AFTER_STATEMENT_IN_BLOCK:
		std::cerr << "incorrect symbol after statement in block" << "\033[0m" << std::endl;
		break;
	case Error::EXPECT_DOT_AT_END:
		std::cerr << "expect dot at end" << "\033[0m" << std::endl;
		break;
	case Error::MISSING_SEMICOLON:
		std::cerr << "missing ;" << "\033[0m" << std::endl;
		break;
	case Error::UNDECLARED_IDENTIFIER:
		//std::cerr << "undeclared identifier" << "\033[0m" << std::endl;
		std::cerr << "\033[0m";
		break;
	case Error::ASSIGNED_TO_CONST_OR_PROC:
		std::cerr << "can't assign to const or procedure" << "\033[0m" << std::endl;
		break;
	case Error::USE_ASSIGN_INSTEAD_OF_EQUAL:
		std::cerr << "use := insted of =" << "\033[0m" << std::endl;
		break;
	case Error::EXPECT_IDENTIFIER_AFTER_CALL:
		std::cerr << "expect identifier after call" << "\033[0m" << std::endl;
		break;
	case Error::CANNOT_CALL_VAR_OR_CONST:
		std::cerr << "cannot call var or const" << "\033[0m" << std::endl;
		break;
	case Error::EXPECT_THEN:
		std::cerr << "expect then" << "\033[0m" << std::endl;
		break;
	case Error::EXPECT_SEMICOLON_OR_END:
		std::cerr << "expect ; or end" << "\033[0m" << std::endl;
		break;
	case Error::EXPECT_DO:
		std::cerr << "expect do" << "\033[0m" << std::endl;
		break;
	case Error::INCORRECT_SYMBOL_AFTER_STATEMENT:
		std::cerr << "incorrect symbol after statement" << "\033[0m" << std::endl;
		break;
	case Error::EXPECT_RATIONAL_OPERATOR:
		std::cerr << "expect rational operator" << "\033[0m" << std::endl;
		break;
	case Error::EXPRESSION_CANNOT_CONTAIN_PROC:
		std::cerr << "expressio cannot contain procedure" << "\033[0m" << std::endl;
		break;
	case Error::MISSING_RIGHT_PARENTHESIS:
		std::cerr << "missing right parenthesis" << "\033[0m" << std::endl;
		break;
	case Error::INCORRECT_SYMBOL_AFTER_FACTOR:
		std::cerr << "incorrect symbol after factor" << "\033[0m" << std::endl;
		break;
	case Error::INCORECT_SYMBOL_LEADS_EXPRESSION:
		std::cerr << "incorrect symbol leads expression" << "\033[0m" << std::endl;
		break;
	case Error::EXCEED_MAX_LEVEL:
		std::cerr << "exceed max level" << "\033[0m" << std::endl;
		break;
	case Error::TOO_BIG_NUMBER:
		std::cerr << "number is too big" << "\033[0m" << std::endl;
		break;
	case Error::EXPECT_LEFT_PARENTHSIS:
		std::cerr << "expect left parenthsis" << "\033[0m" << std::endl;
		break;
	case Error::EXPECT_ASSIGN:
		std::cerr << "expect :=" << "\033[0m" << std::endl;
		break;
	case Error::EXPECT:
		break;
	case Error::UNEXPECTED:
		std::cerr << "unexpected symbol" << "\033[0m" << std::endl;
		break;
	default:
		break;
	}
}
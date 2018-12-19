#pragma once
#include <string>
class Error
{
public:
	enum ErrorType {
		USE_EQUAL_INSTEAD_OF_ASSIGN = 1,
		EQUAL_MUST_BE_FOLLOWED_BY_NUMBER,
		IDENTIFIER_MUST_BE_FOLLOWED_BY_NUMBER,
		SHOULD_BE_FOLLOWED_BY_IDENTIFIRE,
		MISSING_COMMA_OR_SEMICOLON,
		INCORRECT_SYMBOL_AFTER_PROC_DECLARATION,
		EXPECT_STATEMENT,
		INCORECT_SYMBOL_AFTER_STATEMENT_IN_BLOCK,
		EXPECT_DOT_AT_END,
		MISSING_SEMICOLON,
		UNDECLARED_IDENTIFIER, //11
		ASSIGNED_TO_CONST_OR_PROC,
		USE_ASSIGN_INSTEAD_OF_EQUAL,
		EXPECT_IDENTIFIER_AFTER_CALL,
		CANNOT_CALL_VAR_OR_CONST,
		EXPECT_THEN,
		EXPECT_SEMICOLON_OR_END,
		EXPECT_DO,
		INCORRECT_SYMBOL_AFTER_STATEMENT,
		EXPECT_RATIONAL_OPERATOR,
		EXPRESSION_CANNOT_CONTAIN_PROC,
		MISSING_RIGHT_PARENTHESIS,
		INCORRECT_SYMBOL_AFTER_FACTOR,
		INCORECT_SYMBOL_LEADS_EXPRESSION,
		EXCEED_MAX_LEVEL,
		TOO_BIG_NUMBER,
		EXPECT_LEFT_PARENTHSIS,
		EXPECT_ASSIGN,
		MISSING,
		UNEXPECTED,
	};

	Error();
	~Error();
	static void my_error_raise(int line, ErrorType errorType);
	static void raiseError(int line, ErrorType errorType, std::string errorInfo);
	static void raiseMissingError(int line, std::string missingStr);
	static void raiseUnexpectedError(int line, std::string unexpectedInfo);
	static void raiseSimpleError(int line, std::string info);
};


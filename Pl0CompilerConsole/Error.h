#pragma once
#include <string>
class Error
{
public:
	enum ErrorType {
		MISSING,
		UNEXPECTED,
		EXCEED_MAX_LEVEL,
		SHOULD_USE_EQUAL,
		SHOULD_IDENTIFIRE
	};

	Error();
	~Error();
	static void raiseError(int line, ErrorType errorType);
	static void raiseError(int line, ErrorType errorType, std::string errorInfo);
	static void raiseMissingError(int line, std::string missingStr);
	static void raiseUnexpectedError(int line, std::string unexpectedInfo);
	static void raiseSimpleError(int line, std::string info);
};


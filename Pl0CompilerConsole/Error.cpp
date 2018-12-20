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

void Error::my_error_raise(int line, ErrorType errorType)
{
	std::cout << "Error " << errorType << " happened in line : " << line << std::endl;
}

void Error::raiseError(int line, ErrorType errorType, std::string errorInfo)
{
	std::cout << "Error " << errorType << " happened in line : " << line << std::endl;
	std::cout << "Error info " << errorInfo << std::endl;
 }

void Error::raiseMissingError(int line, std::string missingStr)
{
	std::cerr << "Missing " << missingStr << " in line " << line+1 << std::endl;
	throw std::exception("missing");
}

void Error::raiseUnexpectedError(int line, std::string unexpectedInfo)
{
	std::cerr << "Unexpected " << unexpectedInfo << " in line " << line << std::endl;
	throw std::exception("Unexpected");
}

void Error::raiseSimpleError(int line, std::string info)
{
	std::cerr << info << " in line " << line << std::endl;
	throw std::exception(info.c_str());
}

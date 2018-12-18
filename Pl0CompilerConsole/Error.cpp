#include "pch.h"
#include "Error.h"
#include <iostream>
#include <exception>

Error::Error()
{
}


Error::~Error()
{
}

void Error::raiseError(int line, ErrorType errorType)
{
	switch (errorType)
	{
	case Error::MISSING:
		break;
	case Error::UNEXPECTED:
		break;
	case Error::EXCEED_MAX_LEVEL:
		break;
	default:
		break;
	}
}

void Error::raiseError(int line, ErrorType errorType, std::string errorInfo)
{
	//throw std::exception(errorInfo);
}

void Error::raiseMissingError(int line, std::string missingStr)
{
	std::cerr << "Missing " << missingStr << " in line " << line << std::endl;
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

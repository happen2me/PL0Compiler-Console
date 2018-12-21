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
}
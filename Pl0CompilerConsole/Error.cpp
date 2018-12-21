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
	std::cout << "Error " << errorType << " happened in line : " << line << std::endl;
}
// Pl0CompilerConsole.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "WordAnalyzer.h"
#include "GrammarAnalyzer.h"
#include "Interpreter.h"
using namespace std;

inline bool exists_test(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

int main()
{
	cout << "Running" << endl;
	cout << "input file name (refer to code_sapmples directory)" << endl;
	string path;
	cin >> path;
	path = "code_samples/" + path;
	while (!exists_test(path))
	{
		cout << path << " not exits";
		cout << "input file name (refer to code_sapmples directory)" << endl;
		path = "code_samples/" + path;
	}

	WordAnalyzer wordAnalyzer(path);
	try {
		wordAnalyzer.analyze();
	}
	catch (std::exception e) {
		cout << "Word analysis failed" << endl;
		return 0;
	}

	if (wordAnalyzer.getErrorCount() != 0) {
		cout << wordAnalyzer.getErrorCount() << " errors detected in word analysis" << endl;
	}
	else{
		GrammarAnalyzer grammarAnalyzer(wordAnalyzer.getResult());
		try {
			grammarAnalyzer.runCompile();
		}
		catch (std::exception e) {
			std::cout << e.what() << std::endl;
		}
		try {
			//wordAnalyzer.printResult();			
			ofstream myfile;
			myfile.open("output/pcodes.txt");
			grammarAnalyzer.printPcodes(myfile);
			myfile.close();
			grammarAnalyzer.printSymbolTable(std::cout);
			grammarAnalyzer.printPcodes(std::cout);
			std::cout << grammarAnalyzer.getErrorCount() << " errors detected" << std::endl;
			if (!grammarAnalyzer.errorHappened()) {
				Interpreter interpreter(grammarAnalyzer.getResults());
				interpreter.run();
			}
		}
		catch (const std::exception e) {
			std::cerr << e.what() << std::endl;
		}
	}


	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

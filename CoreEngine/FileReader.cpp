#include "FileReader.h"
#include <fstream>
string
ReadFile(CORE_STRING pStrFilePath)
{
	std::ifstream ifs(pStrFilePath);
	std::string content((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));

	return content;
}

string
ReadFile(string & filePath)
{
	std::ifstream ifs(filePath);
	std::string content((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));

	return content;
}
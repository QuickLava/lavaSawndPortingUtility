#ifndef LAVA_MONTY_PYTHON_REIMP_UTILITY_H_V1
#define LAVA_MONTY_PYTHON_REIMP_UTILITY_H_V1

#include <conio.h>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iomanip>
#include <unordered_map>
#include <map>

namespace lava
{
	std::string stringToUpper(const std::string& stringIn);
	std::size_t hexVecToNum(const std::vector<char>& vecIn);
	std::vector<char> numToHexVec(std::size_t vecIn);
	std::size_t hexStringToNum(const std::string& stringIn);
	std::string numToHexStringWithPadding(std::size_t numIn, std::size_t paddingLength);
	std::string numToDecStringWithPadding(std::size_t numIn, std::size_t paddingLength);
	std::string hexVecToHexStringWithPadding(const std::vector<char>& vecIn, std::size_t paddingLength);
	std::string hexVecToDecStringWithPadding(const std::vector<char>& vecIn, std::size_t paddingLength);

	void addVectors(std::vector<std::size_t>& vec1, const std::vector<std::size_t>& vec2);
}


#endif

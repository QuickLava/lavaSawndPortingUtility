#ifndef LAVA_SAWND_PORTING_UTILITY_UTILITY_H_V1
#define LAVA_SAWND_PORTING_UTILITY_UTILITY_H_V1

#include <conio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <unordered_map>
#include <array>
#include <map>

namespace lava
{
	std::string stringToUpper(const std::string& stringIn);
	std::string numToHexStringWithPadding(std::size_t numIn, std::size_t paddingLength);
	std::string numToDecStringWithPadding(std::size_t numIn, std::size_t paddingLength);
}


#endif

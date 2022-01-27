#include "lavaMPRUtility.h"

namespace lava
{
	std::string stringToUpper(const std::string& stringIn)
	{
		std::string result = stringIn;
		for (std::size_t i = 0; i < result.size(); i++)
		{
			result[i] = toupper(result[i]);
		}
		return result;
	}
	std::size_t hexVecToNum(const std::vector<char>& vecIn)
	{
		std::size_t result = SIZE_MAX;
		std::size_t vecLength = vecIn.size();
		if (vecLength <= 4)
		{
			int temp = 0x0;
			result = 0;
			for (std::size_t i = 0; i < vecLength; i++)
			{
				result = result << 8;
				temp = vecIn[i];
				temp &= 0x000000FF;
				result |= temp;
			}
		}
		else
		{
			std::cerr << "Provided vector unsupported. Max length is 4 bytes, provided was " << vecLength << ".\n";
		}
		return result;
	}
	std::vector<char> numToHexVec(std::size_t vecIn)
	{
		std::vector<char> result(4, 0);

		union
		{
			std::size_t n;
			char arr[4];
		} s;
		s.n = vecIn;
		result[0] = s.arr[3];
		result[1] = s.arr[2];
		result[2] = s.arr[1];
		result[3] = s.arr[0];
		return result;
	}
	std::size_t hexStringToNum(const std::string& stringIn)
	{
		std::size_t result = 0;
		result = std::stoul("0x" + stringIn, nullptr, 16);
		return result;
	}
	std::string numToHexStringWithPadding(std::size_t numIn, std::size_t paddingLength)
	{
		std::stringstream convBuff;
		convBuff << std::hex << numIn;
		std::string result = convBuff.str();
		for (int i = 0; i < result.size(); i++)
		{
			result[i] = std::toupper(result[i]);
		}
		if (result.size() < paddingLength)
		{
			result = std::string(paddingLength - result.size(), '0') + result;
		}
		return result;
	}
	std::string numToDecStringWithPadding(std::size_t numIn, std::size_t paddingLength)
	{
		std::string result = std::to_string(numIn);
		if (result.size() < paddingLength)
		{
			result = std::string(paddingLength - result.size(), '0') + result;
		}
		return result;
	}
	std::string hexVecToHexStringWithPadding(const std::vector<char>& vecIn, std::size_t paddingLength)
	{
		return (numToHexStringWithPadding(hexVecToNum(vecIn), paddingLength));
	}
	std::string hexVecToDecStringWithPadding(const std::vector<char>& vecIn, std::size_t paddingLength)
	{
		return (numToDecStringWithPadding(hexVecToNum(vecIn), paddingLength));
	}
	
	void addVectors(std::vector<std::size_t>& vec1, const std::vector<std::size_t>& vec2)
	{
		std::size_t minSize = std::min(vec1.size(), vec2.size());
		for (std::size_t i = 0; i < minSize; i++)
		{
			vec1[i] += vec2[i];
		}
	}
}
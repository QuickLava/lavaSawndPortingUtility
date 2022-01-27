#include "lavaSawndz.h"

namespace lava
{
	// Returns 1 if the specified err message was cut off
	bool printError(int errnoIn)
	{
		std::vector<char> errorMessageBuffer{};
		// I'm currently hardcoding the err buffer to 32, because Visual Studio doesn't have an implementation of strerrorlen_s to properly check the length of the message.
		errorMessageBuffer.resize(32);
		// Get the message and store it in errorMessageBuffer, recording whether the message got cut off
		bool fullMessageGotten = strerror_s(errorMessageBuffer.data(), 32, errno);
		std::cerr << std::string(errorMessageBuffer.data());
		return fullMessageGotten;
	}

	bool writeIntToOutput(std::ofstream& out, std::size_t val)
	{
		out.write(lava::numToHexVec(val).data(), 4);
		return out.good();
	}

	// Byte Array Stuff
	void byteArray::populate(std::istream& sourceStream)
	{
		std::string buffer = "";
		sourceStream.seekg(0, sourceStream.end);
		std::size_t sourceSize(sourceStream.tellg());
		sourceStream.seekg(0, sourceStream.beg);
		body.resize(sourceSize);
		sourceStream.read(body.data(), sourceSize);
		size = body.size();
		std::cout << "Loaded " << size << " byte(s) of data.\n\n";
	}
	std::vector<char> byteArray::getBytes(std::size_t numToGet, std::size_t startIndex, std::size_t& numGotten)
	{
		numGotten = 0;
		if (startIndex < size)
		{
			if (startIndex + numToGet > size)
			{
				numToGet = size - startIndex;
				return std::vector<char>(body.begin() + startIndex, body.end());
			}
			numGotten = numToGet;
			return std::vector<char>(body.begin() + startIndex, body.begin() + startIndex + numToGet);
		}
		else
		{
			std::cerr << "\nRequested region startpoint was invalid. Specified index was [" << startIndex << "], max valid index is [" << size - 1 << "].\n";
		}
		return std::vector<char>();
	}
	bool byteArray::setBytes(std::vector<char> bytesIn, std::size_t atIndex)
	{
		bool result = 0;
		if ((atIndex + bytesIn.size()) > atIndex && atIndex + bytesIn.size() < size)
		{
			/*int tempInt = 0;
			char* tempPtr = body.data() + atIndex;
			std::cout << "Original Value: " << std::hex;
			for (int i = 0; i < bytesIn.size(); i++)
			{
				tempInt = *(tempPtr + i);
				tempInt &= 0x000000FF;
				std::cout << ((tempInt < 0x10) ? "0" : "") << tempInt;
			}
			std::cout << "\n" << std::dec;*/
			std::memcpy(body.data() + atIndex, bytesIn.data(), bytesIn.size());
			/*tempInt = 0;
			tempPtr = body.data() + atIndex;
			std::cout << "Modified Value: " << std::hex;
			for (int i = 0; i < bytesIn.size(); i++)
			{
				tempInt = *(tempPtr + i);
				tempInt &= 0x000000FF;
				std::cout << ((tempInt < 0x10) ? "0" : "") << tempInt;
			}
			std::cout << "\n" << std::dec;*/
			result = 1;
		}
		return result;
	}
	bool byteArray::dumpToFile(std::string targetPath)
	{
		bool result = 0;
		std::ofstream output;
		output.open(targetPath, std::ios_base::binary | std::ios_base::out);
		if (output.is_open())
		{
			output.write(body.data(), size);
			result = 1;
			std::cout << "Dumped body to \"" << targetPath << "\".\n";
		}
		return result;
	}
	std::size_t byteArray::search(const std::vector<char>& searchCriteria, std::size_t startItr, std::size_t endItr)
	{
		std::vector<char>::iterator itr = body.end();
		if (endItr < startItr)
		{
			endItr = SIZE_MAX;
		}
		if (endItr > size)
		{
			endItr = size;
		}
		if (size && startItr < size && searchCriteria.size())
		{
			itr = std::search(body.begin() + startItr, body.begin() + endItr, searchCriteria.begin(), searchCriteria.end());
		}
		return (itr != body.end()) ? itr - body.begin() : SIZE_MAX;
	}
	std::vector<std::size_t> byteArray::searchMultiple(const std::vector<char>& searchCriteria, std::size_t startItr, std::size_t endItr)
	{
		std::size_t cursor = startItr;
		std::vector<std::size_t> result;
		std::size_t critSize = searchCriteria.size();
		bool done = 0;
		while (!done && cursor <= endItr)
		{
			cursor = search(searchCriteria, cursor);
			if (cursor != SIZE_MAX)
			{
				result.push_back(cursor);
				if ((cursor + critSize) > cursor)
				{
					cursor += critSize;
				}
			}
			else
			{
				done = 1;
			}
		}
		return result;
	}

	namespace sawndz
	{
		bool brsarFile::init(std::string filePathIn)
		{
			bool result = 0;
			std::ifstream fileIn;
			fileIn.open(filePathIn, std::ios_base::in |std::ios_base::binary);
			if (fileIn.is_open())
			{
				result = 1;
				contents.populate(fileIn);
				std::size_t tempNumber = 0;
				std::size_t numGotten = 0;
				info_address = lava::hexVecToNum(contents.getBytes(4, 0x18, numGotten));
				relocation_address = info_address + 0x2C;
				tempNumber = lava::hexVecToNum(contents.getBytes(4, relocation_address, numGotten));
				group_num = lava::hexVecToNum(contents.getBytes(4, info_address + 0x8 + tempNumber, numGotten));
				grprel_baseoff = info_address + 0x10 + tempNumber;
			}
			return result;
		}
		std::size_t brsarFile::readIntFromContents(std::size_t offsetIn, std::size_t& numGotten)
		{
			return lava::hexVecToNum(contents.getBytes(4, offsetIn, numGotten));
		}

		std::size_t brsarFile::getGroupOffset(std::size_t groupIDIn, std::size_t* grakOut)
		{
			std::size_t result = SIZE_MAX;

			std::size_t groupID = SIZE_MAX;
			std::size_t tempAddress = SIZE_MAX;
			std::size_t numGotten = 0;
			bool found = 0;
			std::size_t i = 0;
			std::size_t buff = 0;;
			while (!found && i < group_num)
			{
				buff = grprel_baseoff + (i * 8);
				tempAddress = readIntFromContents(buff, numGotten);
				groupID = readIntFromContents(info_address + 8 + tempAddress, numGotten);
				if (groupID == groupIDIn)
				{
					if (grakOut != nullptr)
					{
						*grakOut = i;
					}
					found = 1;
					result = info_address + 8 + tempAddress;
					std::cout << "Found Group #" << groupID << " @ 0x" << lava::numToHexStringWithPadding(result, 8) << "\n";
				}
				i++;
			}
			return result;
		}

		bool brsarFile::exportSawnd(std::size_t groupID, std::string targetFilePath)
		{
			bool result = 0;
			std::cout << "Creating \"" << targetFilePath << "\" from Group #" << groupID << "...\n";
			std::size_t group_offset = SIZE_MAX;
			std::size_t base_rwsd = SIZE_MAX;
			std::size_t size = SIZE_MAX;
			std::size_t col_rel = SIZE_MAX;
			std::size_t col_num = SIZE_MAX;
			std::size_t col_val = SIZE_MAX;
			std::size_t total_size = SIZE_MAX;
			std::size_t samples = SIZE_MAX;
			std::size_t address = SIZE_MAX;
			std::size_t numGotten = SIZE_MAX;

			std::ofstream Copy;
			Copy.open(targetFilePath, std::ios_base::out | std::ios_base::binary);
			if (Copy.is_open())
			{
				group_offset = getGroupOffset(groupID);
				if (group_offset != SIZE_MAX)
				{
					base_rwsd = readIntFromContents(group_offset + 0x10, numGotten);
					total_size = readIntFromContents(group_offset + 0x14, numGotten);
					samples = readIntFromContents(group_offset + 0x1C, numGotten);
					total_size += samples;
					std::cout << "Samples(0x" << lava::numToHexStringWithPadding(samples, 8) << ")\n";
					address = readIntFromContents(group_offset + 0x24, numGotten);
					std::cout << "Address(0x" << lava::numToHexStringWithPadding(address, 8) << ")\n";

					col_rel = info_address + 0x8 + address;
					col_num = readIntFromContents(col_rel, numGotten);

					Copy.put(2);
					lava::writeIntToOutput(Copy, groupID);
					lava::writeIntToOutput(Copy, samples);

					std::size_t bufferNum = INT_MAX;
					for (std::size_t i = 0; i < col_num; i++)
					{
						bufferNum = col_rel + 8 + (i * 8);
						std::cout << "Collection #" << i << " (0x" << lava::numToHexStringWithPadding(bufferNum, 8) << "), Address_Val(0x" << lava::numToHexStringWithPadding(address, 8) << ")\n";
						address = readIntFromContents(bufferNum, numGotten);
						bufferNum = info_address + 8 + address;
						col_val = readIntFromContents(bufferNum, numGotten);
						std::cout << "\tVal[0]: Buffer_Val(0x" <<
							lava::numToHexStringWithPadding(bufferNum, 8) << "), Col_Val(0x" <<
							lava::numToHexStringWithPadding(col_val, 8) << "), Address_Val(0x" <<
							lava::numToHexStringWithPadding(address, 8) << ")\n";
						lava::writeIntToOutput(Copy, col_val);
						bufferNum = info_address + 8 + address + 0xC;
						col_val = readIntFromContents(bufferNum, numGotten);
						std::cout << "\tVal[1]: Buffer_Val(0x" <<
							lava::numToHexStringWithPadding(bufferNum, 8) << "), Samples_Val(0x" <<
							lava::numToHexStringWithPadding(samples, 8) << ")\n";
						lava::writeIntToOutput(Copy, col_val);
						bufferNum = info_address + 8 + address + 0x10;
						col_val = readIntFromContents(bufferNum, numGotten);
						std::cout << "\tVal[2]: Buffer_Val(0x" <<
							lava::numToHexStringWithPadding(bufferNum, 8) << "), Samples_Val(0x" <<
							lava::numToHexStringWithPadding(samples, 8) << ")\n";
						lava::writeIntToOutput(Copy, col_val);
					}
					std::cout << "Total Size:" << total_size << "\n";
					Copy.write(contents.body.data() + base_rwsd, total_size);
				}
				else
				{
					std::cerr << "Provided group ID couldn't be located. Aborting export.\n";
					printf("ERROR: The group is incorrect.\n");
					remove(targetFilePath.c_str());
				}
			}
			else
			{
				std::cerr << "Error creating .sawnd: ";
				printError(errno);
				std::cerr << "\n";
			}
			return result;
		}

	}
}
#ifndef LAVA_BYTE_ARRAY_H_V1
#define LAVA_BYTE_ARRAY_H_V1

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>


namespace lava
{
	struct byteArray
	{
	private:
		bool _populated = 0;

		template<typename objectType>
		std::vector<unsigned char> typeToBytes(objectType& objectIn, bool reverse) const
		{
			std::vector<unsigned char> result = std::vector<unsigned char>((unsigned char*)(&objectIn), (unsigned char*)(&objectIn) + sizeof(objectType));
			if (reverse)
			{
				std::reverse(result.begin(), result.end());
			}
			return result;
		}

		template<typename objectType>
		objectType bytesToType(std::vector<unsigned char> bytesIn, bool reverse) const
		{
			objectType result = ULONG_MAX;
			if (reverse)
			{
				std::reverse(bytesIn.begin(), bytesIn.end());
			}
			result = *((objectType*)bytesIn.data());
			return result;
		}

	public:
		std::vector<char> body = {};
	private:
		template<typename objectType>
		objectType getObj(std::size_t startIndex) const
		{
			objectType result = SIZE_MAX;
			std::size_t numGotten = 0;
			std::vector<unsigned char> bytes = getBytes(sizeof(objectType), startIndex, numGotten);
			if (numGotten == sizeof(objectType))
			{
				result = bytesToType<objectType>(bytes, 1);
			}
			return result;
		}
		template<typename objectType>
		bool setObj(const objectType& objectIn, std::size_t startIndex)
		{
			bool result = 0;
			if (startIndex + sizeof(objectType) < body.size())
			{
				result = setBytes(typeToBytes(objectIn, 1), startIndex);
			}
			return result;
		}
		template<typename objectType>
		bool insertObj(const objectType& objectIn, std::size_t startIndex)
		{
			bool result = 0;
			if (startIndex < body.size())
			{
				result = insertBytes(typeToBytes(objectIn, 1), startIndex);
			}
			return result;
		}

		template<typename objectType>
		std::size_t findObj(const objectType& objectIn, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const
		{
			std::size_t result = 0;
			if (startItr < body.size())
			{
				result = search(typeToBytes(objectIn, 1), startItr, endItr);
			}
			return result;
		}

		template<typename objectType>
		std::vector<std::size_t> findObjMultiple(const objectType& objectIn, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const
		{
			std::vector<std::size_t> result{};
			if (startItr < body.size())
			{
				result = searchMultiple(typeToBytes(objectIn, 1), startItr, endItr);
			}
			return result;
		}

	public:
		void populate(std::istream& sourceStream);
		bool populated() const;

		std::vector<unsigned char> getBytes(std::size_t numToGet, std::size_t startIndex, std::size_t& numGot) const;
		unsigned long long int getLLong(std::size_t startIndex) const;
		unsigned long int getLong(std::size_t startIndex) const;
		unsigned short int getShort(std::size_t startIndex) const;
		unsigned char getChar(std::size_t startIndex) const;
		double getDouble(std::size_t startIndex) const;
		float getFloat(std::size_t startIndex) const;

		bool setBytes(std::vector<unsigned char> bytesIn, std::size_t atIndex);
		bool setLLong(unsigned long long int valueIn, std::size_t atIndex);
		bool setLong(unsigned long int valueIn, std::size_t atIndex);
		bool setShort(unsigned short int valueIn, std::size_t atIndex);
		bool setChar(unsigned char valueIn, std::size_t atIndex);
		bool setDouble(double valueIn, std::size_t atIndex);
		bool setFloat(float valueIn, std::size_t atIndex);

		bool insertBytes(std::vector<unsigned char> bytesIn, std::size_t atIndex);
		bool insertLLong(unsigned long long int valueIn, std::size_t atIndex);
		bool insertLong(unsigned long int valueIn, std::size_t atIndex);
		bool insertShort(unsigned short int valueIn, std::size_t atIndex);
		bool insertChar(unsigned char valueIn, std::size_t atIndex);
		bool insertDouble(double valueIn, std::size_t atIndex);
		bool insertFloat(float valueIn, std::size_t atIndex);

		std::size_t search(const std::vector<unsigned char>& searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;
		std::size_t searchLLong(unsigned long long int searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;
		std::size_t searchLong(unsigned long int searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;
		std::size_t searchShort(unsigned short int searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;
		std::size_t searchChar(unsigned char searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;
		std::size_t searchDouble(double searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;
		std::size_t searchFloat(float searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;

		std::vector<std::size_t> searchMultiple(const std::vector<unsigned char>& searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;
		std::vector<std::size_t> searchMultipleLLong(unsigned long long int searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;
		std::vector<std::size_t> searchMultipleLong(unsigned long int searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;
		std::vector<std::size_t> searchMultipleShort(unsigned short int searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;
		std::vector<std::size_t> searchMultipleChar(unsigned char searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;
		std::vector<std::size_t> searchMultipleDouble(double searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;
		std::vector<std::size_t> searchMultipleFloat(float searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX) const;

		bool dumpToFile(std::string targetPath) const;
	};
}

#endif
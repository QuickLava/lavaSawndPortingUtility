#ifndef LAVA_SAWNDZ_REIMP_H_V1
#define LAVA_SAWNDZ_REIMP_H_V2

#include "lavaMPRUtility.h"

namespace lava
{
	bool printError(int errnoIn);
	bool writeIntToOutput(std::ofstream& out, std::size_t val);

	struct byteArray
	{
		std::size_t size = 0;
		std::vector<char> body = {};
		void populate(std::istream& sourceStream);
		std::vector<char> getBytes(std::size_t numToGet, std::size_t startIndex, std::size_t& numGot);
		bool setBytes(std::vector<char> bytesIn, std::size_t atIndex);
		std::size_t search(const std::vector<char>& searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX);
		std::vector<std::size_t> searchMultiple(const std::vector<char>& searchCriteria, std::size_t startItr = 0, std::size_t endItr = SIZE_MAX);
		bool dumpToFile(std::string targetPath);
	};

	namespace sawndz
	{
		struct brsarFile
		{
			std::size_t info_address = SIZE_MAX;
			std::size_t relocation_address = SIZE_MAX;
			std::size_t group_num = SIZE_MAX;
			std::size_t grprel_baseoff = SIZE_MAX;
			std::size_t grid = SIZE_MAX;
			std::size_t base_rwsd = SIZE_MAX;
			std::size_t size = SIZE_MAX;
			
			lava::byteArray contents;
			bool init(std::string filePathIn);
			std::size_t readIntFromContents(std::size_t offsetIn, std::size_t& numGotten);
			std::size_t getGroupOffset(std::size_t groupIDIn, std::size_t* grakOut = nullptr);
			bool exportSawnd(std::size_t groupID, std::string targetFilePath = "sawnd.sawnd");
		};
	}
}


#endif
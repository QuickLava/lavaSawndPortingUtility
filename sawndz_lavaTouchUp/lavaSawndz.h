#ifndef LAVA_SAWNDZ_REIMP_H_V1
#define LAVA_SAWNDZ_REIMP_H_V2

#include "lavaByteArray.h"
#include "lavaMPRUtility.h"

namespace lava
{
	bool printError(int errnoIn);

	template<typename objectType>
	bool writeRawDataToStream(std::ostream& out, const objectType& objectIn)
	{
		std::vector<char> temp((char*)(&objectIn), (char*)(&objectIn) + sizeof(objectType));
		std::reverse(temp.begin(), temp.end());
		out.write(temp.data(), sizeof(objectType));
		return out.good();
	}

	namespace sawndz
	{
		struct brawlReference
		{
			bool isOffset = 1;
			unsigned long address = ULONG_MAX;

			brawlReference(unsigned long long valueIn = ULONG_MAX);
			unsigned long getAddress(unsigned long relativeToIn = ULONG_MAX);
			std::string getAddressString(unsigned long relativeToIn = ULONG_MAX);
			unsigned long long getHex();
		};
		struct brawlReferenceVector
		{
			std::vector<brawlReference> refs{};

			bool populate(lava::byteArray& bodyIn, std::size_t addressIn = SIZE_MAX);
			std::vector<unsigned long> getHex();
			bool exportContents(std::ostream& destinationStream);
		};

		struct dataInfo
		{
			unsigned long address = ULONG_MAX;

			// References
			brawlReference wsdInfo;
			brawlReference trackTable;
			brawlReference noteTable;

			// "WSD" Data
			float wsdPitch = FLT_MAX;
			unsigned char wsdPan = UCHAR_MAX;
			unsigned char wsdSurroundPan = UCHAR_MAX;
			unsigned char wsdFxSendA = UCHAR_MAX;
			unsigned char wsdFxSendB = UCHAR_MAX;
			unsigned char wsdFxSendC = UCHAR_MAX;
			unsigned char wsdMainSend = UCHAR_MAX;
			unsigned char wsdPad1 = UCHAR_MAX;
			unsigned char wsdPad2 = UCHAR_MAX;
			brawlReference wsdGraphEnvTableRef = ULLONG_MAX;
			brawlReference wsdRandomizerTableRef = ULLONG_MAX;
			unsigned long wsdPadding = ULONG_MAX;

			// "Track Table" Data
			brawlReferenceVector ttReferenceList1;
			brawlReference ttIntermediateReference = ULLONG_MAX;
			brawlReferenceVector ttReferenceList2;
			float ttPosition = FLT_MAX;
			float ttLength = FLT_MAX;
			unsigned long ttNoteIndex = ULONG_MAX;
			unsigned long ttReserved = ULONG_MAX;

			// "Note Table" Data
			brawlReferenceVector ntReferenceList;
			unsigned long ntWaveIndex = ULONG_MAX;
			unsigned char ntAttack = UCHAR_MAX;
			unsigned char ntDecay = UCHAR_MAX;
			unsigned char ntSustain = UCHAR_MAX;
			unsigned char ntRelease = UCHAR_MAX;
			unsigned char ntHold = UCHAR_MAX;
			unsigned char ntPad1 = UCHAR_MAX;
			unsigned char ntPad2 = UCHAR_MAX;
			unsigned char ntPad3 = UCHAR_MAX;
			unsigned char ntOriginalKey = UCHAR_MAX;
			unsigned char ntVolume = UCHAR_MAX;
			unsigned char ntPan = UCHAR_MAX;
			unsigned char ntSurroundPan = UCHAR_MAX;
			float ntPitch = FLT_MAX;
			brawlReference ntIfoTableRef;
			brawlReference ntGraphEnvTableRef;
			brawlReference ntRandomizerTableRef;
			unsigned long ntReserved = ULONG_MAX;

			bool populate(lava::byteArray& bodyIn, std::size_t addressIn);
			bool exportContents(std::ostream& destinationStream);
		};
		struct rwsdDataSection
		{
			unsigned long address = ULONG_MAX;

			unsigned long length = ULONG_MAX;
			
			brawlReferenceVector entryReferences;
			std::vector<dataInfo> entries{};

			bool populate(lava::byteArray& bodyIn, std::size_t address);
			bool exportContents(std::ostream& destinationStream);
		};

		struct waveInfo
		{
			unsigned long address = ULONG_MAX;

			unsigned char encoding = UCHAR_MAX;
			unsigned char looped = UCHAR_MAX;
			unsigned char channels = UCHAR_MAX;
			unsigned char sampleRate24 = UCHAR_MAX;
			unsigned short sampleRate = USHRT_MAX;
			unsigned char dataLocationType = UCHAR_MAX;
			unsigned char pad = UCHAR_MAX;
			unsigned long loopStartSample = ULONG_MAX;
			unsigned long nibbles = ULONG_MAX;
			unsigned long channelInfoTableOffset = ULONG_MAX;
			unsigned long dataLocation = ULONG_MAX;
			unsigned long reserved = ULONG_MAX;

			unsigned long channelInfoTableLength = LONG_MAX;
			std::vector<unsigned long> channelInfoTable{};

			bool populate(lava::byteArray& bodyIn, std::size_t address);
			bool exportContents(std::ostream& destinationStream);
		};
		struct rwsdWaveSection
		{
			unsigned long address = ULONG_MAX;

			unsigned long length = ULONG_MAX;
			unsigned long entryCount = ULONG_MAX;

			std::vector<unsigned long> entryOffsets{};
			std::vector<waveInfo> entries{};

			bool populate(lava::byteArray& bodyIn, std::size_t addressIn);
			bool exportContents(std::ostream& destinationStream);
		};

		struct rwsdHeader
		{
			unsigned long address = ULONG_MAX;

			unsigned short endianType = USHRT_MAX;
			unsigned short version = USHRT_MAX;
			unsigned long headerLength = ULONG_MAX;
			unsigned short entriesOffset = USHRT_MAX;
			unsigned short entriesCount = USHRT_MAX;

			unsigned long dataOffset = ULONG_MAX;
			unsigned long dataLength = ULONG_MAX;
			unsigned long waveOffset = ULONG_MAX;
			unsigned long waveLength = ULONG_MAX;

			bool populate(lava::byteArray& bodyIn, std::size_t addressIn);
			bool exportContents(std::ostream& destinationStream);
		};
		struct rwsd
		{
			unsigned long address = ULONG_MAX;

			rwsdHeader header;
			rwsdDataSection dataSection;
			rwsdWaveSection waveSection;

			bool populate(lava::byteArray& bodyIn, std::size_t addressIn);
			bool exportContents(std::ostream& destinationStream);
			bool exportContents(std::string destinationFile);
		};

		struct groupInfo
		{
			unsigned long address = ULONG_MAX;

			unsigned long fileID = ULONG_MAX;
			unsigned long headerOffset = ULONG_MAX;
			unsigned long headerLength = ULONG_MAX;
			unsigned long dataOffset = ULONG_MAX;
			unsigned long dataLength = ULONG_MAX;
			unsigned long reserved = ULONG_MAX;

			bool populate(lava::byteArray& bodyIn, std::size_t address);
			bool exportContents(std::ostream& destinationStream);
		};

		struct groupHeader
		{
			unsigned long address = ULONG_MAX;

			unsigned long stringID = ULONG_MAX;
			unsigned long entryNum = ULONG_MAX;
			brawlReference extFilePathRef = ULLONG_MAX;
			unsigned long headerOffset = ULONG_MAX;
			unsigned long headerLength = ULONG_MAX;
			unsigned long waveDataOffset = ULONG_MAX;
			unsigned long waveDataLength = ULONG_MAX;
			brawlReference listOffset = ULLONG_MAX;

			bool populate(lava::byteArray& bodyIn, std::size_t address);
			bool exportContents(std::ostream& destinationStream);
		};


		struct brsarFile
		{
			std::size_t info_address = SIZE_MAX;
			std::size_t relocation_address = SIZE_MAX;
			std::size_t group_num = SIZE_MAX;
			std::size_t grprel_baseoff = SIZE_MAX;
			std::size_t grid = SIZE_MAX;
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
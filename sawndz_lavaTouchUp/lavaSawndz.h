#ifndef LAVA_SAWNDZ_REIMP_H_V1
#define LAVA_SAWNDZ_REIMP_H_V1

#include "lavaByteArray.h"
#include "lavaSPUUtility.h"
#include "lavaBrawlConstants.h"

namespace lava
{
	template<typename objectType>
	bool writeRawDataToStream(std::ostream& out, const objectType& objectIn, bool doReverse = 1)
	{
		std::vector<char> temp((char*)(&objectIn), (char*)(&objectIn) + sizeof(objectType));
		if (doReverse)
		{
			std::reverse(temp.begin(), temp.end());
		}
		out.write(temp.data(), sizeof(objectType));
		return out.good();
	}

	unsigned long countCharInString(const std::string& stringIn, char searchIn);
	std::size_t findNthInString(const std::string& stringIn, char searchIn, unsigned long n);

	namespace brawl
	{
		namespace sawndz
		{
			const std::string version = "v0.85";
			const unsigned long _HEX_TAG_RWSD = 0x52575344;
			const unsigned long _NOT_IN_FILE_ADDRESS = 0xDEADBEEF;
			const unsigned long _OVERWRITE_SOUND_SHARED_WAVE_RETURN_CODE = 0xDEADDEED;

			enum class rsarFileSectionType
			{
				rFST_RWSD = 0,
				rFST_RBNK,
				rFST_RSEQ,
				rsarFileSectionTypeCount
			};

			struct groupFileInfo
			{
				unsigned long groupID = ULONG_MAX;
				std::vector<unsigned long> acceptableFileIDs{};

				groupFileInfo(unsigned long groupIDIn = ULONG_MAX, std::vector<unsigned long> acceptableFileIDsIn = {}) :groupID(groupIDIn), acceptableFileIDs(acceptableFileIDsIn) {};

				bool fileIDAcceptable(unsigned long fileIDIn);
			};
			extern std::unordered_map<unsigned long, groupFileInfo> fighterIDToGroupInfoMap;
			extern std::unordered_map<std::string, std::string> soundEffectNameOverrides;
			bool populateSoundEffectNameOverrides(std::string overrideFilePathIn);
			std::string shuckSoundEffectName(std::string nameIn, std::unordered_map<std::string, std::string>::iterator* overrideUsedOut = nullptr);

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


			struct brsarSymbMaskEntry
			{
				unsigned long address = ULONG_MAX;

				unsigned short flags = USHRT_MAX;
				unsigned short bit = USHRT_MAX;
				unsigned long leftID = ULONG_MAX;
				unsigned long rightID = ULONG_MAX;
				unsigned long stringID = ULONG_MAX;
				unsigned long index = ULONG_MAX;

				bool populate(lava::byteArray& bodyIn, unsigned long addressIn);
			};
			struct brsarSymbMaskHeader
			{
				unsigned long address = ULONG_MAX;

				unsigned long rootID = ULONG_MAX;
				unsigned long numEntries = ULONG_MAX;

				std::vector<brsarSymbMaskEntry> entries{};

				bool populate(lava::byteArray& bodyIn, unsigned long addressIn);
			};
			struct brsarSymbSection
			{
				unsigned long address = ULONG_MAX;

				unsigned long stringOffset = ULONG_MAX;
				unsigned long soundsOffset = ULONG_MAX;
				unsigned long typesOffset = ULONG_MAX;
				unsigned long groupsOffset = ULONG_MAX;
				unsigned long banksOffset = ULONG_MAX;

				std::vector<unsigned long> stringOffsets{};
				brsarSymbMaskHeader soundsMaskHeader;
				brsarSymbMaskHeader typesMaskHeader;
				brsarSymbMaskHeader groupsMaskHeader;
				brsarSymbMaskHeader banksMaskHeader;

				bool populate(lava::byteArray& bodyIn, std::size_t addressIn);
			};


			struct brsarInfoSoundEntry
			{
				unsigned long address = ULONG_MAX;

				unsigned long stringID = ULONG_MAX;
				unsigned long fileID = ULONG_MAX;
				unsigned long playerID = ULONG_MAX;
				brawlReference param3DRefOffset = ULLONG_MAX;
				unsigned char volume = UCHAR_MAX;
				unsigned char playerPriority = UCHAR_MAX;
				unsigned char soundType = UCHAR_MAX;
				unsigned char remoteFilter = UCHAR_MAX;
				brawlReference soundInfoRef = ULLONG_MAX;
				unsigned long userParam1 = ULONG_MAX;
				unsigned long userParam2 = ULONG_MAX;
				unsigned char panMode = UCHAR_MAX;
				unsigned char panCurve = UCHAR_MAX;
				unsigned char actorPlayerID = UCHAR_MAX;
				unsigned char reserved = UCHAR_MAX;

				bool populate(lava::byteArray& bodyIn, std::size_t addressIn);
			};
			struct brsarInfoBankEntry
			{
				unsigned long address = ULONG_MAX;

				unsigned long stringID = ULONG_MAX;
				unsigned long fileID = ULONG_MAX;
				unsigned long padding = ULONG_MAX;

				bool populate(lava::byteArray& bodyIn, std::size_t addressIn);
			};
			struct brsarInfoPlayerEntry
			{
				unsigned long address = ULONG_MAX;

				unsigned long stringID = ULONG_MAX;
				unsigned char playableSoundCount = UCHAR_MAX;
				unsigned char padding = UCHAR_MAX;
				unsigned short padding2 = USHRT_MAX;
				unsigned long heapSize = ULONG_MAX;
				unsigned long reserved = ULONG_MAX;

				bool populate(lava::byteArray& bodyIn, std::size_t addressIn);
			};
			struct brsarInfoFileEntry
			{
				unsigned long address = ULONG_MAX;

				unsigned long groupID = ULONG_MAX;
				unsigned long index = ULONG_MAX;

				bool populate(lava::byteArray& bodyIn, std::size_t addressIn);
			};
			struct brsarInfoFileHeader
			{
				unsigned long address = ULONG_MAX;

				unsigned long headerLength = ULONG_MAX;
				unsigned long dataLength = ULONG_MAX;
				unsigned long entryNumber = ULONG_MAX;
				brawlReference stringOffset = ULLONG_MAX;
				brawlReference listOffset = ULLONG_MAX;

				std::vector<brsarInfoFileEntry> entries;

				bool populate(lava::byteArray& bodyIn, std::size_t addressIn);
			};
			struct brsarInfoGroupEntry
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
			struct brsarInfoGroupHeader
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
			struct brsarInfoSection
			{
				unsigned long address = ULONG_MAX;

				unsigned long length = ULONG_MAX;
				brawlReference soundsSectionReference = ULLONG_MAX;
				brawlReference banksSectionReference = ULLONG_MAX;
				brawlReference playerSectionReference = ULLONG_MAX;
				brawlReference filesSectionReference = ULLONG_MAX;
				brawlReference groupsSectionReference = ULLONG_MAX;

				brawlReferenceVector soundsSection;
				brawlReferenceVector banksSection;
				brawlReferenceVector playerSection;
				brawlReferenceVector filesSection;
				brawlReferenceVector groupsSection;

				std::vector<brsarInfoSoundEntry> soundEntries{};
				std::vector<brsarInfoFileHeader> fileHeaders{};

				bool populate(lava::byteArray& bodyIn, std::size_t addressIn);
				std::vector<brsarInfoFileHeader*> findFilesWithGroupID(lava::byteArray& bodyIn, unsigned long groupIDIn);
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
				brawlReference ntIfoTableRef = ULLONG_MAX;
				brawlReference ntGraphEnvTableRef = ULLONG_MAX;
				brawlReference ntRandomizerTableRef = ULLONG_MAX;
				unsigned long ntReserved = ULONG_MAX;

				void copyOverDataInfoProperties(const dataInfo& sourceInfo);

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

			struct wavePacket
			{
				bool populated = 0;

				unsigned long address = ULONG_MAX;

				unsigned long length = ULONG_MAX;
				unsigned long paddingLength = ULONG_MAX;

				std::vector<unsigned char> body{};

				bool populate(lava::byteArray& bodyIn, unsigned long addressIn, unsigned long lengthIn);
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

				unsigned long channelInfoTableLength = ULONG_MAX;
				std::vector<unsigned long> channelInfoTable{};

				wavePacket packetContents;

				unsigned long getLengthInBytes() const;
				void copyOverWaveInfoProperties(const waveInfo& sourceInfo);

				bool populate(lava::byteArray& bodyIn, unsigned long addressIn);
				bool exportContents(std::ostream& destinationStream);
			};
			struct rwsdWaveSection
			{
				unsigned long address = ULONG_MAX;

				unsigned long length = ULONG_MAX;

				std::vector<unsigned long> entryOffsets{};
				std::vector<waveInfo> entries{};

				void pushEntry(const waveInfo& entryIn);

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

				bool hasExclusiveWave(unsigned long dataSectionIndex);
				bool isFirstToUseWave(unsigned long dataSectionIndex);
				waveInfo* getWaveInfoAssociatedWithDataInfo(unsigned long dataSectionIndex);
				bool populateWavePacket(lava::byteArray& bodyIn, unsigned long parentGroupWaveDataAddress, unsigned long collectionDataOffset, unsigned long dataSectionIndex);
				signed long overwriteSound(unsigned long dataSectionIndex, const dataInfo& dataInfoIn, const waveInfo& waveInfoIn, bool allowSharedWaveSplit = 0);

				bool populate(lava::byteArray& bodyIn, std::size_t addressIn);
				bool exportContents(std::ostream& destinationStream);
			};

			struct brsarFileSection
			{
				unsigned long address = ULONG_MAX;

				unsigned long length = ULONG_MAX;
				std::vector<rwsd> rwsdEntries{};

				bool catalogueRWSD(lava::byteArray& bodyIn, std::size_t addressIn);
				bool catalogueRWSDCheat(lava::byteArray& bodyIn, std::size_t addressIn);

				bool populate(lava::byteArray& bodyIn, std::size_t addressIn);
			};

			/*
			struct groupPortBundle
			{
				bool populatedSuccessfully = 0;

				unsigned long groupID = ULONG_MAX;
				unsigned long groupOffset = ULONG_MAX;
				brsarInfoGroupHeader groupHeader;
				brawlReferenceVector collectionReferences;
				std::vector<brsarInfoGroupEntry> collections{};
				std::vector<unsigned long> groupEntriesFirstSoundIndex{};
				std::vector<rwsd> collectionRWSDs{};

				bool populateWavePacket(lava::byteArray& bodyIn, unsigned long collectionIndex, unsigned long dataEntryIndex);
				bool populateAllWavePackets(lava::byteArray& bodyIn);
				bool exportAsSawnd(std::ostream& outputStream);
			};
			struct groupPortEntryInfoBundle
			{
				bool sourceGroupDataIndexLocked = 0;
				bool destinationGroupDataIndexLocked = 0;
				unsigned long sourceGroupInfoIndex = ULONG_MAX;
				unsigned long destinationGroupInfoIndex = ULONG_MAX;
				std::pair<unsigned long, unsigned long> sourceGroupDataIndex = { ULONG_MAX, ULONG_MAX };
				std::pair<unsigned long, unsigned long> destinationGroupDataIndex = { ULONG_MAX, ULONG_MAX };
			};

			enum class groupPortSoundCorrErrorCode
			{
				sCEC_NULL = 0x00,
				sCEC_NO_MATCH,
				sCEC_PUSHED_BY_OTHER,
				sCEC_PUSHED_BY_OVERRIDE,
				sCEC_SHARED_WAVE,
			};

			struct groupPortEntryErrorBundle
			{
				unsigned long sourceGroupInfoIndex = ULONG_MAX;
				std::pair<unsigned long, unsigned long> sourceGroupDataIndex = { ULONG_MAX, ULONG_MAX };
			};
			struct groupPortSoundCorrespondence
			{
				std::unordered_map<std::string, groupPortEntryInfoBundle> matches{};
				//std::map<groupPortSoundCorrErrorCode, std::vector<std::pair<unsigned long, unsigned long>>> failedMatches{};
				std::map<groupPortSoundCorrErrorCode, std::vector<groupPortEntryErrorBundle>> failedMatches{};
				unsigned long successfulMatches = ULONG_MAX;

				bool outputCorrespondenceData(std::ostream& output);
				bool summarizeResults(std::ostream& output);
			};
			*/

			struct brsarFile
			{
				lava::byteArray contents;

				brsarSymbSection symbSection;
				brsarInfoSection infoSection;
				brsarFileSection fileSection;

				bool init(std::string filePathIn);

				std::string getSymbString(unsigned long indexIn);
				bool summarizeSymbStringData(std::ostream& output = std::cout);
				bool outputConsecutiveSoundEntryStringsWithSameFileID(unsigned long startingIndex, std::ostream& output = std::cout);

				unsigned long getGroupOffset(unsigned long groupIDIn);
				bool listSoundsInGroup(unsigned long groupIDIn, std::ostream& output = std::cout);

				/*groupPortBundle getGroupPortBundle(groupFileInfo groupInfoIn);
				groupPortSoundCorrespondence getGroupPortSoundCorr(const groupPortBundle& sourceGroupBundle, const groupPortBundle& destinationGroupBundle);
				bool portCorrespondingSounds(groupPortSoundCorrespondence& soundCorr, const groupPortBundle& sourceGroupBundle, groupPortBundle& destinationGroupBundle, bool portEmptySounds = 1, bool allowSharedDestinationWaveSplit = 0);
				
				bool portGroupToGroup(unsigned long sourceCharFIDIn, unsigned long destinationCharFIDIn, std::ostream& contentsOutput, std::ostream& logOutput = std::cout, groupPortSoundCorrespondence* soundMappingOut = nullptr);*/

				bool exportSawnd(std::size_t groupID, std::string targetFilePath);
			};

			struct sawndHeader
			{
				unsigned char address = ULONG_MAX;

				unsigned char sawndVersion = ULONG_MAX;
				unsigned long groupID = ULONG_MAX;
				unsigned long waveDataLength = ULONG_MAX;

				std::vector<std::array<unsigned long, 3>> fileInfoTriples{};

				bool populate(const lava::byteArray& bodyIn, unsigned long addressIn);
			};
			//groupPortBundle buildBundleFromSawnd(std::string filePathIn);
			bool summarizeSawndMetadata(std::string filePathIn, std::ostream& output = std::cout);
		}
	}
}


#endif
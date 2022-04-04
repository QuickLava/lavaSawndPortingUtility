#ifndef LAVA_SAWND_PORTING_UTILITY_H_V1
#define LAVA_SAWND_PORTING_UTILITY_H_V1

#include "lavaSawndz.h"
namespace lava
{
	namespace brawl
	{
		namespace sawndz
		{
			struct groupPortBundle
			{
				bool populatedSuccessfully = 0;

				unsigned long groupID = ULONG_MAX;
				unsigned long groupOffset = ULONG_MAX;
				brsarInfoGroupHeader groupHeader;
				brawlReferenceVector collectionReferences;
				std::vector<std::pair<unsigned long, unsigned long>> collectionsOrder{};
				std::vector<brsarInfoGroupEntry> activeCollections{};
				std::vector<brsarInfoGroupEntry> inactiveCollections{};
				std::vector<unsigned long> groupEntriesFirstSoundIndex{};
				std::vector<rwsd> activeCollectionRWSDs{};
				std::vector<std::vector<unsigned char>> inactiveCollectionsInfo{};
				std::vector<std::vector<unsigned char>> inactiveCollectionsData{};

				brsarInfoGroupEntry* getCollectionPtr(std::pair<unsigned long, unsigned long> collOrderIndex);

				bool populateWavePacket(lava::byteArray& bodyIn, unsigned long collectionIndex, unsigned long dataEntryIndex);
				bool populateAllWavePackets(lava::byteArray& bodyIn);
				bool exportAsSawnd(std::ostream& outputStream);
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
			struct groupPortEntryInfoBundle
			{
				bool sourceGroupDataIndexLocked = 0;
				bool destinationGroupDataIndexLocked = 0;
				unsigned long sourceGroupInfoIndex = ULONG_MAX;
				unsigned long destinationGroupInfoIndex = ULONG_MAX;
				std::pair<unsigned long, unsigned long> sourceGroupDataIndex = { ULONG_MAX, ULONG_MAX };
				std::pair<unsigned long, unsigned long> destinationGroupDataIndex = { ULONG_MAX, ULONG_MAX };
			};

			struct groupPortSoundCorrespondence
			{
				std::unordered_map<std::string, groupPortEntryInfoBundle> matches{};
				std::map<groupPortSoundCorrErrorCode, std::vector<groupPortEntryErrorBundle>> failedMatches{};
				unsigned long successfulMatches = ULONG_MAX;

				bool outputCorrespondenceData(std::ostream& output);
				bool summarizeResults(std::ostream& output);
			};

			groupPortBundle getGroupPortBundle(brsarFile& sourceBrsar, groupFileInfo groupInfoIn);
			groupPortSoundCorrespondence getGroupPortSoundCorr(brsarFile& sourceBrsar, const groupPortBundle& sourceGroupBundle, const groupPortBundle& destinationGroupBundle);
			bool portCorrespondingSounds(groupPortSoundCorrespondence& soundCorr, const groupPortBundle& sourceGroupBundle, groupPortBundle& destinationGroupBundle, bool portEmptySounds = 1, bool allowSharedDestinationWaveSplit = 0);

			bool portGroupToGroup(brsarFile& sourceBrsar, unsigned long sourceCharFIDIn, unsigned long destinationCharFIDIn, std::ostream& contentsOutput, std::ostream& logOutput = std::cout, groupPortSoundCorrespondence* soundMappingOut = nullptr);
		}
	}
}

#endif
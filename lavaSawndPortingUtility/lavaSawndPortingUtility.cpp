#include "lavaSawndPortingUtility.h"

namespace lava
{
	namespace brawl
	{
		namespace sawndz
		{
			brsarInfoGroupEntry* groupPortBundle::getCollectionPtr(std::pair<unsigned long, unsigned long> collOrderIndex)
			{
				brsarInfoGroupEntry* result = nullptr;
				// If the current collection isn't disabled
				if (collOrderIndex.first == 0)
				{
					result = &activeCollections[collOrderIndex.second];
				}
				// Otherwise
				else
				{
					result = &inactiveCollections[collOrderIndex.second];
				}
				return result;
			}

			bool groupPortBundle::populateWavePacket(lava::byteArray& bodyIn, unsigned long collectionIndex, unsigned long dataEntryIndex)
			{
				bool result = 0;

				if (populatedSuccessfully)
				{
					if (collectionIndex < activeCollections.size())
					{
						result = activeCollectionRWSDs[collectionIndex].populateWavePacket(bodyIn, groupHeader.waveDataOffset, activeCollections[collectionIndex].dataOffset, dataEntryIndex);
					}
				}

				return result;
			}
			bool groupPortBundle::populateAllWavePackets(lava::byteArray& bodyIn)
			{
				bool result = 0;

				if (populatedSuccessfully)
				{
					for (unsigned long i = 0; i < activeCollectionRWSDs.size(); i++)
					{
						std::vector<dataInfo>* dataVector = &activeCollectionRWSDs[i].dataSection.entries;
						for (unsigned long u = 0; u < dataVector->size(); u++)
						{
							populateWavePacket(bodyIn, i, u);
						}
					}
				}

				return result;
			}
			bool groupPortBundle::exportAsSawnd(std::ostream& outputStream)
			{
				bool result = 0;
				lava::writeRawDataToStream(outputStream, char(0x02));
				lava::writeRawDataToStream(outputStream, groupHeader.stringID);
				lava::writeRawDataToStream(outputStream, groupHeader.waveDataLength);

				for (unsigned long i = 0; i < collectionsOrder.size(); i++)
				{
					std::pair<unsigned long, unsigned long> collectionIndeces = collectionsOrder[i];
					brsarInfoGroupEntry* currEntry = nullptr;
					// If the current collection isn't disabled
					if (collectionIndeces.first == 0)
					{
						currEntry = &activeCollections[collectionIndeces.second];
					}
					// Otherwise
					else
					{
						currEntry = &inactiveCollections[collectionIndeces.second];
						if (i != 0)
						{
							std::pair<unsigned long, unsigned long> prevCollectionIndeces = collectionsOrder[i - 1];
							brsarInfoGroupEntry* prevEntry = getCollectionPtr(collectionsOrder[i -1]);
							if (prevCollectionIndeces.first == 0)
							{
								prevEntry = &activeCollections[prevCollectionIndeces.second];
							}
							else
							{
								prevEntry = &inactiveCollections[prevCollectionIndeces.second];
							}
							currEntry->headerOffset = prevEntry->headerOffset + prevEntry->headerLength;
							//currEntry->dataOffset = prevEntry->dataOffset + prevEntry->dataLength;
							if (currEntry->dataOffset != 0x00000000)
							{
								currEntry->dataOffset = prevEntry->dataOffset + prevEntry->dataLength;
							}
						}
					}
					lava::writeRawDataToStream(outputStream, currEntry->fileID);
					lava::writeRawDataToStream(outputStream, currEntry->dataOffset);
					lava::writeRawDataToStream(outputStream, currEntry->dataLength);
				}
				for (unsigned long i = 0; i < collectionsOrder.size(); i++)
				{
					std::pair<unsigned long, unsigned long> collectionIndeces = collectionsOrder[i];
					if (collectionIndeces.first == 0)
					{
						activeCollectionRWSDs[collectionIndeces.second].exportContents(outputStream);
					}
					else
					{
						char* infoBytesPtr = (char*)(inactiveCollectionsInfo[collectionIndeces.second].data());
						outputStream.write(infoBytesPtr, inactiveCollectionsInfo[collectionIndeces.second].size());
					}
				}
				for (unsigned long i = 0; i < collectionsOrder.size(); i++)
				{
					std::pair<unsigned long, unsigned long> collectionIndeces = collectionsOrder[i];
					if (collectionIndeces.first == 0)
					{
						rwsd* currRWSD = &activeCollectionRWSDs[collectionIndeces.second];
						for (unsigned long u = 0; u < currRWSD->waveSection.entries.size(); u++)
						{
							waveInfo* currEntry = &currRWSD->waveSection.entries[u];
							if (currEntry->packetContents.populated)
							{
								//std::cout << "Collection " << i << ", Wave 0x" << lava::numToHexStringWithPadding(u, 0x04) << "\n";
								//std::cout << "\tOutput Stream Pos: 0x" << lava::numToHexStringWithPadding(outputStream.tellp(), 0x08) << "\n";
								std::vector<char>* currEntryPacketBodyPtr = (std::vector<char>*) (&currEntry->packetContents.body);
								outputStream.write(currEntryPacketBodyPtr->data(), currEntryPacketBodyPtr->size());
								//std::cout << "\tOutput Stream Pos (Post Data-Write): 0x" << lava::numToHexStringWithPadding(outputStream.tellp(), 0x08) << "\n";
								std::vector<char> padding{};
								if (currEntry->packetContents.paddingLength > 0x100)
								{
									std::cerr << "Padding length not initialized properly.\n";
								}
								else
								{
									padding.resize(currEntry->packetContents.paddingLength, 0x00);
								}
								outputStream.write(padding.data(), padding.size());
								//std::cout << "\tOutput Stream Pos (Post Pad-Write): 0x" << lava::numToHexStringWithPadding(outputStream.tellp(), 0x08) << "\n";
							}
							else
							{
								unsigned long prevEndLoc = ULONG_MAX;
								unsigned long nextStartLoc = ULONG_MAX;
								if (u > 0)
								{
									waveInfo* prevEntry = &currRWSD->waveSection.entries[u - 1];
									if (prevEntry != nullptr && prevEntry->packetContents.populated)
									{
										prevEndLoc = prevEntry->dataLocation + prevEntry->packetContents.body.size() + prevEntry->packetContents.paddingLength;
									}
								}
								else
								{
									prevEndLoc = 0;
								}
								if (u < (currRWSD->waveSection.entries.size() - 1))
								{
									waveInfo* nextEntry = &currRWSD->waveSection.entries[u + 1];
									if (nextEntry != nullptr)
									{
										nextStartLoc = nextEntry->dataLocation;
									}
								}
								else
								{
									nextStartLoc = groupHeader.waveDataLength;
								}
								if (prevEndLoc != ULONG_MAX && nextStartLoc != ULONG_MAX)
								{
									std::cout << "Packet for Collection " << i << ", Wave 0x" << lava::numToHexStringWithPadding(u, 0x02) << " wasn't populated, exporting improvized packet!\n";

									currEntry->packetContents.address = _NOT_IN_FILE_ADDRESS;
									currEntry->packetContents.populated = 1;
									currEntry->packetContents.body.clear();
									currEntry->packetContents.body.resize(nextStartLoc - prevEndLoc, 0x00);
									currEntry->packetContents.length = currEntry->packetContents.body.size();
									currEntry->packetContents.paddingLength = 0;
									std::vector<char>* improvizedPacketBodyPtr = (std::vector<char>*) (&currEntry->packetContents.body);
									outputStream.write(improvizedPacketBodyPtr->data(), improvizedPacketBodyPtr->size());
								}
								else
								{
									std::cerr << "Couldn't Export Wave Data for Collection " << i << ", Wave #" << u << "\n";
									std::cerr << "Packet contents not populated, and unable to determine appropriate bounds for an improvized packet!\n";
								}
							}
						}
					}
					else
					{
						char* dataBytesPtr = (char*)(inactiveCollectionsData[collectionIndeces.second].data());
						outputStream.write(dataBytesPtr, inactiveCollectionsData[collectionIndeces.second].size());
					}
				}

				result = outputStream.good();
				return result;
			}

			bool groupPortSoundCorrespondence::outputCorrespondenceData(std::ostream& output)
			{
				bool result = 0;

				if (output.good())
				{
					std::map<unsigned long, unsigned long> sortedIndeces{};
					for (auto i : matches)
					{
						if (i.second.sourceGroupDataIndex.first != ULONG_MAX && i.second.sourceGroupDataIndex.second != ULONG_MAX)
						{
							sortedIndeces[i.second.sourceGroupInfoIndex] = i.second.destinationGroupInfoIndex;
						}
					}
					for (auto i : sortedIndeces)
					{
						output << "0x" << lava::numToHexStringWithPadding(i.first, 0x04) << " 0x" << lava::numToHexStringWithPadding(i.second, 0x04) << "\n";
					}
					for (auto i : failedMatches)
					{
						std::vector<groupPortEntryMessageBundle>* indexPairVec = &i.second;
						for (unsigned long u = 0; u < indexPairVec->size(); u++)
						{
							output << "0x" << lava::numToHexStringWithPadding((*indexPairVec)[u].sourceGroupInfoIndex, 0x04) << " 0x" << lava::numToHexStringWithPadding(sortedIndeces.begin()->second, 0x04) << "\n";
						}
					}
					result = output.good();
				}

				return result;
			}
			bool groupPortSoundCorrespondence::outputLMPRPatch(std::ostream& output, unsigned long sourceFighterID, unsigned long destinationFighterID)
			{
				bool result = 0;

				if (output.good())
				{
					output << "<?xml version=\"1.0\"?>\n";
					output << "<!--This patch was produced automatically using lavaSawndPortingUtility " << version << ".-->\n";
					output << "<!--It is intended for use with lavaMontyPythonReimp (Github Page: https://github.com/QuickLava/lavaMontyPythonReimp/ ).-->\n";
					output << "<!--Note: Remember to change Death and Damage SFX IDs in PSACompressor after running this patch!-->\n";
					output << "<movesetPatch name = \""; 
					auto sourceFighterNameItr = lava::brawl::LAVA_CHARA_FID_TO_NAME.find(sourceFighterID);
					auto destFighterNameItr = lava::brawl::LAVA_CHARA_FID_TO_NAME.find(destinationFighterID);
					if (sourceFighterNameItr != lava::brawl::LAVA_CHARA_FID_TO_NAME.end())
					{
						output << sourceFighterNameItr->second;
					}
					else
					{
						output << "Fighter0x" << lava::numToHexStringWithPadding(sourceFighterID, 0x02);
					}
					output << "-to-";
					if (destFighterNameItr != lava::brawl::LAVA_CHARA_FID_TO_NAME.end())
					{
						output << destFighterNameItr->second;
					}
					else
					{
						output << "Fighter0x" << lava::numToHexStringWithPadding(destinationFighterID, 0x02);
					}
					output << " Port SFX ID Patch\">\n";
					output << "\t<targets>\n";
					output << "\t\t<command name = \"Sound Effect\" signature = \"0x0A000100\" paramIndex = \"0\"/>\n";
					output << "\t\t<command name = \"Sound Effect (Transient)\" signature = \"0x0A020100\" paramIndex = \"0\"/>\n";
					output << "\t\t<command name = \"Stop Sound Effect\" signature = \"0x0A030100\" paramIndex = \"0\"/>\n";
					output << "\t\t<command name = \"Victory\" signature = \"0x0A050100\" paramIndex = \"0\"/>\n";
					output << "\t\t<command name = \"Other Sound Effect (landing)\" signature = \"0x0A090100\" paramIndex = \"0\"/>\n";
					output << "\t\t<command name = \"Other Sound Effect (impact)\" signature = \"0x0A0A0100\" paramIndex = \"0\"/>\n";
					output << "\t\t<command name = \"Stepping Sound Effect\" signature = \"0x0A010100\" paramIndex = \"0\"/>\n";
					output << "\t</targets>\n";
					output << "\t<modifications>\n";
					output << "\t</modifications>\n";
					output << "\t<directApply name = \"SFX ID Replacement\">\n";

					std::map<unsigned long, unsigned long> sortedIndeces{};
					for (auto i : matches)
					{
						if (i.second.sourceGroupDataIndex.first != ULONG_MAX && i.second.sourceGroupDataIndex.second != ULONG_MAX)
						{
							sortedIndeces[i.second.sourceGroupInfoIndex] = i.second.destinationGroupInfoIndex;
						}
					}
					output << "\t\t<!--Successfully found destinations for " << successfulMatches << " sound(s):-->\n";
					for (auto i : sortedIndeces)
					{
						output << "\t\t" << lava::numToHexStringWithPadding(i.first, 0x08) << ", " << lava::numToHexStringWithPadding(i.second, 0x08) << "\n";
					}
					for (auto i : failedMatches)
					{
						if (i.second.size())
						{
							output << "\t\t<!--";
							switch (i.first)
							{
								case groupPortSoundCorrMessageCode::sCMC_NO_MATCH:
								{
									output << "Couldn't find destinations for " << i.second.size() << " sound(s):";
									break;
								}
								case groupPortSoundCorrMessageCode::sCMC_PUSHED_BY_OTHER:
								{
									output << "Non-overrides caused " << i.second.size() << " sound(s) to be omitted:";
									break;
								}
								case groupPortSoundCorrMessageCode::sCMC_PUSHED_BY_OVERRIDE:
								{
									output << "Overrides caused " << i.second.size() << " sound(s) to be omitted:";
									break;
								}
								case groupPortSoundCorrMessageCode::sCMC_SHARED_WAVE_ADD_FAIL:
								{
									output << i.second.size() << " sound(s) omitted because their destinations used shared wave entries:";
									break;
								}
								default:
								{
									output << i.second.size() << " sound(s) omitted for unknown reasons:";
									break;
								}
							}
							output << "-->\n";
							std::vector<groupPortEntryMessageBundle>* indexPairVec = &i.second;
							for (unsigned long u = 0; u < indexPairVec->size(); u++)
							{
								output << "\t\t" << lava::numToHexStringWithPadding((*indexPairVec)[u].sourceGroupInfoIndex, 0x08) << ", " << lava::numToHexStringWithPadding(sortedIndeces.begin()->second, 0x08) << "\n";
							}
						}
					}

					output << "\t</directApply>\n";
					output << "</movesetPatch>\n";
					result = output.good();
				}

				return result;
			}

			groupPortBundle getGroupPortBundle(brsarFile& sourceBrsar, groupFileInfo groupInfoIn)
			{
				groupPortBundle result;

				if (sourceBrsar.contents.populated())
				{
					result.groupID = groupInfoIn.groupID;
					result.groupOffset = sourceBrsar.getGroupOffset(groupInfoIn.groupID);

					if (result.groupOffset != ULONG_MAX)
					{
						result.populatedSuccessfully = 1;
						result.populatedSuccessfully &= result.groupHeader.populate(sourceBrsar.contents, result.groupOffset);
						result.populatedSuccessfully &= result.collectionReferences.populate(sourceBrsar.contents, result.groupHeader.listOffset.getAddress(sourceBrsar.infoSection.address + 0x08));

						std::size_t totalCollectionCount = result.collectionReferences.refs.size();
						result.activeCollections.reserve(totalCollectionCount);
						result.inactiveCollections.reserve(totalCollectionCount);

						// Populate the collections associated with this group.
						for (std::size_t i = 0; i < totalCollectionCount; i++)
						{
							brsarInfoGroupEntry tempEntry;
							bool populatedSuccessfully = tempEntry.populate(sourceBrsar.contents, result.collectionReferences.refs[i].getAddress(sourceBrsar.infoSection.address + 0x08));
							if (groupInfoIn.fileIDAcceptable(tempEntry.fileID))
							{
								result.collectionsOrder.push_back(std::make_pair(0, result.activeCollections.size()));
								result.activeCollections.push_back(tempEntry);
								result.populatedSuccessfully &= populatedSuccessfully;
							}
							else
							{
								result.collectionsOrder.push_back(std::make_pair(1, result.inactiveCollections.size()));
								result.inactiveCollections.push_back(tempEntry);
								result.populatedSuccessfully &= populatedSuccessfully;
								std::cerr << "\tCollection (File ID: " << tempEntry.fileID << ") is disabled, and won't be used for porting.\n";
							}
						}

						// Find the first Sound Entry with each collection's File ID.
						result.groupEntriesFirstSoundIndex.resize(result.activeCollections.size(), ULONG_MAX);
						for (std::size_t u = 0; u < result.activeCollections.size(); u++)
						{
							for (std::size_t i = 0; result.groupEntriesFirstSoundIndex[u] == ULONG_MAX && i < sourceBrsar.infoSection.soundEntries.size(); i++)
							{
								if (sourceBrsar.infoSection.soundEntries[i].fileID == result.activeCollections[u].fileID)
								{
									result.groupEntriesFirstSoundIndex[u] = i;
								}
							}
						}

						result.activeCollectionRWSDs.resize(result.activeCollections.size());
						for (std::size_t i = 0; i < result.activeCollections.size(); i++)
						{
							result.populatedSuccessfully &= result.activeCollectionRWSDs[i].populate(sourceBrsar.contents, result.groupHeader.headerOffset + result.activeCollections[i].headerOffset);
						}

						std::vector<unsigned char> temp{};
						std::size_t numGotten = SIZE_MAX;
						result.inactiveCollectionsInfo.reserve(result.inactiveCollections.size());
						result.inactiveCollectionsData.reserve(result.inactiveCollections.size());
						for (std::size_t i = 0; i < result.inactiveCollections.size(); i++)
						{
							brsarInfoGroupEntry* currCollection = &result.inactiveCollections[i];
							temp = sourceBrsar.contents.getBytes(currCollection->headerLength, result.groupHeader.headerOffset + currCollection->headerOffset, numGotten);
							result.inactiveCollectionsInfo.push_back(temp);
							temp = sourceBrsar.contents.getBytes(currCollection->dataLength, result.groupHeader.waveDataOffset + currCollection->dataOffset, numGotten);
							result.inactiveCollectionsData.push_back(temp);
						}
					}
				}

				return result;
			}
			groupPortSoundCorrespondence getGroupPortSoundCorr(brsarFile& sourceBrsar, const groupPortBundle& sourceGroupBundle, const groupPortBundle& destinationGroupBundle)
			{
				groupPortSoundCorrespondence result;

				if (sourceGroupBundle.populatedSuccessfully)
				{
					if (destinationGroupBundle.populatedSuccessfully)
					{
						result.successfulMatches = 0;

						const std::vector<dataInfo>* dataVectorPtr = nullptr;
						const std::vector<waveInfo>* waveVectorPtr = nullptr;
						const dataInfo* dataPtr = nullptr;
						const waveInfo* wavePtr = nullptr;
						unsigned long soundEffectNameIndex = ULONG_MAX;
						groupPortEntryInfoBundle* currentEntry = nullptr;
						std::string unshuckedName = "";
						std::string shuckedName = "";
						std::unordered_map<std::string, std::string>::iterator overrideUsedReceiver = soundEffectNameOverrides.end();

						for (std::size_t i = 0; i < destinationGroupBundle.activeCollectionRWSDs.size(); i++)
						{
							dataVectorPtr = &destinationGroupBundle.activeCollectionRWSDs[i].dataSection.entries;
							waveVectorPtr = &destinationGroupBundle.activeCollectionRWSDs[i].waveSection.entries;
							unsigned long firstSoundIndex = destinationGroupBundle.groupEntriesFirstSoundIndex[i];
							for (std::size_t u = 0; u < dataVectorPtr->size(); u++)
							{
								dataPtr = &(*dataVectorPtr)[u];
								wavePtr = &(*waveVectorPtr)[dataPtr->ntWaveIndex];
								soundEffectNameIndex = 0x194 + firstSoundIndex + u;
								auto emplaceResult = result.matches.emplace(std::make_pair(shuckSoundEffectName(sourceBrsar.getSymbString(soundEffectNameIndex), &overrideUsedReceiver), groupPortEntryInfoBundle()));
								currentEntry = &emplaceResult.first->second;
								currentEntry->destinationGroupDataIndex = std::make_pair(i, u);
								currentEntry->destinationGroupInfoIndex = soundEffectNameIndex - 0x194;
								currentEntry->destinationGroupDataIndexLocked = overrideUsedReceiver != soundEffectNameOverrides.end();
							}
						}
						for (std::size_t i = 0; i < sourceGroupBundle.activeCollectionRWSDs.size(); i++)
						{
							dataVectorPtr = &sourceGroupBundle.activeCollectionRWSDs[i].dataSection.entries;
							waveVectorPtr = &sourceGroupBundle.activeCollectionRWSDs[i].waveSection.entries;
							unsigned long firstSoundIndex = sourceGroupBundle.groupEntriesFirstSoundIndex[i];
							for (std::size_t u = 0; u < dataVectorPtr->size(); u++)
							{
								shuckedName = "";
								dataPtr = &(*dataVectorPtr)[u];
								wavePtr = &(*waveVectorPtr)[dataPtr->ntWaveIndex];
								soundEffectNameIndex = 0x194 + firstSoundIndex + u;
								unshuckedName = sourceBrsar.getSymbString(soundEffectNameIndex);
								shuckedName = shuckSoundEffectName(unshuckedName, &overrideUsedReceiver);
								auto findResult = result.matches.find(shuckedName);
								if (findResult == result.matches.end())
								{
									groupPortEntryMessageBundle newErrorEntry;
									newErrorEntry.sourceGroupDataIndex = std::make_pair(i, u);
									newErrorEntry.sourceGroupInfoIndex = soundEffectNameIndex - 0x194;
									result.failedMatches[groupPortSoundCorrMessageCode::sCMC_NO_MATCH].push_back(newErrorEntry);
								}
								else
								{
									if (findResult->second.sourceGroupDataIndex.first != ULONG_MAX &&
										findResult->second.sourceGroupDataIndex.second != ULONG_MAX)
									{
										if (!findResult->second.sourceGroupDataIndexLocked)
										{
											if (overrideUsedReceiver != soundEffectNameOverrides.end())
											{
												groupPortEntryMessageBundle newErrorEntry;
												newErrorEntry.sourceGroupDataIndex = findResult->second.sourceGroupDataIndex;
												newErrorEntry.sourceGroupInfoIndex = soundEffectNameIndex - 0x194;
												newErrorEntry.destinationGroupDataIndex = findResult->second.destinationGroupDataIndex;
												newErrorEntry.destinationGroupInfoIndex = findResult->second.destinationGroupInfoIndex;
												result.failedMatches[groupPortSoundCorrMessageCode::sCMC_PUSHED_BY_OVERRIDE].push_back(newErrorEntry);
												findResult->second.sourceGroupDataIndexLocked = 1;
											}
											else
											{
												groupPortEntryMessageBundle newErrorEntry;
												newErrorEntry.sourceGroupDataIndex = findResult->second.sourceGroupDataIndex;
												newErrorEntry.sourceGroupInfoIndex = soundEffectNameIndex - 0x194;
												newErrorEntry.destinationGroupDataIndex = findResult->second.destinationGroupDataIndex;
												newErrorEntry.destinationGroupInfoIndex = findResult->second.destinationGroupInfoIndex;
												result.failedMatches[groupPortSoundCorrMessageCode::sCMC_PUSHED_BY_OTHER].push_back(newErrorEntry);
											}
											findResult->second.sourceGroupInfoIndex = soundEffectNameIndex - 0x194;
											findResult->second.sourceGroupDataIndex = std::make_pair(i, u);
										}
										else
										{
											groupPortEntryMessageBundle newErrorEntry;
											newErrorEntry.sourceGroupDataIndex = std::make_pair(i, u);
											newErrorEntry.sourceGroupInfoIndex = soundEffectNameIndex - 0x194;
											newErrorEntry.destinationGroupDataIndex = findResult->second.destinationGroupDataIndex;
											newErrorEntry.destinationGroupInfoIndex = findResult->second.destinationGroupInfoIndex;

											result.failedMatches[groupPortSoundCorrMessageCode::sCMC_PUSHED_BY_OVERRIDE].push_back(newErrorEntry);
										}
									}
									else
									{
										if (overrideUsedReceiver != soundEffectNameOverrides.end())
										{
											findResult->second.sourceGroupDataIndexLocked = 1;
										}
										findResult->second.sourceGroupInfoIndex = soundEffectNameIndex - 0x194;
										findResult->second.sourceGroupDataIndex = std::make_pair(i, u);
										result.successfulMatches++;
									}
								}
							}
						}


					}
				}

				return result;
			}

			bool portCorrespondingSounds(groupPortSoundCorrespondence& soundCorr, const groupPortBundle& sourceGroupBundle, groupPortBundle& destinationGroupBundle, bool portEmptySounds, bool maintainSharedWaves, bool allowSharedDestinationWaveSplit)
			{
				bool result = 0;

				if (soundCorr.successfulMatches != ULONG_MAX)
				{
					unsigned long copySourceCollIndex = ULONG_MAX;
					unsigned long copySourceDataIndex = ULONG_MAX;
					unsigned long copyDestCollIndex = ULONG_MAX;
					unsigned long copyDestDataIndex = ULONG_MAX;
					const dataInfo* copySourceData = nullptr;
					const waveInfo* copySourceWave = nullptr;

					std::unordered_map<std::string, groupPortEntryInfoBundle>::iterator;
					std::unordered_map<waveInfo*, dataInfo*>;

					// Key is Wave Index, Value is a pair: First Value == Source Data Index, Second Value == Destination Data Index
					std::unordered_map<const waveInfo*, std::vector<std::pair<unsigned long, unsigned long>>> waveIndecesToReferrerDataIndeces{};
					bool addEntryToMap = 0; 
					for (auto i : soundCorr.matches)
					{
						addEntryToMap = 0;
						copySourceCollIndex = i.second.sourceGroupDataIndex.first;
						copySourceDataIndex = i.second.sourceGroupDataIndex.second;
						if (copySourceCollIndex != ULONG_MAX && copySourceDataIndex != ULONG_MAX)
						{
							copyDestCollIndex = i.second.destinationGroupDataIndex.first;
							copyDestDataIndex = i.second.destinationGroupDataIndex.second;
							copySourceData = &sourceGroupBundle.activeCollectionRWSDs[copySourceCollIndex].dataSection.entries[copySourceDataIndex];
							copySourceWave = &sourceGroupBundle.activeCollectionRWSDs[copySourceCollIndex].waveSection.entries[copySourceData->ntWaveIndex];

							auto findResult = waveIndecesToReferrerDataIndeces.find(copySourceWave);
							if (!maintainSharedWaves || findResult == waveIndecesToReferrerDataIndeces.end())
							{
								if (portEmptySounds || copySourceWave->nibbles >= 3)
								{
									long changeInSize = destinationGroupBundle.activeCollectionRWSDs[copyDestCollIndex].overwriteSound(copyDestDataIndex, *copySourceData, *copySourceWave, allowSharedDestinationWaveSplit);

									if (changeInSize != ULONG_MAX)
									{
										if (changeInSize != _OVERWRITE_SOUND_SHARED_WAVE_RETURN_CODE)
										{
											addEntryToMap = 1;
											destinationGroupBundle.groupHeader.waveDataLength += changeInSize;
											destinationGroupBundle.activeCollections[copyDestCollIndex].dataLength += changeInSize;
											if (copyDestCollIndex < destinationGroupBundle.activeCollections.size() - 1)
											{
												destinationGroupBundle.activeCollections[copyDestCollIndex + 1].dataOffset += changeInSize;
											}
										}
										else
										{
											groupPortEntryMessageBundle newErrorEntry;
											newErrorEntry.sourceGroupDataIndex = i.second.sourceGroupDataIndex;
											newErrorEntry.sourceGroupInfoIndex = i.second.sourceGroupInfoIndex;
											newErrorEntry.destinationGroupDataIndex = i.second.destinationGroupDataIndex;
											newErrorEntry.destinationGroupInfoIndex = i.second.destinationGroupInfoIndex;

											i.second.sourceGroupDataIndex = { ULONG_MAX, ULONG_MAX };
											i.second.sourceGroupInfoIndex = ULONG_MAX;
											i.second.sourceGroupDataIndexLocked = 0;
											soundCorr.failedMatches[groupPortSoundCorrMessageCode::sCMC_SHARED_WAVE_ADD_FAIL].push_back(newErrorEntry);
											soundCorr.successfulMatches--;
										}
									}
								}
								else
								{
									std::cerr << "Skipping empty sound!\n";
								}
							}
							else
							{
								unsigned long donorDataIndex = findResult->second.begin()->second;
								unsigned long originalWaveIndex = destinationGroupBundle.activeCollectionRWSDs[copyDestCollIndex].dataSection.entries[copyDestDataIndex].ntWaveIndex;
								long changeInSize = destinationGroupBundle.activeCollectionRWSDs[copyDestCollIndex].shareWaveTargetBetweenDataEntries(copyDestDataIndex, donorDataIndex, copySourceData, _clearExistingWaveWhenPortingSharedWave);
								if (changeInSize != ULONG_MAX)
								{
									addEntryToMap = 1;
									if (changeInSize != _OVERWRITE_SOUND_SHARED_WAVE_RETURN_CODE)
									{
										destinationGroupBundle.groupHeader.waveDataLength += changeInSize;
										destinationGroupBundle.activeCollections[copyDestCollIndex].dataLength += changeInSize;
										if (copyDestCollIndex < destinationGroupBundle.activeCollections.size() - 1)
										{
											destinationGroupBundle.activeCollections[copyDestCollIndex + 1].dataOffset += changeInSize;
										}
										unsigned long newWaveIndex = destinationGroupBundle.activeCollectionRWSDs[copyDestCollIndex].dataSection.entries[copyDestDataIndex].ntWaveIndex;
										groupPortEntryMessageBundle newMessageEntry;
										newMessageEntry.sourceGroupDataIndex = i.second.sourceGroupDataIndex;
										newMessageEntry.sourceGroupInfoIndex = i.second.sourceGroupInfoIndex;
										newMessageEntry.destinationGroupDataIndex = i.second.destinationGroupDataIndex;
										newMessageEntry.destinationGroupInfoIndex = i.second.destinationGroupInfoIndex;
										newMessageEntry.specialMessageText = " (Orig. Wave ID: 0x" + lava::numToHexStringWithPadding(originalWaveIndex, 0x02);
										newMessageEntry.specialMessageText += ", New Wave ID: 0x" + lava::numToHexStringWithPadding(newWaveIndex, 0x02) + ")";
										soundCorr.otherMessages[groupPortSoundCorrMessageCode::sCMC_SHARED_WAVE_OPEN_SPACE].push_back(newMessageEntry);
									}
									else
									{
										groupPortEntryMessageBundle newMessageEntry;
										newMessageEntry.sourceGroupDataIndex = i.second.sourceGroupDataIndex;
										newMessageEntry.sourceGroupInfoIndex = i.second.sourceGroupInfoIndex;
										newMessageEntry.destinationGroupDataIndex = i.second.destinationGroupDataIndex;
										newMessageEntry.destinationGroupInfoIndex = i.second.destinationGroupInfoIndex;

										i.second.sourceGroupDataIndex = { ULONG_MAX, ULONG_MAX };
										i.second.sourceGroupInfoIndex = ULONG_MAX;
										i.second.sourceGroupDataIndexLocked = 0;
										soundCorr.otherMessages[groupPortSoundCorrMessageCode::sCMC_SHARED_WAVE_CLEAR_FAIL].push_back(newMessageEntry);
										soundCorr.successfulMatches--;
									}
								}
							}
							if (addEntryToMap)
							{
								waveIndecesToReferrerDataIndeces[copySourceWave].push_back(std::make_pair(copySourceDataIndex, copyDestDataIndex));
							}
						}
					}

					result = 1;
				}
				return result;
			}

			bool portGroupToGroup(brsarFile& sourceBrsar, unsigned long sourceCharFIDIn, unsigned long destinationCharFIDIn, std::ostream& contentsOutput, std::ostream& logOutput, groupPortSoundCorrespondence* soundMappingOut)
			{
				bool result = 0;

				if (sourceBrsar.contents.populated())
				{
					std::unordered_map<unsigned long, groupFileInfo>::iterator sourceGroupFileInfo = fighterIDToGroupInfoMap.find(sourceCharFIDIn);
					if (sourceGroupFileInfo != fighterIDToGroupInfoMap.end())
					{
						std::unordered_map<unsigned long, groupFileInfo>::iterator destinationGroupFileInfo = fighterIDToGroupInfoMap.find(destinationCharFIDIn);
						if (destinationGroupFileInfo != fighterIDToGroupInfoMap.end())
						{
							groupPortBundle sourceGroupBundle = getGroupPortBundle(sourceBrsar, sourceGroupFileInfo->second);
							groupPortBundle destinationGroupBundle = getGroupPortBundle(sourceBrsar, destinationGroupFileInfo->second);
							logOutput << "Copying Sounds from Group 0x" << lava::numToHexStringWithPadding(sourceGroupFileInfo->second.groupID, 0x03) << " to Group 0x" << lava::numToHexStringWithPadding(destinationGroupFileInfo->second.groupID, 0x03) << "...\n";

							if (sourceGroupBundle.populatedSuccessfully)
							{
								if (destinationGroupBundle.populatedSuccessfully)
								{
									groupPortSoundCorrespondence soundCorr = getGroupPortSoundCorr(sourceBrsar, sourceGroupBundle, destinationGroupBundle);

									if (soundCorr.successfulMatches != ULONG_MAX)
									{
										sourceGroupBundle.populateAllWavePackets(sourceBrsar.contents);
										destinationGroupBundle.populateAllWavePackets(sourceBrsar.contents);

										if (portCorrespondingSounds(soundCorr, sourceGroupBundle, destinationGroupBundle, 1, _maintainSharedWaves, 0))
										{
											result = destinationGroupBundle.exportAsSawnd(contentsOutput);
											if (soundMappingOut != nullptr)
											{
												*soundMappingOut = soundCorr;
											}
										}
										else
										{
											std::cerr << "Failed to port sounds, skipping export.\n";
										}

										logOutput << "Successfully found destinations for " << soundCorr.successfulMatches << " sound(s).\n";
										for (auto i : soundCorr.failedMatches)
										{
											std::vector<groupPortEntryMessageBundle>* messageVecPtr = &i.second;
											unsigned long soundEffectNameIndex = ULONG_MAX;
											if (i.first == groupPortSoundCorrMessageCode::sCMC_NO_MATCH || !messageVecPtr->empty())
											{
												switch (i.first)
												{
													case groupPortSoundCorrMessageCode::sCMC_NO_MATCH:
													{
														logOutput << "Couldn't find destinations for " << messageVecPtr->size() << " sound(s):\n";
														break;
													}
													case groupPortSoundCorrMessageCode::sCMC_PUSHED_BY_OTHER:
													{
														logOutput << "Non-overrides caused " << messageVecPtr->size() << " sound(s) to be omitted:\n";
														break;
													}
													case groupPortSoundCorrMessageCode::sCMC_PUSHED_BY_OVERRIDE:
													{
														logOutput << "Overrides caused " << messageVecPtr->size() << " sound(s) to be omitted:\n";
														break;
													}
													case groupPortSoundCorrMessageCode::sCMC_SHARED_WAVE_ADD_FAIL:
													{
														logOutput << messageVecPtr->size() << " sound(s) omitted because their destinations used shared wave entries:\n";
														break;
													}
													default:
													{
														logOutput << messageVecPtr->size() << " sound(s) experienced unforeseen errors:\n";
														break;
													}
												}
												for (std::size_t u = 0; u < messageVecPtr->size(); u++)
												{
													groupPortEntryMessageBundle* currMessage = &(*messageVecPtr)[u];

													std::pair<unsigned long, unsigned long>* sourceGroupDataIndeces = &currMessage->sourceGroupDataIndex;
													std::pair<unsigned long, unsigned long>* destGroupDataIndeces = &currMessage->destinationGroupDataIndex;
													soundEffectNameIndex = 0x194 + 
														sourceGroupBundle.groupEntriesFirstSoundIndex[sourceGroupDataIndeces->first] + sourceGroupDataIndeces->second;
													logOutput << "\tCollection #" << sourceGroupDataIndeces->first + 1
														<< ", Sound #" << sourceGroupDataIndeces->second << ": " << sourceBrsar.getSymbString(soundEffectNameIndex) 
														<< " (InfoIndex: 0x" <<	lava::numToHexStringWithPadding(currMessage->sourceGroupInfoIndex, 0x04) << ")";
													if (!currMessage->specialMessageText.empty())
													{
														logOutput << currMessage->specialMessageText;
													}
													logOutput << "\n";
												}
											}
										}
										for (auto i : soundCorr.otherMessages)
										{
											std::vector<groupPortEntryMessageBundle>* messageVecPtr = &i.second;
											unsigned long soundEffectNameIndex = ULONG_MAX;
											if (!messageVecPtr->empty())
											{
												switch (i.first)
												{
													case groupPortSoundCorrMessageCode::sCMC_SHARED_WAVE_OPEN_SPACE:
													{
														logOutput << "Because " << messageVecPtr->size() << " sound(s) now use shared wave entries, their original wave IDs are now free to use:\n";
														break;
													}
													case groupPortSoundCorrMessageCode::sCMC_SHARED_WAVE_CLEAR_FAIL:
													{
														logOutput << messageVecPtr->size() << " sound(s) that now use shared wave entries didn't have their own waves, so they didn't free up any IDs:\n";
														break;
													}
													default:
													{
														logOutput << "Something unexpected happened to " << messageVecPtr->size() << " sound(s):\n";
														break;
													}
												}
												for (std::size_t u = 0; u < messageVecPtr->size(); u++)
												{
													groupPortEntryMessageBundle* currMessage = &(*messageVecPtr)[u];

													std::pair<unsigned long, unsigned long>* sourceGroupDataIndeces = &currMessage->sourceGroupDataIndex;
													std::pair<unsigned long, unsigned long>* destGroupDataIndeces = &currMessage->destinationGroupDataIndex;
													soundEffectNameIndex = 0x194 +
														destinationGroupBundle.groupEntriesFirstSoundIndex[destGroupDataIndeces->first] + destGroupDataIndeces->second;
													logOutput << "\tCollection #" << destGroupDataIndeces->first + 1
														<< ", Sound #" << destGroupDataIndeces->second << ": " << sourceBrsar.getSymbString(soundEffectNameIndex)
														<< " (InfoIndex: 0x" << lava::numToHexStringWithPadding(currMessage->destinationGroupInfoIndex, 0x04) << ")";
													if (!currMessage->specialMessageText.empty())
													{
														logOutput << currMessage->specialMessageText;
													}
													logOutput << "\n";
												}
											}
										}
									}
								}
								else
								{
									logOutput << "Couldn't begin port, destination group couldn't be successfully parsed.\n";
								}
							}
							else
							{
								logOutput << "Couldn't begin port, source group couldn't be successfully parsed.\n";
							}
						}
					}
				}
				return result;
			}

		}
	}
}
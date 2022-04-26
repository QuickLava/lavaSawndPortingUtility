#include "lavaSawndz.h"

namespace lava
{
	unsigned long countCharInString(const std::string& stringIn, char searchIn)
	{
		unsigned long count = 0;
		for (std::size_t i = 0; i < stringIn.size(); i++)
		{
			count += stringIn[i] == searchIn;
		}
		return count;
	}
	std::size_t findNthInString(const std::string& stringIn, char searchIn, unsigned long n)
	{
		std::size_t result = SIZE_MAX;

		std::size_t stringLength = stringIn.size();
		if (n < stringLength)
		{
			std::size_t index = 0;
			unsigned long count = 0;
			while (index < stringLength && count < n)
			{
				if (stringIn[index] == searchIn)
				{
					count++;
				}
				if (count < n)
				{
					index++;
				}
			}

			if (count == n)
			{
				result = index;
			}

		}
		return result;
	}

	namespace brawl
	{
		namespace sawndz
		{
			bool groupFileInfo::fileIDAcceptable(unsigned long fileIDIn)
			{
				bool result = 0;
				std::size_t i = 0;
				while (!result && i < acceptableFileIDs.size())
				{
					result = acceptableFileIDs[i] == fileIDIn;
					i++;
				}
				return result;
			}
			std::unordered_map<unsigned long, groupFileInfo> fighterIDToGroupInfoMap = {
				{ LCFI_CAPTAIN_FALCON, { 20, {136, 755}}},
				{ LCFI_DEDEDE, {12, {560, 151}}},
				{ LCFI_DIDDY_KONG, {33, {143, 585}}},
				{ LCFI_DONKEY_KONG, {13, {500, 163}}},
				{ LCFI_FALCO, {28, {383, 144}}},
				{ LCFI_FOX, {17, {528, 621}}},
				{ LCFI_GANONDORF, { 29, {583, 296}}},
				{ LCFI_MR_GAME_AND_WATCH, { 27, {461, 526}}},
				{ LCFI_GIGA_BOWSER, { 22, {475, 171}}},
				{ LCFI_IKE, { 39, {617, 710}}},
				{ LCFI_KIRBY, { 16, {161, 457}}},
				{ LCFI_BOWSER, { 22, {484, 180}}},
				{ LCFI_LINK, { 9, {406, 186}}},
				{ LCFI_LUCARIO, { 38, {472, 620}}},
				{ LCFI_LUCAS, { 32, {377, 533}}},
				{ LCFI_LUIGI, { 19, {558, 254}}},
				{ LCFI_MARIO, { 8, {813, 260}}},
				{ LCFI_MARTH, { 26, {713, 537}}},
				{ LCFI_META_KNIGHT, { 11, {683, 486}}},
				{ LCFI_NANA, { 25, {596, 701}}},
				{ LCFI_NESS, { 21, {132, 187}}},
				{ LCFI_PEACH, { 23, {487, 751}}},
				{ LCFI_PIKACHU, { 18, {595, 250}}},
				{ LCFI_OLIMAR, { 31, {687, 395}}},
				{ LCFI_PIT, { 10, {479, 577}}},
				{ LCFI_IVYSAUR, { 37, {301, 446}}},
				{ LCFI_CHARIZARD, { 35, {542, 536}}},
				{ LCFI_POKEMON_TRAINER, { 34, {178, 543}}},
				{ LCFI_SQUIRTLE, { 36, {594, 248}}},
				{ LCFI_POPO, { 25, {339, 469}}},
				{ LCFI_JIGGLYPUFF, { 41, {609, 423}}},
				{ LCFI_ROB, { 40, {574, 720}}},
				{ LCFI_SAMUS, { 14, {255, 80}}},
				{ LCFI_SHEIK, { 24, {735, 442}}},
				{ LCFI_SNAKE, { 44, {261, 629}}},
				{ LCFI_SONIC, { 45, {130, 600}}},
				{ LCFI_ZERO_SUIT_SAMUS, { 14, {754, 630}}},
				{ LCFI_TOON_LINK, { 42, {712, 539}}},
				{ LCFI_WARIO, { 30, {680, 480}}},
				{ LCFI_WOLF, { 43, {378, 470}}},
				{ LCFI_YOSHI, { 15, {689, 573}}},
				{ LCFI_ZELDA, { 24, {481, 253}}},
			};

			std::unordered_map<std::string, std::string> soundEffectNameOverrides{};
			bool populateSoundEffectNameOverrides(std::string overrideFilePathIn)
			{
				bool result = 0;

				std::ifstream overridesIn(overrideFilePathIn, std::ios_base::in);
				if (overridesIn.is_open())
				{
					result = 1;
					std::string currentLine = "";
					std::string manipStr = "";
					while (std::getline(overridesIn, currentLine))
					{
						// Disregard the current line if it's empty, or is marked as a comment
						if (!currentLine.empty() && currentLine[0] != '#' && currentLine[0] != '/')
						{
							manipStr = "";
							bool inQuote = 0;
							bool doEscapeChar = 0;
							for (std::size_t i = 0; i < currentLine.size(); i++)
							{
								if (currentLine[i] == '\"' && !doEscapeChar)
								{
									inQuote = !inQuote;
								}
								else if (currentLine[i] == '\\')
								{
									doEscapeChar = 1;
								}
								else if (inQuote || !std::isspace(currentLine[i]))
								{
									doEscapeChar = 0;
									manipStr += currentLine[i];
								}
							}

							std::size_t nameIDDelimLoc = manipStr.find(',');
							if (nameIDDelimLoc != std::string::npos && nameIDDelimLoc < (manipStr.size() - 1))
							{
								auto emplaceResult = soundEffectNameOverrides.emplace(std::make_pair(manipStr.substr(0, nameIDDelimLoc), manipStr.substr(nameIDDelimLoc + 1, std::string::npos)));

								if (!emplaceResult.second)
								{
									std::cerr << "";
									emplaceResult.first->second = manipStr.substr(nameIDDelimLoc + 1, std::string::npos);
								}
							}
						}
					}
				}
				else
				{
					std::cerr << "Unable to parse Sound Name Overrides from \"" << overrideFilePathIn << "\": couldn't open file.\n";
				}
				return result;
			}
			std::string shuckSoundEffectName(std::string nameIn, std::unordered_map<std::string, std::string>::iterator* overrideUsedOut)
			{
				std::string result = "";

				std::size_t underscoreCount = 0;
				for (std::size_t i = 0; i < nameIn.size(); i++)
				{
					underscoreCount += nameIn[i] == '_';
				}

				if (underscoreCount >= 3)
				{
					if (nameIn.find("snd_vc_ouen") == 0)
					{
						result = "snd_vc_ouen";
					}
					else
					{
						std::unordered_map<std::string, std::string>::iterator findResult = soundEffectNameOverrides.find(nameIn);
						if (findResult != soundEffectNameOverrides.end())
						{
							result = findResult->second;
							if (overrideUsedOut != nullptr)
							{
								*overrideUsedOut = findResult;
							}
						}
						else
						{
							result = nameIn.substr(0, findNthInString(nameIn, '_', 2)) + nameIn.substr(findNthInString(nameIn, '_', 3));
							if (overrideUsedOut != nullptr)
							{
								*overrideUsedOut = soundEffectNameOverrides.end();
							}
						}
					}
				}
				result = lava::stringToUpper(result);
				return result;
			}

			/* Brawl Reference */

			brawlReference::brawlReference(unsigned long long valueIn)
			{
				isOffset = bool(valueIn & 0xFFFFFFFF00000000);
				address = valueIn & 0x00000000FFFFFFFF;
			}
			unsigned long brawlReference::getAddress(unsigned long relativeToIn)
			{
				unsigned long result = address;
				if (isOffset)
				{
					if (relativeToIn != ULONG_MAX)
					{
						result += relativeToIn;
					}
				}
				return result;
			}
			std::string brawlReference::getAddressString(unsigned long relativeToIn)
			{
				return lava::numToHexStringWithPadding(getAddress(relativeToIn), 8);
			}
			unsigned long long brawlReference::getHex()
			{
				unsigned long long result = address;
				if (isOffset)
				{
					result |= 0x0100000000000000;
				}
				return result;
			}

			bool brawlReferenceVector::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated() && (addressIn < bodyIn.body.size()))
				{
					std::size_t count = bodyIn.getLong(addressIn);
					refs.resize(count);
					unsigned long cursor = addressIn + 0x04;
					for (std::size_t i = 0; i < count; i++)
					{
						refs[i] = brawlReference(bodyIn.getLLong(cursor));
						cursor += 0x08;
					}
					result = 1;
				}

				return result;
			}
			std::vector<unsigned long> brawlReferenceVector::getHex()
			{
				std::vector<unsigned long> result{};

				result.push_back(refs.size());
				for (std::size_t i = 0; i < refs.size(); i++)
				{
					unsigned long long temp = refs[i].getHex();
					result.push_back(temp >> 0x20);
					result.push_back(temp);
				}

				return result;
			}
			bool brawlReferenceVector::exportContents(std::ostream& destinationStream)
			{
				bool result = 0;

				if (destinationStream.good())
				{
					std::vector<unsigned long> hexBufferVector = getHex();
					for (std::size_t i = 0; i < hexBufferVector.size(); i++)
					{
						lava::writeRawDataToStream(destinationStream, hexBufferVector[i]);
					}
					result = destinationStream.good();
				}

				return result;
			}

			/* Brawl Reference */



			/* BRSAR Symb Section */

			bool brsarSymbMaskEntry::populate(lava::byteArray& bodyIn, unsigned long addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;

					flags = bodyIn.getShort(addressIn);
					bit = bodyIn.getShort(addressIn + 0x02);
					leftID = bodyIn.getLong(address + 0x04);
					rightID = bodyIn.getLong(address + 0x08);
					stringID = bodyIn.getLong(address + 0x0C);
					index = bodyIn.getLong(address + 0x10);

					result = 1;
				}

				return result;
			}
			bool brsarSymbMaskHeader::populate(lava::byteArray& bodyIn, unsigned long addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;

					rootID = bodyIn.getLong(address);
					numEntries = bodyIn.getLong(address + 0x04);

					unsigned long cursor = address + 0x08;
					unsigned long cursorMax = cursor + (0x14 * numEntries);

					std::size_t i = 0;
					entries.resize(numEntries);

					while (cursor < cursorMax)
					{
						entries[i].populate(bodyIn, cursor);
						cursor += 0x14;
						i++;
					}


					result = 1;
				}

				return result;
			}

			bool brsarSymbSection::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;

					stringOffset = bodyIn.getLong(address + 0x08);
					soundsOffset = bodyIn.getLong(address + 0x0C);
					typesOffset = bodyIn.getLong(address + 0x10);
					groupsOffset = bodyIn.getLong(address + 0x14);
					banksOffset = bodyIn.getLong(address + 0x18);

					/*soundsMaskHeader.populate(bodyIn, address + 0x08 + soundsOffset);
					typesMaskHeader.populate(bodyIn, address + 0x08 + typesOffset);
					groupsMaskHeader.populate(bodyIn, address + 0x08 + groupsOffset);
					banksMaskHeader.populate(bodyIn, address + 0x08 + banksOffset);*/

					unsigned long cursor = address + 0x08 + stringOffset;
					stringOffsets.resize(bodyIn.getLong(cursor), ULONG_MAX);
					cursor += 0x04;
					for (std::size_t i = 0; i < stringOffsets.size(); i++)
					{
						stringOffsets[i] = bodyIn.getLong(cursor);
						cursor += 0x04;
					}

					result = 1;
				}
				return result;
			}

			/* BRSAR Symb Section */



			/* BRSAR Info Section */

			bool brsarInfoSoundEntry::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;

					stringID = bodyIn.getLong(address);
					fileID = bodyIn.getLong(address + 0x04);
					playerID = bodyIn.getLong(address + 0x08);
					param3DRefOffset = brawlReference(address + 0x0C);
					volume = bodyIn.getChar(address + 0x14);
					playerPriority = bodyIn.getChar(address + 0x15);
					soundType = bodyIn.getChar(address + 0x16);
					remoteFilter = bodyIn.getChar(address + 0x17);
					soundInfoRef = brawlReference(address + 0x18);
					userParam1 = bodyIn.getLong(address + 0x20);
					userParam2 = bodyIn.getLong(address + 0x24);
					panMode = bodyIn.getChar(address + 0x28);
					panCurve = bodyIn.getChar(address + 0x29);
					actorPlayerID = bodyIn.getChar(address + 0x2A);
					reserved = bodyIn.getChar(address + 0x2B);
				}

				return result;
			}

			bool brsarInfoFileEntry::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;

					groupID = bodyIn.getLong(address);
					index = bodyIn.getLong(address + 0x04);
				}

				return result;
			}
			bool brsarInfoFileHeader::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;

					headerLength = bodyIn.getLong(address);
					dataLength = bodyIn.getLong(address + 0x04);
					entryNumber = bodyIn.getLong(address + 0x08);
					stringOffset = brawlReference(bodyIn.getLLong(address + 0x0C));
					listOffset = brawlReference(bodyIn.getLLong(address + 0x14));
				}

				return result;
			}

			bool brsarInfoGroupEntry::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;

					fileID = bodyIn.getLong(address);
					headerOffset = bodyIn.getLong(address + 0x04);
					headerLength = bodyIn.getLong(address + 0x08);
					dataOffset = bodyIn.getLong(address + 0x0C);
					dataLength = bodyIn.getLong(address + 0x10);
					reserved = bodyIn.getLong(address + 0x14);

					result = 1;
				}

				return result;
			}
			bool brsarInfoGroupEntry::exportContents(std::ostream& destinationStream)
			{
				bool result = 0;
				if (destinationStream.good())
				{
					lava::writeRawDataToStream(destinationStream, fileID);
					lava::writeRawDataToStream(destinationStream, headerOffset);
					lava::writeRawDataToStream(destinationStream, headerLength);
					lava::writeRawDataToStream(destinationStream, dataOffset);
					lava::writeRawDataToStream(destinationStream, dataLength);
					lava::writeRawDataToStream(destinationStream, reserved);

					result = destinationStream.good();
				}
				return result;
			}
			bool brsarInfoGroupHeader::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;

					stringID = bodyIn.getLong(address);
					entryNum = bodyIn.getLong(address + 0x04);
					extFilePathRef = brawlReference(bodyIn.getLLong(address + 0x08));
					headerOffset = bodyIn.getLong(address + 0x10);
					headerLength = bodyIn.getLong(address + 0x14);
					waveDataOffset = bodyIn.getLong(address + 0x18);
					waveDataLength = bodyIn.getLong(address + 0x1C);
					listOffset = brawlReference(bodyIn.getLLong(address + 0x20));

					result = 1;
				}

				return result;
			}
			bool brsarInfoGroupHeader::exportContents(std::ostream& destinationStream)
			{
				bool result = 0;
				if (destinationStream.good())
				{
					lava::writeRawDataToStream(destinationStream, stringID);
					lava::writeRawDataToStream(destinationStream, entryNum);
					lava::writeRawDataToStream(destinationStream, extFilePathRef.getHex());
					lava::writeRawDataToStream(destinationStream, headerOffset);
					lava::writeRawDataToStream(destinationStream, headerLength);
					lava::writeRawDataToStream(destinationStream, waveDataOffset);
					lava::writeRawDataToStream(destinationStream, waveDataLength);
					lava::writeRawDataToStream(destinationStream, listOffset.getHex());

					result = destinationStream.good();
				}
				return result;
			}

			std::vector<brsarInfoFileHeader*> brsarInfoSection::findFilesWithGroupID(lava::byteArray& bodyIn, unsigned long groupIDIn)
			{
				std::vector<brsarInfoFileHeader*> result{};

				if (bodyIn.populated())
				{
					brsarInfoFileHeader* currHeader = nullptr;
					for (std::size_t i = 0; i < fileHeaders.size(); i++)
					{
						currHeader = &fileHeaders[i];
						for (std::size_t u = 0; u < currHeader->entries.size(); u++)
						{
							if (currHeader->entries[u].groupID == groupIDIn)
							{
								result.push_back(currHeader);
							}
						}
					}
				}

				return result;
			}

			bool brsarInfoSection::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;

					soundsSectionReference = brawlReference(bodyIn.getLLong(address + 0x08));
					banksSectionReference = brawlReference(bodyIn.getLLong(address + 0x10));
					playerSectionReference = brawlReference(bodyIn.getLLong(address + 0x18));
					filesSectionReference = brawlReference(bodyIn.getLLong(address + 0x20));
					groupsSectionReference = brawlReference(bodyIn.getLLong(address + 0x28));

					soundsSection.populate(bodyIn, soundsSectionReference.getAddress(address + 0x08));
					banksSection.populate(bodyIn, banksSectionReference.getAddress(address + 0x08));
					playerSection.populate(bodyIn, playerSectionReference.getAddress(address + 0x08));
					filesSection.populate(bodyIn, filesSectionReference.getAddress(address + 0x08));
					groupsSection.populate(bodyIn, groupsSectionReference.getAddress(address + 0x08));

					soundEntries.resize(soundsSection.refs.size());
					for (std::size_t i = 0; i < soundsSection.refs.size(); i++)
					{
						soundEntries[i].populate(bodyIn, soundsSection.refs[i].getAddress(address + 0x08));
					}

					fileHeaders.resize(filesSection.refs.size());
					brsarInfoFileHeader* currHeader = nullptr;
					for (std::size_t i = 0; i < filesSection.refs.size(); i++)
					{
						currHeader = &fileHeaders[i];
						currHeader->populate(bodyIn, filesSection.refs[i].getAddress(address + 0x08));
						brawlReferenceVector temp;
						temp.populate(bodyIn, currHeader->listOffset.getAddress(address + 0x08));

						for (std::size_t u = 0; u < temp.refs.size(); u++)
						{
							currHeader->entries.push_back(brsarInfoFileEntry());
							currHeader->entries.back().populate(bodyIn, temp.refs[u].getAddress(address + 0x08));
						}
					}

					result = 1;
				}

				return result;
			}

			/* BRSAR Info Section */



			/* BRSAR File Section */

			/* RWSD */
			void dataInfo::copyOverDataInfoProperties(const dataInfo& sourceInfo)
			{
				address = _NOT_IN_FILE_ADDRESS;

				wsdPitch = sourceInfo.wsdPitch;
				wsdPan = sourceInfo.wsdPan;
				wsdSurroundPan = sourceInfo.wsdSurroundPan;
				wsdFxSendA = sourceInfo.wsdFxSendA;
				wsdFxSendB = sourceInfo.wsdFxSendB;
				wsdFxSendC = sourceInfo.wsdFxSendC;
				wsdMainSend = sourceInfo.wsdMainSend;
				wsdPad1 = sourceInfo.wsdPad1;
				wsdPad2 = sourceInfo.wsdPad2;

				ttPosition = sourceInfo.ttPosition;
				ttLength = sourceInfo.ttLength;
				ttNoteIndex = sourceInfo.ttNoteIndex;

				ntAttack = sourceInfo.ntAttack;
				ntDecay = sourceInfo.ntDecay;
				ntSustain = sourceInfo.ntSustain;
				ntRelease = sourceInfo.ntRelease;
				ntHold = sourceInfo.ntHold;
				ntPad1 = sourceInfo.ntPad1;
				ntPad2 = sourceInfo.ntPad2;
				ntPad3 = sourceInfo.ntPad3;
				ntOriginalKey = sourceInfo.ntOriginalKey;
				ntVolume = sourceInfo.ntVolume;
				ntPan = sourceInfo.ntPan;
				ntSurroundPan = sourceInfo.ntSurroundPan;
				ntPitch = sourceInfo.ntPitch;
			}

			bool dataInfo::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;
					wsdInfo = brawlReference(bodyIn.getLLong(address));
					trackTable = brawlReference(bodyIn.getLLong(address + 0x08));
					noteTable = brawlReference(bodyIn.getLLong(address + 0x10));

					wsdPitch = bodyIn.getFloat(address + 0x18);
					wsdPan = bodyIn.getChar(address + 0x1C);
					wsdSurroundPan = bodyIn.getChar(address + 0x1D);
					wsdFxSendA = bodyIn.getChar(address + 0x1E);
					wsdFxSendB = bodyIn.getChar(address + 0x1F);
					wsdFxSendC = bodyIn.getChar(address + 0x20);
					wsdMainSend = bodyIn.getChar(address + 0x21);
					wsdPad1 = bodyIn.getChar(address + 0x22);
					wsdPad2 = bodyIn.getChar(address + 0x23);
					wsdGraphEnvTableRef = brawlReference(bodyIn.getLLong(address + 0x24));
					wsdRandomizerTableRef = brawlReference(bodyIn.getLLong(address + 0x2C));
					wsdPadding = bodyIn.getLong(address + 0x34);

					ttReferenceList1.populate(bodyIn, address + 0x38);
					ttIntermediateReference = brawlReference(bodyIn.getLLong(address + 0x44));
					ttReferenceList2.populate(bodyIn, address + 0x4C);
					ttPosition = bodyIn.getFloat(address + 0x58);
					ttLength = bodyIn.getFloat(address + 0x5C);
					ttNoteIndex = bodyIn.getLong(address + 0x60);
					ttReserved = bodyIn.getLong(address + 0x64);

					ntReferenceList.populate(bodyIn, address + 0x68);
					ntWaveIndex = bodyIn.getLong(address + 0x74);
					ntAttack = bodyIn.getChar(address + 0x78);
					ntDecay = bodyIn.getChar(address + 0x79);
					ntSustain = bodyIn.getChar(address + 0x7A);
					ntRelease = bodyIn.getChar(address + 0x7B);
					ntHold = bodyIn.getChar(address + 0x7C);
					ntPad1 = bodyIn.getChar(address + 0x7D);
					ntPad2 = bodyIn.getChar(address + 0x7E);
					ntPad3 = bodyIn.getChar(address + 0x7F);
					ntOriginalKey = bodyIn.getChar(address + 0x80);
					ntVolume = bodyIn.getChar(address + 0x81);
					ntPan = bodyIn.getChar(address + 0x82);
					ntSurroundPan = bodyIn.getChar(address + 0x83);
					ntPitch = bodyIn.getFloat(address + 0x84);
					ntIfoTableRef = brawlReference(bodyIn.getLLong(address + 0x88));
					ntGraphEnvTableRef = brawlReference(bodyIn.getLLong(address + 0x90));
					ntRandomizerTableRef = brawlReference(bodyIn.getLLong(address + 0x98));
					ntReserved = bodyIn.getLong(address + 0xA0);

					result = 1;
				}

				return result;
			}
			bool dataInfo::exportContents(std::ostream& destinationStream)
			{
				bool result = 0;
				if (destinationStream.good())
				{
					lava::writeRawDataToStream(destinationStream, wsdInfo.getHex());
					lava::writeRawDataToStream(destinationStream, trackTable.getHex());
					lava::writeRawDataToStream(destinationStream, noteTable.getHex());
					lava::writeRawDataToStream(destinationStream, wsdPitch);
					lava::writeRawDataToStream(destinationStream, wsdPan);
					lava::writeRawDataToStream(destinationStream, wsdSurroundPan);
					lava::writeRawDataToStream(destinationStream, wsdFxSendA);
					lava::writeRawDataToStream(destinationStream, wsdFxSendB);
					lava::writeRawDataToStream(destinationStream, wsdFxSendC);
					lava::writeRawDataToStream(destinationStream, wsdMainSend);
					lava::writeRawDataToStream(destinationStream, wsdPad1);
					lava::writeRawDataToStream(destinationStream, wsdPad2);
					lava::writeRawDataToStream(destinationStream, wsdGraphEnvTableRef.getHex());
					lava::writeRawDataToStream(destinationStream, wsdRandomizerTableRef.getHex());
					lava::writeRawDataToStream(destinationStream, wsdPadding);

					ttReferenceList1.exportContents(destinationStream);
					lava::writeRawDataToStream(destinationStream, ttIntermediateReference.getHex());
					ttReferenceList2.exportContents(destinationStream);
					lava::writeRawDataToStream(destinationStream, ttPosition);
					lava::writeRawDataToStream(destinationStream, ttLength);
					lava::writeRawDataToStream(destinationStream, ttNoteIndex);
					lava::writeRawDataToStream(destinationStream, ttReserved);

					ntReferenceList.exportContents(destinationStream);
					lava::writeRawDataToStream(destinationStream, ntWaveIndex);
					lava::writeRawDataToStream(destinationStream, ntAttack);
					lava::writeRawDataToStream(destinationStream, ntDecay);
					lava::writeRawDataToStream(destinationStream, ntSustain);
					lava::writeRawDataToStream(destinationStream, ntRelease);
					lava::writeRawDataToStream(destinationStream, ntHold);
					lava::writeRawDataToStream(destinationStream, ntPad1);
					lava::writeRawDataToStream(destinationStream, ntPad2);
					lava::writeRawDataToStream(destinationStream, ntPad3);
					lava::writeRawDataToStream(destinationStream, ntOriginalKey);
					lava::writeRawDataToStream(destinationStream, ntVolume);
					lava::writeRawDataToStream(destinationStream, ntPan);
					lava::writeRawDataToStream(destinationStream, ntSurroundPan);
					lava::writeRawDataToStream(destinationStream, ntPitch);
					lava::writeRawDataToStream(destinationStream, ntIfoTableRef.getHex());
					lava::writeRawDataToStream(destinationStream, ntGraphEnvTableRef.getHex());
					lava::writeRawDataToStream(destinationStream, ntRandomizerTableRef.getHex());
					lava::writeRawDataToStream(destinationStream, ntReserved);

					result = destinationStream.good();
				}
				return result;
			}
			bool rwsdDataSection::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;
					length = bodyIn.getLong(addressIn + 0x04);
					entryReferences.populate(bodyIn, addressIn + 0x08);
					std::vector<brawlReference>* refVecPtr = &entryReferences.refs;
					std::size_t entryCount = refVecPtr->size();
					unsigned long entryTargetAddress = ULONG_MAX;
					entries.resize(entryCount);
					for (std::size_t i = 0; i < entryCount; i++)
					{
						entryTargetAddress = address + 0x08 + refVecPtr->at(i).getAddress();
						if (bodyIn.getLong(entryTargetAddress) == 0x01000000)
						{
							entries[i].populate(bodyIn, entryTargetAddress);
						}
						else
						{
							std::cerr << "Skipping Data Entry @ " << entryTargetAddress << "\n";
						}
					}

					result = 1;
				}

				return result;
			}
			bool rwsdDataSection::exportContents(std::ostream& destinationStream)
			{
				bool result = 0;
				if (destinationStream.good())
				{
					unsigned long initialStreamPos = destinationStream.tellp();
					destinationStream << "DATA";
					lava::writeRawDataToStream(destinationStream, length);
					entryReferences.exportContents(destinationStream);
					for (std::size_t i = 0; i < entries.size(); i++)
					{
						entries[i].exportContents(destinationStream);
					}
					unsigned long finalStreamPos = destinationStream.tellp();
					unsigned long lengthOfExport = finalStreamPos - initialStreamPos;
					if (lengthOfExport != length)
					{
						if (lengthOfExport < length)
						{
							std::vector<char> padding{};
							padding.resize(length - lengthOfExport, 0x00);
							destinationStream.write(padding.data(), padding.size());
						}
					}
					result = destinationStream.good();
				}
				return result;
			}

			bool wavePacket::populate(lava::byteArray& bodyIn, unsigned long addressIn, unsigned long lengthIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;
					length = lengthIn;

					std::size_t numGotten = SIZE_MAX;
					body = bodyIn.getBytes(lengthIn, addressIn, numGotten);
					result = numGotten == lengthIn;
					populated = result;
					if (!result)
					{
						length = ULONG_MAX;
						address = ULONG_MAX;
						body.clear();
					}
				}

				return result;
			}

			unsigned long waveInfo::getLengthInBytes() const
			{
				unsigned long result = ULONG_MAX;

				if (address != ULONG_MAX)
				{
					auto divResult = ldiv(nibbles, 2);
					result = divResult.quot + divResult.rem;
				}

				return result;
			}
			void waveInfo::copyOverWaveInfoProperties(const waveInfo& sourceInfo)
			{
				address = _NOT_IN_FILE_ADDRESS;

				encoding = sourceInfo.encoding;
				looped = sourceInfo.looped;
				channels = sourceInfo.channels;
				sampleRate24 = sourceInfo.sampleRate24;
				sampleRate = sourceInfo.sampleRate;
				//dataLocationType = sourceInfo.dataLocationType;
				pad = sourceInfo.pad;
				loopStartSample = sourceInfo.loopStartSample;
				nibbles = sourceInfo.nibbles;
				channelInfoTableOffset = sourceInfo.channelInfoTableOffset;
				//dataLocation = sourceInfo.dataLocation;
				reserved = sourceInfo.reserved;

				channelInfoTableLength = sourceInfo.channelInfoTableLength;
				channelInfoTable = sourceInfo.channelInfoTable;
			}
			bool waveInfo::populate(lava::byteArray& bodyIn, unsigned long addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;

					encoding = bodyIn.getChar(address);
					looped = bodyIn.getChar(address + 0x01);
					channels = bodyIn.getChar(address + 0x02);
					sampleRate24 = bodyIn.getChar(address + 0x03);
					sampleRate = bodyIn.getShort(address + 0x04);
					dataLocationType = bodyIn.getChar(address + 0x06);
					pad = bodyIn.getChar(address + 0x07);
					loopStartSample = bodyIn.getLong(address + 0x08);
					nibbles = bodyIn.getLong(address + 0x0C);
					channelInfoTableOffset = bodyIn.getLong(address + 0x10);
					dataLocation = bodyIn.getLong(address + 0x14);
					reserved = bodyIn.getLong(address + 0x18);

					channelInfoTableLength = channels * (0x20 + (encoding == 2 ? 0x30 : 0x00));
					for (unsigned long cursor = 0x0; cursor < channelInfoTableLength; cursor += 0x04)
					{
						channelInfoTable.push_back(bodyIn.getLong(address + channelInfoTableOffset + cursor));
					}
					result = 1;
				}

				return result;
			}
			bool waveInfo::exportContents(std::ostream& destinationStream)
			{
				bool result = 0;
				if (destinationStream.good())
				{
					lava::writeRawDataToStream(destinationStream, encoding);
					lava::writeRawDataToStream(destinationStream, looped);
					lava::writeRawDataToStream(destinationStream, channels);
					lava::writeRawDataToStream(destinationStream, sampleRate24);
					lava::writeRawDataToStream(destinationStream, sampleRate);
					lava::writeRawDataToStream(destinationStream, dataLocationType);
					lava::writeRawDataToStream(destinationStream, pad);
					lava::writeRawDataToStream(destinationStream, loopStartSample);
					lava::writeRawDataToStream(destinationStream, nibbles);
					lava::writeRawDataToStream(destinationStream, channelInfoTableOffset);
					lava::writeRawDataToStream(destinationStream, dataLocation);
					lava::writeRawDataToStream(destinationStream, reserved);

					for (unsigned long i = 0x0; i < channelInfoTable.size(); i++)
					{
						lava::writeRawDataToStream(destinationStream, channelInfoTable[i]);
					}

					result = destinationStream.good();
				}
				return result;
			}

			void rwsdWaveSection::pushEntry(const waveInfo& entryIn)
			{
				if (!entryOffsets.empty())
				{
					for (unsigned long i = 0; i < entryOffsets.size(); i++)
					{
						entryOffsets[i] += 0x04;
					}
					entryOffsets.push_back(entryOffsets.back() + 0x1C + entryIn.channelInfoTableLength);
				}
				else
				{
					entryOffsets.push_back(0x10);
				}

				entries.push_back(entryIn);
				entries.back().address = _NOT_IN_FILE_ADDRESS;
				if (entries.size() > 1)
				{
					waveInfo* formerFinalEntry = &entries[entries.size() - 2];
					unsigned long newDataLocation = formerFinalEntry->dataLocation + formerFinalEntry->getLengthInBytes();
					if (formerFinalEntry->packetContents.populated)
					{
						newDataLocation += formerFinalEntry->packetContents.paddingLength;
					}
					entries.back().dataLocation = newDataLocation;
				}
				else
				{
					entries.back().dataLocation = 0x00;
				}
				
				length += 0x04 + 0x1C + entryIn.channelInfoTableLength;
			}

			bool rwsdWaveSection::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;

					length = bodyIn.getLong(addressIn + 0x04);
					unsigned long entryCount = bodyIn.getLong(addressIn + 0x08);

					for (unsigned long cursor = 0x0; cursor < (entryCount * 4); cursor += 0x04)
					{
						entryOffsets.push_back(bodyIn.getLong(address + 0x0C + cursor));
						entries.push_back(waveInfo());
						entries.back().populate(bodyIn, address + entryOffsets.back());
					}

					result = 1;
				}

				return result;
			}
			bool rwsdWaveSection::exportContents(std::ostream& destinationStream)
			{
				bool result = 0;
				if (destinationStream.good())
				{
					unsigned long initialStreamPos = destinationStream.tellp();

					destinationStream << "WAVE";
					lava::writeRawDataToStream(destinationStream, length);
					lava::writeRawDataToStream<unsigned long>(destinationStream, entries.size());

					for (unsigned long i = 0x0; i < entryOffsets.size(); i++)
					{
						lava::writeRawDataToStream(destinationStream, entryOffsets[i]);
					}
					for (unsigned long i = 0x0; i < entries.size(); i++)
					{
						entries[i].exportContents(destinationStream);
					}

					unsigned long finalStreamPos = destinationStream.tellp();
					unsigned long lengthOfExport = finalStreamPos - initialStreamPos;
					if (lengthOfExport != length)
					{
						if (lengthOfExport < length)
						{
							std::vector<char> padding{};
							padding.resize(length - lengthOfExport, 0x00);
							destinationStream.write(padding.data(), padding.size());
						}
					}

					result = destinationStream.good();
				}
				return result;
			}

			bool rwsdHeader::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;
					endianType = bodyIn.getShort(addressIn + 0x04);
					version = bodyIn.getShort(addressIn + 0x06);
					headerLength = bodyIn.getLong(addressIn + 0x08);
					entriesOffset = bodyIn.getShort(addressIn + 0x0C);
					entriesCount = bodyIn.getShort(addressIn + 0x0E);

					dataOffset = bodyIn.getLong(addressIn + 0x10);
dataLength = bodyIn.getLong(addressIn + 0x14);
waveOffset = bodyIn.getLong(addressIn + 0x18);
waveLength = bodyIn.getLong(addressIn + 0x1C);
result = 1;
				}

				return result;
			}
			bool rwsdHeader::exportContents(std::ostream& destinationStream)
			{
				bool result = 0;
				if (destinationStream.good())
				{
					destinationStream << "RWSD";
					lava::writeRawDataToStream(destinationStream, endianType);
					lava::writeRawDataToStream(destinationStream, version);
					lava::writeRawDataToStream(destinationStream, headerLength);
					lava::writeRawDataToStream(destinationStream, entriesOffset);
					lava::writeRawDataToStream(destinationStream, entriesCount);
					lava::writeRawDataToStream(destinationStream, dataOffset);
					lava::writeRawDataToStream(destinationStream, dataLength);
					lava::writeRawDataToStream(destinationStream, waveOffset);
					lava::writeRawDataToStream(destinationStream, waveLength);

					result = destinationStream.good();
				}
				return result;
			}

			bool rwsd::hasExclusiveWave(unsigned long dataSectionIndex)
			{
				bool result = 1;
				if (dataSectionIndex < dataSection.entries.size())
				{
					dataInfo* dataInfoEntry = &dataSection.entries[dataSectionIndex];
					unsigned long dataWaveIndex = dataInfoEntry->ntWaveIndex;

					std::size_t i = 0;
					while (result && i < dataSection.entries.size())
					{
						if (i != dataSectionIndex)
						{
							result = dataWaveIndex != dataSection.entries[i].ntWaveIndex;
						}
						i++;
					}
				}
				return result;
			}
			bool rwsd::isFirstToUseWave(unsigned long dataSectionIndex)
			{
				bool result = 1;
				if (dataSectionIndex < dataSection.entries.size())
				{
					dataInfo* dataInfoEntry = &dataSection.entries[dataSectionIndex];
					unsigned long dataWaveIndex = dataInfoEntry->ntWaveIndex;

					std::size_t i = 0;
					while (result && i < dataSectionIndex)
					{
						result = dataWaveIndex != dataSection.entries[i].ntWaveIndex;
						i++;
					}
				}
				return result;
			}
			waveInfo* rwsd::getWaveInfoAssociatedWithDataInfo(unsigned long dataSectionIndex)
			{
				waveInfo* result = nullptr;

				if (dataSectionIndex < dataSection.entries.size())
				{
					unsigned long waveIndex = dataSection.entries[dataSectionIndex].ntWaveIndex;
					if (waveIndex < waveSection.entries.size())
					{
						result = &waveSection.entries[waveIndex];
					}
				}

				return result;
			}
			bool rwsd::populateWavePacket(lava::byteArray& bodyIn, unsigned long parentGroupWaveDataAddress, unsigned long collectionDataOffset, unsigned long dataSectionIndex)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					waveInfo* associatedWaveInfo = getWaveInfoAssociatedWithDataInfo(dataSectionIndex);
					if (associatedWaveInfo != nullptr && !associatedWaveInfo->packetContents.populated)
					{
						dataInfo* associatedDataInfo = &dataSection.entries[dataSectionIndex];
						unsigned long waveDataStartLocation = parentGroupWaveDataAddress + collectionDataOffset + associatedWaveInfo->dataLocation;
						unsigned long waveDataLength = associatedWaveInfo->getLengthInBytes();
						if (waveDataStartLocation + waveDataLength < bodyIn.body.size())
						{
							result = associatedWaveInfo->packetContents.populate(bodyIn, waveDataStartLocation, waveDataLength);
							if (associatedDataInfo->ntWaveIndex < (waveSection.entries.size() - 1))
							{
								unsigned long nextEntryDataLocation = waveSection.entries[associatedDataInfo->ntWaveIndex + 1].dataLocation;
								unsigned long currentEntryWaveInfoEnd = associatedWaveInfo->dataLocation + associatedWaveInfo->getLengthInBytes();
								if (nextEntryDataLocation >= currentEntryWaveInfoEnd)
								{
									associatedWaveInfo->packetContents.paddingLength = nextEntryDataLocation - currentEntryWaveInfoEnd;
								}
								else
								{
									unsigned long overlapSize = currentEntryWaveInfoEnd - nextEntryDataLocation;
									associatedWaveInfo->nibbles -= overlapSize * 2;
									associatedWaveInfo->packetContents.length -= overlapSize;
									associatedWaveInfo->packetContents.body.resize(associatedWaveInfo->packetContents.body.size() - overlapSize);
									associatedWaveInfo->packetContents.paddingLength = 0x00;
									std::cout << "Warning: Wave Packet Truncation performed! Lost 0x" << lava::numToHexStringWithPadding(overlapSize, 0x02) << " byte(s) as a result.\n";
								}
							}
							else
							{
								associatedWaveInfo->packetContents.paddingLength = (0x10 - (associatedWaveInfo->packetContents.length % 0x10));
							}
						}
					}
				}
				
				return result;
			}


			signed long rwsd::overwriteSound(unsigned long dataSectionIndex, const dataInfo& dataInfoIn, const waveInfo& waveInfoIn, bool allowSharedWaveSplit)
			{
				signed long changeInWaveDataSize = LONG_MAX;

				if (waveInfoIn.packetContents.populated)
				{
					dataInfo* dataEntryPtr = &dataSection.entries[dataSectionIndex];
					dataEntryPtr->copyOverDataInfoProperties(dataInfoIn);

					if (isFirstToUseWave(dataSectionIndex))
					{
						waveInfo* associatedWaveInfo = getWaveInfoAssociatedWithDataInfo(dataSectionIndex);
						if (associatedWaveInfo != nullptr && associatedWaveInfo->packetContents.populated)
						{
							signed long differenceInWaveLength = signed long(waveInfoIn.getLengthInBytes()) - signed long(associatedWaveInfo->getLengthInBytes());
							signed long differenceInPaddingLength = signed long(waveInfoIn.packetContents.paddingLength) - signed long(associatedWaveInfo->packetContents.paddingLength);

							changeInWaveDataSize = differenceInWaveLength + differenceInPaddingLength;
							associatedWaveInfo->copyOverWaveInfoProperties(waveInfoIn);
							associatedWaveInfo->packetContents = waveInfoIn.packetContents;
							associatedWaveInfo->packetContents.address = _NOT_IN_FILE_ADDRESS;
							
							for (std::size_t i = dataEntryPtr->ntWaveIndex + 1; i < waveSection.entries.size(); i++)
							{
								waveSection.entries[i].address = _NOT_IN_FILE_ADDRESS;
								waveSection.entries[i].dataLocation += changeInWaveDataSize;
							}
						}
					}
					else
					{
						if (allowSharedWaveSplit)
						{
							std::cout << "Inserting new Wave!\n";
							waveSection.pushEntry(waveInfoIn);
							dataEntryPtr->ntWaveIndex = waveSection.entries.size() - 1;
							header.waveLength += 0x04 + 0x1C + waveInfoIn.channelInfoTableLength;
							header.headerLength += 0x04 + 0x1C + waveInfoIn.channelInfoTableLength;
							changeInWaveDataSize = (signed long)waveSection.entries.back().getLengthInBytes();
							changeInWaveDataSize += (signed long)waveSection.entries.back().packetContents.paddingLength;
						}
						else
						{
							changeInWaveDataSize = _OVERWRITE_SOUND_SHARED_WAVE_RETURN_CODE;
						}
					}
				}
				return changeInWaveDataSize;
			}
			signed long rwsd::shareWaveTargetBetweenDataEntries(unsigned long recipientDataSectionIndex, unsigned long donorDataSectionIndex, const dataInfo* dataInfoIn, bool voidOutExistingSound)
			{
				signed long changeInWaveDataSize = LONG_MAX;

				if (recipientDataSectionIndex < dataSection.entries.size())
				{
					if (donorDataSectionIndex < dataSection.entries.size())
					{
						dataInfo* dataEntryPtr = &dataSection.entries[recipientDataSectionIndex];
						if (dataInfoIn != nullptr)
						{
							dataEntryPtr->copyOverDataInfoProperties(*dataInfoIn);
						}
						if (voidOutExistingSound)
						{
							if (isFirstToUseWave(recipientDataSectionIndex))
							{
								waveInfo* associatedWaveInfo = getWaveInfoAssociatedWithDataInfo(recipientDataSectionIndex);
								if (associatedWaveInfo != nullptr && associatedWaveInfo->packetContents.populated)
								{
									// Create an empty wavePacket.
									wavePacket tempEmptyPacket;
									tempEmptyPacket.body = std::vector<unsigned char>(_EMPTY_SOUND_SOUND_LENGTH, 0x00);
									tempEmptyPacket.length = _EMPTY_SOUND_SOUND_LENGTH;
									tempEmptyPacket.paddingLength = _EMPTY_SOUND_TOTAL_LENGTH - _EMPTY_SOUND_SOUND_LENGTH;
									tempEmptyPacket.populated = 1;
									tempEmptyPacket.address = _NOT_IN_FILE_ADDRESS;

									// Record the difference in size between the new empty packet and the old one.
									signed long differenceInWaveLength = signed long(tempEmptyPacket.length) - signed long(associatedWaveInfo->getLengthInBytes());
									signed long differenceInPaddingLength = signed long(tempEmptyPacket.paddingLength) - signed long(associatedWaveInfo->packetContents.paddingLength);
									changeInWaveDataSize = differenceInWaveLength + differenceInPaddingLength;

									// Overwrite old wavePacket with new one.
									associatedWaveInfo->address = _NOT_IN_FILE_ADDRESS;
									associatedWaveInfo->nibbles = _EMPTY_SOUND_SOUND_LENGTH * 2;
									associatedWaveInfo->packetContents = tempEmptyPacket;

									// Propogate change in size through all other waves.
									for (std::size_t i = dataEntryPtr->ntWaveIndex + 1; i < waveSection.entries.size(); i++)
									{
										waveSection.entries[i].address = _NOT_IN_FILE_ADDRESS;
										waveSection.entries[i].dataLocation += changeInWaveDataSize;
									}
								}
							}
							else
							{
								changeInWaveDataSize = _OVERWRITE_SOUND_SHARED_WAVE_RETURN_CODE;
							}
						}
						else
						{
							changeInWaveDataSize = 0;
						}
						dataInfo* donorDataEntryPtr = &dataSection.entries[donorDataSectionIndex];
						dataEntryPtr->ntWaveIndex = donorDataEntryPtr->ntWaveIndex;
					}
				}
				
				return changeInWaveDataSize;
			}

			bool rwsd::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					if (bodyIn.getLong(addressIn) == _HEX_TAG_RWSD)
					{
						address = addressIn;

						if (header.populate(bodyIn, address))
						{
							result = dataSection.populate(bodyIn, address + header.dataOffset);
							result &= waveSection.populate(bodyIn, address + header.waveOffset);
						}
					}
				}

				return result;
			}
			bool rwsd::exportContents(std::ostream& destinationStream)
			{
				bool result = 0;

				if (destinationStream.good())
				{
					destinationStream << std::hex;
					result &= header.exportContents(destinationStream);
					result &= dataSection.exportContents(destinationStream);
					result &= waveSection.exportContents(destinationStream);
				}

				return result;
			}
			/* RWSD */

			bool brsarFileSection::populate(lava::byteArray& bodyIn, std::size_t addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;

					length = bodyIn.getLong(address + 0x04);

					result = 1;
				}

				return result;
			}

			/* BRSAR File Section */



			/* BRSAR */

			bool brsarFile::init(std::string filePathIn)
			{
				bool result = 0;
				std::ifstream fileIn;
				fileIn.open(filePathIn, std::ios_base::in | std::ios_base::binary);
				if (fileIn.is_open())
				{
					result = 1;
					std::cout << "Parsing \"" << filePathIn << "\"...\n";
					contents.populate(fileIn);
					std::size_t tempNumber = 0;


					result &= symbSection.populate(contents, contents.getLong(0x10));
					result &= infoSection.populate(contents, contents.getLong(0x18));
					result &= fileSection.populate(contents, contents.getLong(0x20));
				}
				return result;
			}

			std::string brsarFile::getSymbString(unsigned long indexIn)
			{
				std::string result = "";

				if (contents.populated())
				{
					if (indexIn < symbSection.stringOffsets.size())
					{
						char* ptr = contents.body.data() + symbSection.address + 0x08 + symbSection.stringOffsets[indexIn];
						result = std::string(ptr);
					}
				}

				return result;
			}
			bool brsarFile::summarizeSymbStringData(std::ostream& output)
			{
				bool result = 0;

				if (output.good() && symbSection.address != ULONG_MAX)
				{
					unsigned long stringOffset = ULONG_MAX;
					unsigned long stringAddress = ULONG_MAX;
					unsigned long stringOffsetAddress = symbSection.address + 0x08 + symbSection.stringOffset + 0x04;
					for (std::size_t i = 0; i < symbSection.stringOffsets.size(); i++)
					{
						stringOffset = symbSection.stringOffsets[i];
						stringAddress = symbSection.address + stringOffset + 0x08;
						char* ptr = contents.body.data() + stringAddress;
						output << "[0x" << lava::numToHexStringWithPadding(i, 4) << "] 0x" << lava::numToHexStringWithPadding(stringOffsetAddress, 4) << "->0x" << lava::numToHexStringWithPadding(stringAddress, 4) << ": " << ptr << "\n";
						stringOffsetAddress += 0x04;
					}

				}

				return result;
			}
			bool brsarFile::outputConsecutiveSoundEntryStringsWithSameFileID(unsigned long startingIndex, std::ostream& output)
			{
				bool result = 0;

				if (output.good() && symbSection.address != ULONG_MAX)
				{
					unsigned long fileID = infoSection.soundEntries[startingIndex].fileID;
					std::size_t i = startingIndex;
					while (i < infoSection.soundEntries.size() && infoSection.soundEntries[i].fileID == fileID)
					{
						output << "\t[String 0x" << lava::numToHexStringWithPadding(i, 0x04) << "] " << getSymbString(i) << "\n";
						i++;
					}
				}

				return result;
			}

			unsigned long brsarFile::getGroupOffset(unsigned long groupIDIn)
			{
				std::size_t result = SIZE_MAX;

				std::size_t i = 0;
				brsarInfoGroupHeader currentGroup;
				bool done = 0;

				while (!done && i < infoSection.groupsSection.refs.size())
				{
					currentGroup.populate(contents, infoSection.groupsSection.refs[i].getAddress(infoSection.address + 0x08));
					if (groupIDIn == currentGroup.stringID)
					{
						result = currentGroup.address;
						done = 1;
					}
					else
					{
						i++;
					}
				}

				return result;
			}
			bool brsarFile::listSoundsInGroup(unsigned long groupIDIn, std::ostream& output)
			{
				bool result = 0;

				if (contents.populated())
				{
					brsarInfoGroupHeader groupHeader;
					brawlReferenceVector collectionReferences;
					std::vector<brsarInfoGroupEntry> collections{};
					std::vector<unsigned long> groupEntriesFirstSoundIndex{};
					std::vector<rwsd> collectionRWSDs{};
					unsigned long groupOffset = getGroupOffset(groupIDIn);

					std::vector<brsarInfoFileEntry*> filesForGroup{};

					if (groupOffset != ULONG_MAX)
					{
						groupHeader.populate(contents, groupOffset);
						collectionReferences.populate(contents, groupHeader.listOffset.getAddress(infoSection.address + 0x08));

						// Populate the collections associated with this group.
						collections.resize(collectionReferences.refs.size());
						for (std::size_t i = 0; i < collectionReferences.refs.size(); i++)
						{
							collections[i].populate(contents, collectionReferences.refs[i].getAddress(infoSection.address + 0x08));
						}

						// Find the first Sound Entry with each collection's File ID.
						groupEntriesFirstSoundIndex.resize(collections.size(), ULONG_MAX);
						for (std::size_t u = 0; u < collections.size(); u++)
						{
							for (std::size_t i = 0; groupEntriesFirstSoundIndex[u] == ULONG_MAX && i < infoSection.soundEntries.size(); i++)
							{
								if (infoSection.soundEntries[i].fileID == collections[u].fileID)
								{
									groupEntriesFirstSoundIndex[u] = i;
								}
							}
						}

						collectionRWSDs.resize(collections.size());
						for (std::size_t i = 0; i < collections.size(); i++)
						{
							collectionRWSDs[i].populate(contents, groupHeader.headerOffset + collections[i].headerOffset);
						}

						std::vector<dataInfo>* dataVector = nullptr;
						std::vector<waveInfo>* waveVector = nullptr;
						dataInfo* dataPtr = nullptr;
						waveInfo* wavePtr = nullptr;
						unsigned long soundEffectNameIndex = ULONG_MAX;
						for (std::size_t i = 0; i < collectionRWSDs.size(); i++)
						{
							dataVector = &collectionRWSDs[i].dataSection.entries;
							waveVector = &collectionRWSDs[i].waveSection.entries;
							output << "Summary for Collection #" << i << " (File ID: 0x" << lava::numToHexStringWithPadding(collections[i].fileID, 0x04) << ")\n";
							for (std::size_t u = 0; u < dataVector->size(); u++)
							{
								dataPtr = &(*dataVector)[u];
								wavePtr = &(*waveVector)[dataPtr->ntWaveIndex];
								soundEffectNameIndex = 0x194 + groupEntriesFirstSoundIndex[i] + u;
								output << "\tSound: 0x" << lava::numToHexStringWithPadding(u, 0x04) << " (\"" <<
									shuckSoundEffectName(getSymbString(soundEffectNameIndex)) << "\")\n";
								output << "\t\tWave Index: 0x" << lava::numToHexStringWithPadding((*dataVector)[u].ntWaveIndex, 0x04) << "\n";
								output << "\t\tWave Data Offset: 0x" << lava::numToHexStringWithPadding(wavePtr->dataLocation, 0x08) << "\n";
								output << "\t\tWave Data Address: 0x" <<
									lava::numToHexStringWithPadding(groupHeader.waveDataOffset + collections[i].dataOffset + wavePtr->dataLocation, 0x08) << "\n";
								output << "\t\tWave Data Length: 0x" << lava::numToHexStringWithPadding(wavePtr->getLengthInBytes(), 0x08) << "\n";
							}
						}
					}
				}
				return result;
			}

			bool brsarFile::exportSawnd(std::size_t groupID, std::string targetFilePath)
			{
				bool result = 0;
				std::cout << "Creating \"" << targetFilePath << "\" from Group #" << groupID << "...\n";

				std::ofstream sawndOutput;
				sawndOutput.open(targetFilePath, std::ios_base::out | std::ios_base::binary);
				if (sawndOutput.is_open())
				{
					std::size_t groupOffset = getGroupOffset(groupID);
					std::cout << "\nGroup found @ 0x" << lava::numToHexStringWithPadding(groupOffset, 8) << "!\n";

					if (groupOffset != SIZE_MAX)
					{
						brsarInfoGroupHeader targetGroup;
						targetGroup.populate(contents, groupOffset);

						std::cout << "Address: 0x" << lava::numToHexStringWithPadding(targetGroup.address, 8) << "\n";
						std::cout << "Header: Offset = 0x" << lava::numToHexStringWithPadding(targetGroup.headerOffset, 8) <<
							", Length = 0x" << lava::numToHexStringWithPadding(targetGroup.headerLength, 8) << " bytes\n";
						std::cout << "Wave Data: Offset = 0x" << lava::numToHexStringWithPadding(targetGroup.waveDataOffset, 8) <<
							", Length = 0x" << lava::numToHexStringWithPadding(targetGroup.waveDataLength, 8) << " bytes\n";

						sawndOutput.put(2);
						lava::writeRawDataToStream(sawndOutput, groupID);
						lava::writeRawDataToStream(sawndOutput, targetGroup.waveDataLength);

						std::size_t collectionRefListAddress = targetGroup.listOffset.getAddress(infoSection.address + 0x08);
						brawlReferenceVector collectionReferences;
						collectionReferences.populate(contents, collectionRefListAddress);
						std::cout << "Collection List Address(0x" << lava::numToHexStringWithPadding(collectionRefListAddress, 8) << ")\n";
						std::cout << "Collection Count: " << collectionReferences.refs.size() << "\n";
						std::vector<brsarInfoGroupEntry> groupInfoEntries;
						brsarInfoGroupEntry* currEntry = nullptr;
						unsigned long currentCollectionAddress = ULONG_MAX;

						for (std::size_t i = 0; i < collectionReferences.refs.size(); i++)
						{
							currentCollectionAddress = collectionReferences.refs[i].getAddress(infoSection.address + 0x08);
							std::cout << "Collection #" << i << ": Info Section Offset = 0x" << lava::numToHexStringWithPadding(currentCollectionAddress, 8) << "\n";
							groupInfoEntries.push_back(brsarInfoGroupEntry());
							currEntry = &groupInfoEntries.back();
							currEntry->populate(contents, currentCollectionAddress);

							std::cout << "\tFile ID: 0x" << lava::numToHexStringWithPadding(currEntry->fileID, 4) << "\n";
							std::cout << "\tHeader Offset: 0x" << lava::numToHexStringWithPadding(currEntry->headerOffset, 4) << "\n";
							std::cout << "\tHeader Length: 0x" << lava::numToHexStringWithPadding(currEntry->headerLength, 4) << "\n";
							std::cout << "\tData Offset: 0x" << lava::numToHexStringWithPadding(currEntry->dataOffset, 4) << "\n";
							std::cout << "\tData Length: 0x" << lava::numToHexStringWithPadding(currEntry->dataLength, 4) << "\n";

							lava::writeRawDataToStream(sawndOutput, currEntry->fileID);
							lava::writeRawDataToStream(sawndOutput, currEntry->dataOffset);
							lava::writeRawDataToStream(sawndOutput, currEntry->dataLength);
						}

						std::cout << "Total Size:" << targetGroup.headerLength + targetGroup.waveDataLength << "\n";
						sawndOutput.write(contents.body.data() + targetGroup.headerOffset, targetGroup.headerLength + targetGroup.waveDataLength);
					}
					else
					{
						std::cerr << "[ERROR] Provided group ID couldn't be located. Aborting export.\n";
						remove(targetFilePath.c_str());
					}
				}
				else
				{
					std::cerr << "[ERROR] Couldn't write to target file location (\"" << targetFilePath << "\").\n";
				}
				return result;
			}

			/* BRSAR */

			/* SAWND */ 

			bool sawndHeader::populate(const lava::byteArray& bodyIn, unsigned long addressIn)
			{
				bool result = 0;

				if (bodyIn.populated())
				{
					address = addressIn;

					sawndVersion = bodyIn.getChar(address);
					groupID = bodyIn.getLong(address + 0x01);
					waveDataLength = bodyIn.getLong(address + 0x05);

					bool firstEntryReached = 0;
					unsigned long cursor = 0x09;
					std::array<unsigned long, 3> currFileTriple{};
					unsigned long currFileTripleSlot = 0;
					while (!firstEntryReached)
					{
						char charIn = bodyIn.getChar(cursor);
						if (charIn != 'R')
						{
							currFileTriple[currFileTripleSlot] = bodyIn.getLong(cursor);
							currFileTripleSlot++;
							if (currFileTripleSlot >= 3)
							{
								fileInfoTriples.push_back(currFileTriple);
								currFileTripleSlot = 0;
								currFileTriple.fill(0x00);
							}
							cursor += 0x04;
						}
						else
						{
							firstEntryReached = 1;
						}
					}

					result = 1;
				}
				return result;
			}

			bool summarizeSawndMetadata(std::string filePathIn, std::ostream& output)
			{
				bool result = 0;

				output << "SAWND Metadata Summary for \"" << filePathIn << "\"...\n\n";

				std::ifstream fileIn(filePathIn, std::ios_base::in | std::ios_base::binary);
				if (fileIn.is_open())
				{
					lava::byteArray sawndFile;
					sawndFile.populate(fileIn);
					if (sawndFile.populated())
					{
						sawndHeader header;
						if (header.populate(sawndFile, 0x00))
						{
							unsigned long assumedWaveDataAddress = sawndFile.body.size() - header.waveDataLength;
							output << "Sawnd Version: " << (int)header.sawndVersion << "\n";
							output << "Group ID: 0x" << lava::numToHexStringWithPadding(header.groupID, 0x04) << " (" << header.groupID << ")\n";
							output << "Total Wave Data Length: 0x" << lava::numToHexStringWithPadding(header.waveDataLength, 0x08) << " (" << header.waveDataLength << ")\n";
							output << "Assumed Wave Data Address: 0x" << lava::numToHexStringWithPadding(assumedWaveDataAddress, 0x08) << "\n";

							if (!header.fileInfoTriples.empty())
							{
								unsigned long cursor = 0x09 + (0x0C * header.fileInfoTriples.size());
								output << "\nStructure Summaries:\n";
								for (unsigned long i = 0; i < header.fileInfoTriples.size(); i++)
								{
									unsigned long fileLength = sawndFile.getLong(cursor + 0x08);
									unsigned long fileTag = sawndFile.getLong(cursor);
									std::stringstream fileTypeStringStream{};
									lava::writeRawDataToStream(fileTypeStringStream, fileTag);
									output << "Collection #" << i + 1 << ", ID: 0x" << lava::numToHexStringWithPadding(header.fileInfoTriples[i][0], 0x04) << " (" << fileTypeStringStream.str() << ") Structure Information\n";
									switch (fileTag)
									{
										case lava::brawl::sawndz::_HEX_TAG_RWSD:
										{
											rwsd tempRWSD;
											if (tempRWSD.populate(sawndFile, cursor))
											{
												output << "\tAddress:\t0x" << lava::numToHexStringWithPadding(tempRWSD.header.address, 0x08) << "\n";
												output << "\tTotal Length/End:\t0x" << lava::numToHexStringWithPadding(tempRWSD.header.headerLength, 0x08) << " / 0x" << lava::numToHexStringWithPadding(tempRWSD.header.headerLength + tempRWSD.header.address, 0x08)<< "\n";
												output << "\tData Section Offset/Address:\t0x" << lava::numToHexStringWithPadding(tempRWSD.header.dataOffset, 0x08) << " / 0x" << lava::numToHexStringWithPadding(tempRWSD.dataSection.address, 0x08) << "\n";
												output << "\tData Section Length/End:\t0x" << lava::numToHexStringWithPadding(tempRWSD.header.dataLength, 0x08) << " / 0x" << lava::numToHexStringWithPadding(tempRWSD.dataSection.address + tempRWSD.header.dataLength, 0x08) << "\n";
												output << "\tData Entry Count:\t\t0x" << lava::numToHexStringWithPadding(tempRWSD.dataSection.entries.size(), 0x04) << "\n";
												output << "\tWave Section Offset/Address:\t0x" << lava::numToHexStringWithPadding(tempRWSD.header.waveOffset, 0x08) << " / 0x" << lava::numToHexStringWithPadding(tempRWSD.waveSection.address, 0x08) << "\n";
												output << "\tWave Section Length/End:\t0x" << lava::numToHexStringWithPadding(tempRWSD.header.waveLength, 0x08) << " / 0x" << lava::numToHexStringWithPadding(tempRWSD.waveSection.address + tempRWSD.header.waveLength, 0x08) << "\n";
												output << "\tWave Entry Count:\t\t0x" << lava::numToHexStringWithPadding(tempRWSD.waveSection.entries.size(), 0x04) << "\n";
												output << "\tRaw Wave Data Offset/Address:\t0x" << lava::numToHexStringWithPadding(header.fileInfoTriples[i][1], 0x08) << " / 0x" << lava::numToHexStringWithPadding(assumedWaveDataAddress + header.fileInfoTriples[i][1], 0x08) << "\n";
												output << "\tRaw Wave Data Length/End:\t0x" << lava::numToHexStringWithPadding(header.fileInfoTriples[i][2], 0x08) << " / 0x" << lava::numToHexStringWithPadding(assumedWaveDataAddress + header.fileInfoTriples[i][1] + header.fileInfoTriples[i][2], 0x08) << "\n";
											}
											break;
										}
										case lava::brawl::sawndz::_HEX_TAG_RBNK:
										{
											output << "\tAddress:\t0x" << lava::numToHexStringWithPadding(cursor, 0x08) << "\n";
											output << "\tTotal Length/End:\t0x" << lava::numToHexStringWithPadding(fileLength, 0x08) << " / 0x" << lava::numToHexStringWithPadding(cursor + fileLength, 0x08) << "\n";
											output << "\tRaw Wave Data Offset/Address:\t0x" << lava::numToHexStringWithPadding(header.fileInfoTriples[i][1], 0x08) << " / 0x" << lava::numToHexStringWithPadding(assumedWaveDataAddress + header.fileInfoTriples[i][1], 0x08) << "\n";
											output << "\tRaw Wave Data Length/End:\t0x" << lava::numToHexStringWithPadding(header.fileInfoTriples[i][2], 0x08) << " / 0x" << lava::numToHexStringWithPadding(assumedWaveDataAddress + header.fileInfoTriples[i][1] + header.fileInfoTriples[i][2], 0x08) << "\n";
											break;
										}
										case lava::brawl::sawndz::_HEX_TAG_RSEQ:
										{
											output << "\tAddress:\t0x" << lava::numToHexStringWithPadding(cursor, 0x08) << "\n";
											output << "\tTotal Length/End:\t0x" << lava::numToHexStringWithPadding(fileLength, 0x08) << " / 0x" << lava::numToHexStringWithPadding(cursor + fileLength, 0x08) << "\n";
											output << "\tRaw Wave Data Offset/Address:\t0x" << lava::numToHexStringWithPadding(header.fileInfoTriples[i][1], 0x08) << " / 0x" << lava::numToHexStringWithPadding(assumedWaveDataAddress + header.fileInfoTriples[i][1], 0x08) << "\n";
											output << "\tRaw Wave Data Length/End:\t0x" << lava::numToHexStringWithPadding(header.fileInfoTriples[i][2], 0x08) << " / 0x" << lava::numToHexStringWithPadding(assumedWaveDataAddress + header.fileInfoTriples[i][1] + header.fileInfoTriples[i][2], 0x08) << "\n";
											break;
										}
										default:
										{
											output << "\tCollection type (" << fileTypeStringStream.str() << ")invalid! SAWND is likely poorly formed!";
											break;
										}
									}
									cursor += sawndFile.getLong(cursor + 0x08);
								}
								cursor = 0x09 + (0x0C * header.fileInfoTriples.size());
								output << "\nEntry Summaries:\n";
								for (unsigned long i = 0; i < header.fileInfoTriples.size(); i++)
								{
									unsigned long fileTag = sawndFile.getLong(cursor);
									std::stringstream fileTypeStringStream{};
									lava::writeRawDataToStream(fileTypeStringStream, fileTag);
									output << "Collection #" << i + 1 << ", ID: 0x" << lava::numToHexStringWithPadding(header.fileInfoTriples[i][0], 0x04) << " (" << fileTypeStringStream.str() << ") Entry Information\n";
									output << "\tFile ID:\t0x" << lava::numToHexStringWithPadding(header.fileInfoTriples[i][0], 0x04) << "\n";
									switch (fileTag)
									{
										case lava::brawl::sawndz::_HEX_TAG_RWSD:
										{
											rwsd tempRWSD;
											if (tempRWSD.populate(sawndFile, cursor))
											{
												std::unordered_map<unsigned long, std::vector<unsigned long>> waveIndecesToReferrerDataIndeces{};

												std::vector<dataInfo>* dataVecPtr = &tempRWSD.dataSection.entries;
												std::vector<waveInfo>* waveVecPtr = &tempRWSD.waveSection.entries;
												brawlReferenceVector* dataRefVecPtr = &tempRWSD.dataSection.entryReferences;
												std::vector<unsigned long>* waveOffVecPtr = &tempRWSD.waveSection.entryOffsets;

												unsigned long waveDataBaseAddress = assumedWaveDataAddress;
												waveDataBaseAddress += header.fileInfoTriples[i][1];

												for (unsigned long u = 0; u < dataVecPtr->size(); u++)
												{
													dataInfo* currDataEntry = &(*dataVecPtr)[u];
													output << "\tData Entry 0x" << lava::numToHexStringWithPadding(u, 0x04) << " (Collection #" << i + 1 << ")\n";
													output << "\t\tOffset / Address:\t0x" << lava::numToHexStringWithPadding(dataRefVecPtr->refs[u].address, 0x08) << " / 0x" << lava::numToHexStringWithPadding(currDataEntry->address, 0x08) << "\n";
													output << "\t\tAssociated Wave ID:\t0x" << lava::numToHexStringWithPadding(currDataEntry->ntWaveIndex, 0x04) << "\n";
													if (currDataEntry->ntWaveIndex != ULONG_MAX)
													{
														auto emplaceResult = waveIndecesToReferrerDataIndeces.emplace(std::make_pair(currDataEntry->ntWaveIndex, std::vector<unsigned long>()));
														if (emplaceResult.second)
														{
															waveInfo* currWaveEntry = &(*waveVecPtr)[currDataEntry->ntWaveIndex];
															output << "\t\tWave Entry Offset / Address:\t0x" << lava::numToHexStringWithPadding((*waveOffVecPtr)[currDataEntry->ntWaveIndex], 0x08) << " / 0x" << lava::numToHexStringWithPadding(currWaveEntry->address, 0x08) << "\n";
															output << "\t\tWave Contents Offset / Address:\t0x" << lava::numToHexStringWithPadding(currWaveEntry->dataLocation, 0x08) << " / 0x" << lava::numToHexStringWithPadding(waveDataBaseAddress + currWaveEntry->dataLocation, 0x08) << "\n";
															output << "\t\tWave Contents Length / End:\t0x" << lava::numToHexStringWithPadding(currWaveEntry->getLengthInBytes(), 0x04) << " / 0x" << lava::numToHexStringWithPadding(waveDataBaseAddress + currWaveEntry->dataLocation + currWaveEntry->getLengthInBytes(), 0x08) << "\n";
															emplaceResult.first->second.push_back(u);
														}
														else
														{
															output << "\t\tSkipping Wave Summary, see Data Entry 0x" << lava::numToHexStringWithPadding(emplaceResult.first->second.front(), 0x04) << "\n";
															emplaceResult.first->second.push_back(u);
														}
													}
												}
											}
											break;
										}
										default:
										{
											output << "No entry summary available for this file type!\n";
											break;
										}
									}
									cursor += sawndFile.getLong(cursor + 0x08);
								}
							}
						}
					}
				}
				return result;
			}

			/* SAWND */
		}
	}
	
}
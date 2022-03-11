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

	namespace sawndz
	{
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
					refs[i] = lava::sawndz::brawlReference(bodyIn.getLLong(cursor));
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

				soundsMaskHeader.populate(bodyIn, address + 0x08 + soundsOffset);
				typesMaskHeader.populate(bodyIn, address + 0x08 + typesOffset);
				groupsMaskHeader.populate(bodyIn, address + 0x08 + groupsOffset);
				banksMaskHeader.populate(bodyIn, address + 0x08 + banksOffset);

				unsigned long cursor = address + 0x08 + stringOffset;
				stringOffsets.resize(bodyIn.getLong(cursor), ULONG_MAX);
				cursor += 0x04;
				for (std::size_t i = 0; i < stringOffsets.size(); i++)
				{
					stringOffsets[i] = bodyIn.getLong(cursor);
					cursor += 0x04;
				}
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
			}

			return result;
		}

		/* BRSAR Info Section */



		/* BRSAR File Section */

		/* RWSD */
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
				entries.resize(entryCount);
				for (std::size_t i = 0; i < entryCount; i++)
				{
					entries[i].populate(bodyIn, address + 0x08 + refVecPtr->at(i).getAddress());
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
				destinationStream << "DATA";
				lava::writeRawDataToStream(destinationStream, length);
				entryReferences.exportContents(destinationStream);
				for (std::size_t i = 0; i < entries.size(); i++)
				{
					entries[i].exportContents(destinationStream);
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
			}

			return result;
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
		bool rwsdWaveSection::populate(lava::byteArray& bodyIn, std::size_t addressIn)
		{
			bool result = 0;

			if (bodyIn.populated())
			{
				address = addressIn;

				length = bodyIn.getLong(addressIn + 0x04);
				entryCount = bodyIn.getLong(addressIn + 0x08);

				for (unsigned long cursor = 0x0; cursor < (entryCount * 4); cursor += 0x04)
				{
					entryOffsets.push_back(bodyIn.getLong(address + 0x0C + cursor));
					entries.push_back(waveInfo());
					entries.back().populate(bodyIn, address + entryOffsets.back());
				}
			}

			return result;
		}
		bool rwsdWaveSection::exportContents(std::ostream& destinationStream)
		{
			bool result = 0;
			if (destinationStream.good())
			{
				destinationStream << "WAVE";
				lava::writeRawDataToStream(destinationStream, length);
				lava::writeRawDataToStream(destinationStream, entryCount);

				for (unsigned long i = 0x0; i < entryOffsets.size(); i++)
				{
					lava::writeRawDataToStream(destinationStream, entryOffsets[i]);
				}
				for (unsigned long i = 0x0; i < entries.size(); i++)
				{
					entries[i].exportContents(destinationStream);
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

		bool rwsd::populate(lava::byteArray& bodyIn, std::size_t addressIn)
		{
			bool result = 0;

			if (bodyIn.populated())
			{
				address = addressIn;
				if (header.populate(bodyIn, address))
				{
					dataSection.populate(bodyIn, address + header.dataOffset);
					waveSection.populate(bodyIn, address + header.waveOffset);
				}
				result = 1;
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
		bool rwsd::exportContents(std::string destinationFile)
		{
			bool result = 0;

			std::ofstream output;
			output.open(destinationFile, std::ios_base::out | std::ios_base::binary);
			if (output.is_open())
			{
				result = rwsd::exportContents(output);
			}

			return result;
		}
		/* RWSD */

		bool brsarFileSection::catalogueRWSD(lava::byteArray& bodyIn, std::size_t addressIn)
		{
			bool result = 0;

			if (bodyIn.populated())
			{
				unsigned long cursor = address + 0x20;
				unsigned long cursorMax = address + length;
				rwsd* currEntry = nullptr;

				std::vector<waveInfo>* currEntryWaveInfoVec = nullptr;
				waveInfo* currWaveInfoEntry = nullptr;

				unsigned int rwsdIndex = 0;

				while (cursor < cursorMax)
				{
					std::cout << "RWSD Entry 0x" << lava::numToHexStringWithPadding(rwsdIndex, 0x08) << " (@ 0x" << lava::numToHexStringWithPadding(cursor, 0x08) << ")\n";
					rwsdEntries.push_back(rwsd());
					currEntry = &rwsdEntries.back();
					currEntry->populate(bodyIn, cursor);
					cursor += currEntry->header.headerLength;

					if (bodyIn.getLong(cursor) != _HEX_TAG_RWSD)
					{
						currEntryWaveInfoVec = &currEntry->waveSection.entries;

						for (std::size_t i = 0; i < currEntry->waveSection.entryCount; i++)
						{
							currWaveInfoEntry = &(*currEntryWaveInfoVec)[i];
							std::cout << "Data Location: 0x" << lava::numToHexStringWithPadding(cursor + currWaveInfoEntry->dataLocation, 0x08);
							std::cout << " + Bytes: 0x" << lava::numToHexStringWithPadding(currWaveInfoEntry->nibbles / 2, 0x08);
							std::cout << " = 0x" << lava::numToHexStringWithPadding(cursor + currWaveInfoEntry->dataLocation + (currWaveInfoEntry->nibbles / 2), 0x08) << "\n";
						}

						cursor += currEntryWaveInfoVec->back().dataLocation + (currEntryWaveInfoVec->back().nibbles / 2);
						if (cursor % 0x10)
						{
							cursor += 0x10 - (cursor % 0x10);
						}

						// 0x52575344 == RWSD in hex, it's the hex tag for a new RWSD section
						while (bodyIn.getLong(cursor) != _HEX_TAG_RWSD)
						{
							cursor += 0x04;
						}
					}
					else
					{
						std::cout << "No entries?\n";
					}

					rwsdIndex++;

				}

			}

			return result;
		}
		bool brsarFileSection::catalogueRWSDCheat(lava::byteArray& bodyIn, std::size_t addressIn)
		{
			bool result = 0;

			if (bodyIn.populated())
			{
				unsigned long cursor = addressIn;
				unsigned long cursorMax = address + length;
				rwsd* currEntry = nullptr;

				std::vector<std::size_t> rwsdLocations = bodyIn.searchMultiple({ 0x52, 0x57, 0x53, 0x44, 0xFE, 0xFF }, cursor, cursorMax);

				rwsdEntries.resize(rwsdLocations.size());
				for (std::size_t i = 0; i < rwsdLocations.size(); i++)
				{
					std::cout << lava::numToHexStringWithPadding(i, 0x04) << " / " << lava::numToHexStringWithPadding(rwsdLocations.size(), 0x04) <<
						" (0x" << lava::numToHexStringWithPadding(rwsdLocations[i], 0x08) << ")\n";
					currEntry = &rwsdEntries[i];
					currEntry->populate(bodyIn, rwsdLocations[i]);
				}
				result = 1;
			}

			return result;
		}
		bool brsarFileSection::populate(lava::byteArray& bodyIn, std::size_t addressIn)
		{
			bool result = 0;

			if (bodyIn.populated())
			{
				address = addressIn;

				length = bodyIn.getLong(address + 0x04);

				//catalogueRWSDCheat(bodyIn, address + 0x20);
				//int test = 1;
			}

			return result;
		}

		/* BRSAR File Section */



		/* BRSAR */

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

				symbSection.populate(contents, contents.getLong(0x10));
				infoSection.populate(contents, contents.getLong(0x18));
				fileSection.populate(contents, contents.getLong(0x20));

				listSoundsInGroup(0x08);
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
		bool brsarFile::listSoundsInGroup(unsigned long groupIDIn)
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
					for (std::size_t i = 0; i < collectionRWSDs.size(); i++)
					{
						dataVector = &collectionRWSDs[i].dataSection.entries;
						waveVector = &collectionRWSDs[i].waveSection.entries;
						std::cout << "Summary for Collection #" << i << " (File ID: 0x" << lava::numToHexStringWithPadding(collections[i].fileID, 0x04) << ")\n";
						for (std::size_t u = 0; u < dataVector->size(); u++)
						{
							dataPtr = &(*dataVector)[u];
							wavePtr = &(*waveVector)[dataPtr->ntWaveIndex];
							std::cout << "\tSound: 0x" << lava::numToHexStringWithPadding(u, 0x04) << " (\"" <<
								getSymbString(0x194 + groupEntriesFirstSoundIndex[i] + u) << "\")\n";
							std::cout << "\t\tWave Index: 0x" << lava::numToHexStringWithPadding((*dataVector)[u].ntWaveIndex, 0x04) << "\n";
							std::cout << "\t\tWave Data Offset: 0x" << lava::numToHexStringWithPadding(wavePtr->dataLocation, 0x08) << "\n";
							std::cout << "\t\tWave Data Address: 0x" << 
								lava::numToHexStringWithPadding(groupHeader.waveDataOffset + collections[i].dataOffset + wavePtr->dataLocation, 0x08) << "\n";
							std::cout << "\t\tWave Data Length: 0x" << lava::numToHexStringWithPadding(wavePtr->nibbles / 2, 0x08) << "\n";
						}
					}


					int temp = 0;
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
					lava::sawndz::brsarInfoGroupHeader targetGroup;
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
					lava::sawndz::rwsd testRWSD;
					testRWSD.populate(contents, targetGroup.headerOffset);
					testRWSD.exportContents("rwsdtest.dat");
					sawndOutput.write(contents.body.data() + targetGroup.headerOffset, targetGroup.headerLength + targetGroup.waveDataLength);
				}
				else
				{
					std::cerr << "Provided group ID couldn't be located. Aborting export.\n";
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

		/* BRSAR */

	}
}
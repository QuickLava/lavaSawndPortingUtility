#include "lavaSawndPortingUtility.h"

const std::string brsarName = "smashbros_sound.brsar";
const std::string overrideFileName = "soundNameOverrides.txt";
const std::string soundBanksToPortName = "soundbanksToPort.txt";

const std::string outputDirectory = "Ported Soundbanks/";
const std::string sawndFileSuffix = ".sawnd";
const std::string logFileSuffix = "_log.txt";
const std::string metadataFileSuffix = "_meta.txt";
const std::string sCorrFileSuffix = "_sound.txt";
const std::string lMPRFileSuffix = "_lMPR.xml";

// Toggles lMPR patch generation. Will be disabled in release builds until I figure out how to edit Misc. Sound Lists.
// The existing "Porting Tools" SFX Changer can do this just fine, that should be prefered at least until I figure this out.
#define ENABLE_LMPR_OUTPUT false
// Toggles porting every bank at once. Disable this for release builds.
#define PORT_ALL_BANKS false
// Toggles outputing ported banks to their own directories. Useful for repacking .zips.
// Forced off when PORT_ALL_BANKS is off because I can't currently generate folders on the fly.
#define OUTPUT_PORTS_TO_DEDICATED_DIR (false && PORT_ALL_BANKS)

int stringToNum(const std::string& stringIn, bool allowNeg = 0, int defaultVal = INT_MAX)
{
	int result = defaultVal;
	std::string manipStr = lava::stringToUpper(stringIn);
	int base = (manipStr.find("0X") == 0) ? 16 : 10;
	char* res = nullptr;
	result = std::strtoul(manipStr.c_str(), &res, base);
	if (res != (manipStr.c_str() + manipStr.size()))
	{
		result = defaultVal;
	}
	if (result < 0 && !allowNeg)
	{
		result = defaultVal;
	}
	return result;
}

unsigned long lookForFIDFromName(std::string nameIn)
{
	unsigned long result = ULONG_MAX;

	nameIn = lava::stringToUpper(nameIn);
	std::unordered_map<unsigned long, std::string>::const_iterator i = lava::brawl::LAVA_CHARA_FID_TO_NAME.begin();
	std::unordered_map<unsigned long, std::string>::const_iterator endItr = lava::brawl::LAVA_CHARA_FID_TO_NAME.end();
	while (result == ULONG_MAX && i != endItr)
	{
		if (nameIn == i->second)
		{
			result = i->first;
		}
		else
		{
			i++;
		}
	}

	return result;
}

std::vector<std::pair<unsigned long, unsigned long>> parseBanksToPort(std::string filePathIn)
{
	std::vector<std::pair<unsigned long, unsigned long>> result{};

	std::cout << "Collecting pairs from \"" << filePathIn << "\"...\n";

	std::ifstream banksToPort;
	banksToPort.open(filePathIn, std::ios_base::in);
	if (banksToPort.is_open())
	{
		std::string currentLine = "";
		std::string manipStr = "";
		while (std::getline(banksToPort, currentLine))
		{
			// Disregard the current line if it's empty, or is marked as a comment
			if (!currentLine.empty() && currentLine[0] != '#' && currentLine[0] != '/')
			{
				manipStr = "";
				for (std::size_t i = 0; i < currentLine.size(); i++)
				{
					if (!std::isspace(currentLine[i]))
					{
						manipStr += currentLine[i];
					}
				}

				std::size_t delimLoc = manipStr.find(',');
				if (delimLoc != std::string::npos && delimLoc < (manipStr.size() - 1))
				{
					std::pair<unsigned long, unsigned long> newEntry = std::make_pair(stringToNum(manipStr.substr(0, delimLoc), 0, ULONG_MAX), stringToNum(manipStr.substr(delimLoc + 1), 0, ULONG_MAX));
					if (newEntry.first == ULONG_MAX)
					{
						newEntry.first = lookForFIDFromName(manipStr.substr(0, delimLoc));
					}
					if (newEntry.second == ULONG_MAX)
					{
						newEntry.second = lookForFIDFromName(manipStr.substr(delimLoc + 1, std::string::npos));
					}

					auto firstIDNameItr = lava::brawl::LAVA_CHARA_FID_TO_NAME.find(newEntry.first);
					if (firstIDNameItr != lava::brawl::LAVA_CHARA_FID_TO_NAME.end())
					{
						auto secondIDNameItr = lava::brawl::LAVA_CHARA_FID_TO_NAME.find(newEntry.second);
						if (secondIDNameItr != lava::brawl::LAVA_CHARA_FID_TO_NAME.end())
						{
							std::cout << "[SUCCESS] Fighter ID Pair: " << "0x" << lava::numToHexStringWithPadding(newEntry.first, 0x02) << " (" << firstIDNameItr->second << "), 0x" << lava::numToHexStringWithPadding(newEntry.second, 0x02) << " (" << secondIDNameItr->second << ").\n"; " \n";
							result.push_back(newEntry);
						}
						else
						{
							std::cerr << "[ERROR] Skipping Fighter Pair: Invalid Destination Fighter Specified (\"" << manipStr.substr(delimLoc + 1, std::string::npos) << "\").\n";
						}
					}
					else
					{
						std::cerr << "[ERROR] Skipping Fighter Pair: Invalid Source Fighter Specified (\"" << manipStr.substr(0, delimLoc) << "\").\n";
					}
				}
			}
		}
		if (result.empty())
		{
			std::cerr << "[WARNING] Successfully loaded input file, but found no valid fighter pairs.\n";
		}
	}
	else
	{
		std::cerr << "[ERROR] Couldn't load input file (\"" << filePathIn << "\"). Ensure that the file exists and try again.\n";
	}

	return result;
}

bool portFighterBankToFighter(lava::brawl::sawndz::brsarFile& brsarFileIn, unsigned long sourceBankFighterID, unsigned long destinationBankFighterID)
{
	bool result = 0;
	
	if (brsarFileIn.contents.populated())
	{
		auto fIDNameMapEndItr = lava::brawl::LAVA_CHARA_FID_TO_NAME.end();
		auto sourceCharNameMapItr = lava::brawl::LAVA_CHARA_FID_TO_NAME.find(sourceBankFighterID);
		auto destinationCharNameMapItr = lava::brawl::LAVA_CHARA_FID_TO_NAME.find(destinationBankFighterID);

		// Checks if both fighter IDs were found in the fID-To-Name map.
		if (sourceCharNameMapItr != fIDNameMapEndItr && destinationCharNameMapItr != fIDNameMapEndItr)
		{
			auto fIDGroupInfoMapEndItr = lava::brawl::sawndz::fighterIDToGroupInfoMap.end();
			auto sourceCharGroupInfoItr = lava::brawl::sawndz::fighterIDToGroupInfoMap.find(sourceCharNameMapItr->first);
			auto destinationCharGroupInfoItr = lava::brawl::sawndz::fighterIDToGroupInfoMap.find(destinationCharNameMapItr->first);

			// Checks that both fighter IDs had group info associated with them.
			if (sourceCharGroupInfoItr != fIDGroupInfoMapEndItr && destinationCharGroupInfoItr != fIDGroupInfoMapEndItr)
			{
				std::string outputFileName = "";
#if OUTPUT_PORTS_TO_DEDICATED_DIR == true
				outputFileName = lava::numToHexStringWithPadding(destinationCharGroupInfoItr->second.groupID - 0x07, 0x03);
				outputFileName += "_(" + sourceCharNameMapItr->second + "_PORT)";
				outputFileName += "/" + outputFileName;
				outputFileName = outputDirectory + outputFileName;
#else
				outputFileName = lava::numToHexStringWithPadding(destinationCharGroupInfoItr->second.groupID - 0x07, 0x03);
				outputFileName += "_(" + sourceCharNameMapItr->second + "_PORT)";
				outputFileName = outputDirectory + outputFileName;
#endif
				std::ofstream fileOut(outputFileName + sawndFileSuffix, std::ios_base::out | std::ios_base::binary);
				if (fileOut.is_open())
				{
					std::ofstream log(outputFileName + logFileSuffix, std::ios_base::out);
					if (log.is_open())
					{
						std::cout << "Porting \"" << sourceCharNameMapItr->second << "\" to \"" << destinationCharNameMapItr->second << "\"...\n";
						log << "lavaSawndPortingUtility " << lava::brawl::sawndz::version << "\n";
						log << "Porting \"" << sourceCharNameMapItr->second << "\" to \"" << destinationCharNameMapItr->second << "\"...\n";

						lava::brawl::sawndz::groupPortSoundCorrespondence soundCorrReceiver;
						result = lava::brawl::sawndz::portGroupToGroup(brsarFileIn, sourceCharNameMapItr->first, destinationCharNameMapItr->first, fileOut, log, &soundCorrReceiver);
						fileOut.close();

						std::ofstream summaryOut(outputFileName + metadataFileSuffix, std::ios_base::out);
						if (summaryOut.is_open())
						{
							lava::brawl::sawndz::summarizeSawndMetadata(outputFileName + sawndFileSuffix, summaryOut);
							summaryOut.close();
						}
						else
						{
							std::cerr << "[ERROR] Failed to generate Sawnd metadata for this port: unable to write to \"" << outputFileName << metadataFileSuffix << "\".\n";
						}

						std::ofstream soundCorrOut(outputFileName + sCorrFileSuffix, std::ios_base::out);
						if (soundCorrOut.is_open())
						{
							if (soundCorrReceiver.successfulMatches != ULONG_MAX)
							{
								soundCorrReceiver.outputCorrespondenceData(soundCorrOut);
							}
							else
							{
								std::cerr << "[ERROR] Failed to generate match data for this port: no successful matches were recorded.\n";
							}
						}
						else
						{
							std::cerr << "[ERROR] Failed to generate match data for this port: unable to write to \"" << outputFileName << sCorrFileSuffix << "\".\n";
						}
#if ENABLE_LMPR_OUTPUT == true
						std::ofstream lMPROut(outputFileName + lMPRFileSuffix, std::ios_base::out);
						if (lMPROut.is_open())
						{
							if (soundCorrReceiver.successfulMatches != ULONG_MAX)
							{
								soundCorrReceiver.outputLMPRPatch(lMPROut, sourceBankFighterID, destinationBankFighterID);
							}
							else
							{
								std::cerr << "[ERROR] Failed to generate an lMPR Patch for this port: no successful matches were recorded.\n";
							}
						}
						else
						{
							std::cerr << "[ERROR] Failed to generate an lMPR Patch for this port: unable to write to \"" << outputFileName << lMPRFileSuffix << "\".\n";
						}
#endif
					}
					else
					{
						std::cerr << "[ERROR] Failed to process port: unable to write to log file location (\"" << outputFileName << logFileSuffix << "\").\n";
					}
				}
				else
				{
					std::cerr << "[ERROR] Failed to process port: unable to write to sawnd file location (\"" << outputFileName << sawndFileSuffix << "\").\n";
				}
			}
			else
			{
				std::cerr << "[ERROR] Failed to process port: couldn't find group info data for provided Fighter IDs.\n";
			}
		}
		else
		{
			std::cerr << "[ERROR] Failed to process port: provided Fighter IDs invalid.\n";
		}
	}
	else
	{
		std::cerr << "[ERROR] Failed to process port: unable to parse BRSAR.\n";
	}

	return result;
}
bool portAllFighterBanksToFighter(lava::brawl::sawndz::brsarFile& brsarFileIn, unsigned long targetBankFighterID)
{
	bool result = 0;

	if (brsarFileIn.contents.populated())
	{
		result = 1;
		for (auto i : lava::brawl::LAVA_CHARA_FID_TO_NAME)
		{
			result &= portFighterBankToFighter(brsarFileIn, i.first, targetBankFighterID);
		}
	}
	
	return result;
}

#if PORT_ALL_BANKS == true
int main(int argc, char* argv[])
{
	std::cout << "lavaSawndPortingUtility " << lava::brawl::sawndz::version << "\n";
	std::cout << "Based directly on work by:\n";
	std::cout << " - Jaklub and Agoaj, as well as mstaklo, ssbbtailsfan, stickman and VILE (Sawndz, Super Sawndz)\n";
	std::cout << " - Soopercool101, as well as Kryal, BlackJax96, and libertyernie (BrawlLib, BrawlBox, BrawlCrate)\n\n";

	std::cout << "\n";
	lava::brawl::sawndz::brsarFile brsarIn;
	if (brsarIn.init(brsarName))
	{
		std::cout << "\n";
		lava::brawl::sawndz::populateSoundEffectNameOverrides(overrideFileName);
		portAllFighterBanksToFighter(brsarIn, lava::brawl::LCFI_SNAKE);
	}
	else
	{
		std::cerr << "[ERROR] Couldn't parse provided BRSAR (\"" << brsarName << "\")! Make sure a valid BRSAR is located in this folder and named correctly, then try again.";
	}

	std::cout << "Press any key to exit.\n";
	_getch();
	return 0;
}
#else
int main(int argc, char* argv[])
{
	std::cout << "lavaSawndPortingUtility " << lava::brawl::sawndz::version << "\n";
	std::cout << "Based directly on work by:\n";
	std::cout << " - Jaklub and Agoaj, as well as mstaklo, ssbbtailsfan, stickman and VILE (Sawndz, Super Sawndz)\n";
	std::cout << " - Soopercool101, as well as Kryal, BlackJax96, and libertyernie (BrawlLib, BrawlBox, BrawlCrate)\n\n";
	
	std::vector<std::pair<unsigned long, unsigned long>> soundBanksToPort = parseBanksToPort(soundBanksToPortName);

	if (!soundBanksToPort.empty())
	{
		std::cout << "\n";
		lava::brawl::sawndz::brsarFile brsarIn;
		if (brsarIn.init(brsarName))
		{
			std::cout << "\n";
			lava::brawl::sawndz::populateSoundEffectNameOverrides(overrideFileName);

			for (std::size_t i = 0; i < soundBanksToPort.size(); i++)
			{
				portFighterBankToFighter(brsarIn, soundBanksToPort[i].first, soundBanksToPort[i].second);
			}
		}
		else
		{
			std::cerr << "[ERROR] Couldn't parse provided BRSAR (\"" << brsarName << "\")! Make sure a valid BRSAR is located in this folder and named correctly, then try again.";
		}
	}

	std::cout << "Press any key to exit.\n";
	_getch();
	return 0;
}
#endif
int summarizeSawndMain(int argc, char* argv[])
{
	std::cout << "lavaSawndSummaryUtility " << lava::brawl::sawndz::version << "\n";
	std::cout << "Based directly on work by:\n";
	std::cout << " - Jaklub and Agoaj, as well as mstaklo, ssbbtailsfan, stickman and VILE (Sawndz, Super Sawndz)\n";
	std::cout << " - Soopercool101, as well as Kryal, BlackJax96, and libertyernie (BrawlLib, BrawlBox, BrawlCrate)\n\n";

	for (unsigned long i = 1; i < argc; i++)
	{
		std::string filePath = argv[i];
		std::cout << "Summarizing " << filePath << "... ";
		std::size_t finalPeriod = filePath.rfind('.');
		if (filePath.substr(finalPeriod, std::string::npos) == ".sawnd")
		{
			std::string summaryFilePath = filePath.substr(0, finalPeriod) + "_meta.txt";
			std::ofstream summaryOutput(summaryFilePath);
			if (summaryOutput.is_open())
			{
				std::cout << "Success!\n";
				lava::brawl::sawndz::summarizeSawndMetadata(filePath, summaryOutput);
			}
			else
			{
				std::cerr << "[ERROR] Failure! Couldn't open summary output file (" << summaryFilePath << ")!\n";
			}
		}
		else
		{
			std::cerr << "[ERROR] Failure! Couldn't open input file to summarize!\n";
		}
	}

	std::cout << "Press any key to exit.\n";
	_getch();
	return 0;
}

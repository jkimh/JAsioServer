#include "stdafx.h"
#include "JReplayReader.h"
#include "JSharedFunc.h"
#include "JLogger.h"


JReplayReader& GetReplayReader()
{
	static JReplayReader reader;
	return reader;
}
JReplayReader::JReplayReader()
{
}


JReplayReader::~JReplayReader()
{
}

void JReplayReader::LoadReplayInfo(const char* filePath)
{
	std::ifstream inf;
	inf.open(filePath);

	std::string line;
	while (inf)
	{
		std::getline(inf, line);
		auto replayInfo = JReplay_Info::DeSerialize(line);
		if (replayInfo == false)
			continue;
		m_packetInfoListBySessionID[replayInfo.sessionID].push_back(replayInfo);
	}
	inf.close();
	for (auto it : m_packetInfoListBySessionID)
	{
		std::sort(it.second.begin(), it.second.end());
	}
}

void JReplayReader::LoadReplayInfo(const std::vector<JReplay_Info>& packetList)
{
	for (auto replayInfo : packetList)
	{
		m_packetInfoListBySessionID[replayInfo.sessionID].push_back(replayInfo);
	}
}
std::vector<JReplay_Info>& JReplayReader::GetPacketInfoListByIdx(uint32_t index)
{
	return std::next(m_packetInfoListBySessionID.begin(), index)->second;
}
size_t JReplayReader::GetPacketInfoListCount()
{
	return m_packetInfoListBySessionID.size();
}
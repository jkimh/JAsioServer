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
		if (!replayInfo)
			continue;
		m_packetInfoListBySessionID[replayInfo.value().sessionID].push_back(replayInfo.value());
	}
	inf.close();
	for (auto it : m_packetInfoListBySessionID)
	{
		std::sort(it.second.begin(), it.second.end());
	}
}

std::vector<JReplay_Info>* JReplayReader::GetPacketInfoList(uint32_t sessionID)
{
	auto it = m_packetInfoListBySessionID.find(sessionID);
	if(it == m_packetInfoListBySessionID.end())
		return nullptr;
	return &it->second;
}

std::vector<JReplay_Info>& JReplayReader::GetPacketInfoListByIdx(uint32_t index)
{
	return std::next(m_packetInfoListBySessionID.begin(), index)->second;
}
size_t JReplayReader::GetPacketInfoListCount()
{
	return m_packetInfoListBySessionID.size();
}
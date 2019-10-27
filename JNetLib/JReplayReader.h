#pragma once
#include "JReplaySaveWorker.h"
#include <map>
class JReplayReader
{
public:
	JReplayReader();
	~JReplayReader();

	void LoadReplayInfo(const char* filePath);
	std::vector<JReplay_Info>* GetPacketInfoList(uint32_t sessionID);
	std::vector<JReplay_Info>& GetPacketInfoListByIdx(uint32_t index);
	size_t GetPacketInfoListCount();

private:
	std::map<uint64_t, std::vector<JReplay_Info>> m_packetInfoListBySessionID;
};

JReplayReader& GetReplayReader();


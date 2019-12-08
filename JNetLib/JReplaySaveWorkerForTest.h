#pragma once
#include "JReplaySaveWorker.h"
class JReplaySaveWorkerForTest : public IReplaySaverWorker
{
public:
	JReplaySaveWorkerForTest();
	~JReplaySaveWorkerForTest();
	virtual void SetFileName(const char* fileName) override;
	virtual void Push(const uint64_t& tickCount, const uint32_t& sessionID, const size_t& buffersize, const char* buffer) override;
	virtual void RunThread() override;
	virtual void End() override;
	virtual std::vector<JReplay_Info>& GetPacketList() { return m_packetBufferInfos; }

private:
	std::vector<JReplay_Info> m_packetBufferInfos;
};

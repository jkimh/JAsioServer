#pragma once
#include "JReplaySaveWorker.h"
class JReplaySaveWorkerMgr
{
public:
	JReplaySaveWorkerMgr();
	~JReplaySaveWorkerMgr();

	void Init(bool isTest = false);
	void SetFileName(const char* fileName);
	void Push(const uint64_t& tickCount, const uint32_t& sessionID, const size_t& buffersize, const char* buffer);
	void RunThread();
	void End();
	std::vector<JReplay_Info>& GetPacketList();
private:
	std::unique_ptr<IReplaySaverWorker> m_saveWorker;
};


JReplaySaveWorkerMgr& GetReplaySaveWorker();
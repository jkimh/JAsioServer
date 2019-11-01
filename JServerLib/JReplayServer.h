#pragma once
#include "JServer.h"
#include "JReplaySaveWorker.h"
class JReplayServer :
	public JServer
{
public:
	JReplayServer();
	virtual void PreUpdateCommanders(uint64_t tickCount) override;

private:
	std::vector<std::vector<JReplay_Info>> m_packetInfoListEachSession;
};


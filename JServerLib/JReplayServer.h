#pragma once
#include "JServer.h"
#include "JReplaySaveWorker.h"
#include "JTickClass.h"
using namespace JSharedLib;
class JReplaySession;
class JReplayServer :
	public JServer
{
public:
	JReplayServer();
	virtual void PreUpdateCommanders(const JTickClass& tick) override;
	void PreUpdateCommandersForTest(const JTickClass& tick, std::vector<std::shared_ptr<JReplaySession>>& out); //테스트를 위해 세션 리스트를 돌려준다.


private:
	std::vector<std::vector<JReplay_Info>> m_packetInfoListEachSession;
};


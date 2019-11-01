#include "stdafx.h"
#include "JReplayServer.h"
#include "JReplayReader.h"
#include "JReplaySession.h"
#include "JCommander.h"

JReplayServer::JReplayServer() : JServer::JServer()
{
	size_t listCount = GetReplayReader().GetPacketInfoListCount();
	for (size_t i = 0; i < listCount; i++)
	{
		m_packetInfoListEachSession.push_back(GetReplayReader().GetPacketInfoListByIdx(i));		
	}
}

//가문 생성을 파일을 통해 진행한다.
void JReplayServer::PreUpdateCommanders(uint64_t tickCount)
{
	for (auto it = m_packetInfoListEachSession.begin(); it != m_packetInfoListEachSession.end();)
	{
		if ((*it).size() == 0)
			continue;
		if ((*it)[0].tickCount == tickCount)
		{
			//create session, commander
			auto session = std::dynamic_pointer_cast<ISession>(std::make_shared<JReplaySession>(*it));
			auto this_sharedPtr(this->shared_from_this());
			m_preCommanders.push_back(std::make_shared<JCommander>(session, this_sharedPtr));
			it = m_packetInfoListEachSession.erase(it);
		}
		else
			it++;
	}
	JServer::PreUpdateCommanders(tickCount);
}

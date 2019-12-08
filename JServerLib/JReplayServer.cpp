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

//���� ������ ������ ���� �����Ѵ�.
//��� ���� �� PreUpdateCommandersForTest �� ���� �ٲ���ߵȴ�.
void JReplayServer::PreUpdateCommanders(const JTickClass& tick)
{
	for (auto it = m_packetInfoListEachSession.begin(); it != m_packetInfoListEachSession.end();)
	{
		if ((*it).size() == 0)
			continue;
		if ((*it)[0].tickCount == tick.tickCount)
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
	JServer::PreUpdateCommanders(tick);
}

//PreUpdateCommanders �Լ��� ������ ����� �ؾ��Ѵ�.
void JReplayServer::PreUpdateCommandersForTest(const JTickClass& tick, std::vector<std::shared_ptr<JReplaySession>>& out)
{
	for (auto it = m_packetInfoListEachSession.begin(); it != m_packetInfoListEachSession.end();)
	{
		if ((*it).size() == 0)
			continue;
		if ((*it)[0].tickCount == tick.tickCount)
		{
			//create session, commander
			auto session = std::make_shared<JReplaySession>(*it);
			auto this_sharedPtr(this->shared_from_this());
			m_preCommanders.push_back(std::make_shared<JCommander>(std::dynamic_pointer_cast<ISession>(session), this_sharedPtr));
			session->SetIsSaveSendQueue(true);
			out.push_back(session);
			it = m_packetInfoListEachSession.erase(it);
		}
		else
			it++;
	}
	JServer::PreUpdateCommanders(tick);
}

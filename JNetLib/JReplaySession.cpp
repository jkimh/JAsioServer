#include "stdafx.h"
#include "JReplaySession.h"
#include "JLogger.h"

JReplaySession::JReplaySession(std::vector<JReplay_Info>& packetList) : 
	m_packetList(packetList.begin(), packetList.end()), m_sessionID(packetList.begin()->sessionID)
{
}


JReplaySession::~JReplaySession()
{
}

bool JReplaySession::ProcessPacket(const uint64_t& tickCount, std::function<bool(const uint64_t&, PACKET_HEADER*)> packetProcess)
{
	if (m_packetList.size() == 0)
		return true;
	if (m_packetList.top().tickCount > tickCount)
		return true;
	else if (m_packetList.top().tickCount < tickCount)
	{
		JLogger.Error("tick捞 瘤车绰单 贸府甫 给窃");
		return false;
	}
	while (m_packetList.top().tickCount == tickCount)
	{
		auto packetInfo = m_packetList.top();
		packetProcess(tickCount, reinterpret_cast<PACKET_HEADER*>(&packetInfo.packetBuffer[0]));

		m_packetList.pop();
		if (m_packetList.size() == 0)
			break;
	}
	return true;
}

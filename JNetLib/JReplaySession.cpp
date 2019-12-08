#include "stdafx.h"
#include "JReplaySession.h"
#include "JLogger.h"

JReplaySession::JReplaySession(std::vector<JReplay_Info>& packetList) : 
	m_packetList(packetList.begin(), packetList.end()), m_sessionID(packetList.begin()->sessionID), m_isSaveSendQue(false)
{
}


JReplaySession::~JReplaySession()
{
}

bool JReplaySession::ProcessPacket(const JTickClass& tick, std::function<bool(const JTickClass&, PACKET_HEADER*)> packetProcess)
{
	if (m_packetList.size() == 0)
		return true;
	if (m_packetList.top().tickCount > tick.tickCount)
		return true;
	else if (m_packetList.top().tickCount < tick.tickCount)
	{
		JLogger.Error("tick捞 瘤车绰单 贸府甫 给窃");
		return false;
	}
	while (m_packetList.top().tickCount == tick.tickCount)
	{
		auto packetInfo = m_packetList.top();
		packetProcess(tick, reinterpret_cast<PACKET_HEADER*>(&packetInfo.packetBuffer[0]));

		m_packetList.pop();
		if (m_packetList.size() == 0)
			break;
	}
	return true;
}

void JReplaySession::PostSend(const bool& isImmediately, const size_t& size, std::shared_ptr<PACKET_HEADER>& data)
{
	if (!m_isSaveSendQue)
		return;

	m_sendQue.push(data);
}

std::shared_ptr<PACKET_HEADER> JReplaySession::PopLastSendPacket()
{
	if (m_sendQue.empty())
		return nullptr;

	auto result = m_sendQue.front();
	m_sendQue.pop();
	return result;
}

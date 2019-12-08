#include "pch.h"
#include "JTestSession.h"


JTestSession::JTestSession()
{
}


JTestSession::~JTestSession()
{
}

void JTestSession::PostSend(const bool & isImmediately, const size_t & size, std::shared_ptr<PACKET_HEADER>& data)
{
	m_sendQue.push(data);
}

bool JTestSession::ProcessPacket(const JTickClass& tick, std::function<bool(const JTickClass&, PACKET_HEADER*)> packetProcess)
{
	auto it = m_recvMap.find(tick.tickCount);
	if (it != m_recvMap.end())
	{
		packetProcess(tick, it->second.get());
		m_recvMap.erase(it);
	}
	return true;
}

bool JTestSession::IsDisconnected()
{
	return false;
}

uint32_t JTestSession::GetSessionID()
{
	return uint32_t();
}

void JTestSession::AddRecvPacket(uint64_t tickCount, std::unique_ptr<PACKET_HEADER> packet)
{
	m_recvMap.insert({ tickCount, std::move(packet) });
}

std::shared_ptr<PACKET_HEADER> JTestSession::PopLastSendPacket()
{
	if(m_sendQue.empty())
		return nullptr;

	auto result = m_sendQue.front();
	m_sendQue.pop();
	return result;
}

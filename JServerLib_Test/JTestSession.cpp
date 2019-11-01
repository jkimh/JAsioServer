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

bool JTestSession::ProcessPacket(const uint64_t & tickCount, std::function<bool(const uint64_t&, PACKET_HEADER*)> packetProcess)
{
	return false;
}

bool JTestSession::IsDisconnected()
{
	return false;
}

uint32_t JTestSession::GetSessionID()
{
	return uint32_t();
}

std::shared_ptr<PACKET_HEADER> JTestSession::GetLastSendPacket()
{
	if(m_sendQue.empty())
		return nullptr;

	auto result = m_sendQue.front();
	m_sendQue.pop();
	return result;
}

#pragma once
#include "ISession.h"
class JTestSession : public ISession
{
public:
	JTestSession();
	~JTestSession();

	// Inherited via ISession
	virtual void PostSend(const bool & isImmediately, const size_t & size, std::shared_ptr<PACKET_HEADER>& data) override;
	virtual bool ProcessPacket(const uint64_t & tickCount, std::function<bool(const uint64_t&, PACKET_HEADER*)> packetProcess) override;
	virtual bool IsDisconnected() override;
	virtual uint32_t GetSessionID() override;

	std::shared_ptr<PACKET_HEADER> GetLastSendPacket();
private:
	std::queue<std::shared_ptr<PACKET_HEADER>> m_sendQue;
};


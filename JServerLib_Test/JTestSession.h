#pragma once
#include "ISession.h"
class JTestSession : public ISession
{
public:
	JTestSession();
	~JTestSession();

	// Inherited via ISession
	virtual void PostSend(const bool & isImmediately, const size_t & size, const std::shared_ptr<PACKET_HEADER>& data) override;
	virtual bool ProcessPacket(const JTickClass & tickCount, std::function<bool(const JTickClass&, PACKET_HEADER*)> packetProcess) override;
	virtual bool IsDisconnected() override;
	virtual uint32_t GetSessionID() override;
	virtual std::shared_ptr<PACKET_HEADER> PopLastSendPacket() override;

	void AddRecvPacket(uint64_t tickCount, std::unique_ptr<PACKET_HEADER> packet);

private:
	std::queue<std::shared_ptr<PACKET_HEADER>> m_sendQue;
	std::map<uint64_t, std::unique_ptr<PACKET_HEADER>> m_recvMap;
};


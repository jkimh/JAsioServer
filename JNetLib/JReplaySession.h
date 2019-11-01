#pragma once
#include "ISession.h"
#include <iostream>
#include "JReplaySaveWorker.h"

//������ ���ǰ��� �ٸ���, input�� ���Ϸ�, output�� �������� �Ѵ�.
//�� ���� ĳ���� ������ �������ϴµ� ���� �ϳ��� �����Ѱ� ...?
//input�� ������ ���ǿ��� read�� ���� ���۸� �״�� ������ ��. � �÷��̾�� ��Ī�Ǵ��� �Ǵ��ϱⰡ �����.
//��Ŷ�� �̸� �� �о����, tick �� ���缭 ó���� ������.

class JReplaySession : public ISession
{
public:
	JReplaySession(std::vector<JReplay_Info>& packetList);
	~JReplaySession();
	virtual void PostSend(const bool& isImmediately, const size_t& size, std::shared_ptr<PACKET_HEADER>& data) override {}
	virtual bool ProcessPacket(const uint64_t& tickCount, std::function<bool(const uint64_t&, PACKET_HEADER*)> packetProcess) override;
	virtual bool IsDisconnected() override {	return false; };
	virtual uint32_t GetSessionID() override { return m_sessionID; };
private:

	std::priority_queue<JReplay_Info, std::vector<JReplay_Info>, std::greater<JReplay_Info>> m_packetList;
	uint32_t m_sessionID;
};


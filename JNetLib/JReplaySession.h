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
	virtual void PostSend(const bool& isImmediately, const size_t& size, const std::shared_ptr<PACKET_HEADER>& data) override;
	virtual bool ProcessPacket(const JTickClass& tick, std::function<bool(const JTickClass&, PACKET_HEADER*)> packetProcess) override;
	virtual bool IsDisconnected() override { return false; };
	virtual uint32_t GetSessionID() override { return m_sessionID; };

	virtual std::shared_ptr<PACKET_HEADER> PopLastSendPacket() override;
	void SetIsSaveSendQueue(bool isSave) { m_isSaveSendQue = isSave; }

private:
	bool m_isSaveSendQue;
	std::queue<std::shared_ptr<PACKET_HEADER>> m_sendQue;
	std::priority_queue<JReplay_Info, std::vector<JReplay_Info>, std::greater<JReplay_Info>> m_packetList;
	uint32_t m_sessionID;
};


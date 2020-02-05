#pragma once
#include "ISession.h"
#include <iostream>
#include "JReplaySaveWorker.h"

//기존의 세션과는 다르게, input을 파일로, output은 버리도록 한다.
//각 개별 캐릭당 세션을 만들어야하는데 파일 하나로 가능한가 ...?
//input은 기존의 세션에서 read로 들어온 버퍼를 그대로 복사한 것. 어떤 플레이어에게 매칭되는지 판단하기가 힘들다.
//페킷을 미리 다 읽어놓고, tick 에 맞춰서 처리를 해주자.

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


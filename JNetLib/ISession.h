#pragma once
#include "stdafx.h"
#include "JPacket.h"
class ISession
{
public:
	virtual void PostSend(const bool& isImmediately, const size_t& size, std::shared_ptr<PACKET_HEADER>& data) = 0;
	virtual bool ProcessPacket(const uint64_t& tickCount, std::function<bool(const uint64_t&, PACKET_HEADER*)> packetProcess) = 0;
	virtual bool IsDisconnected() = 0;
	virtual uint32_t GetSessionID() = 0;
};

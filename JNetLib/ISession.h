#pragma once
#include "stdafx.h"
#include "JPacket.h"
#include "JTickClass.h"
using namespace JSharedLib;
class ISession
{
public:
	virtual void PostSend(const bool& isImmediately, const size_t& size, const std::shared_ptr<PACKET_HEADER>& data) = 0;
	virtual bool ProcessPacket(const JTickClass& tick, std::function<bool(const JTickClass&, PACKET_HEADER*)> packetProcess) = 0;
	virtual bool IsDisconnected() = 0;
	virtual uint32_t GetSessionID() = 0;
	virtual std::shared_ptr<PACKET_HEADER> PopLastSendPacket() = 0;
};

#include "stdafx.h"
#include "JCommander.h"
#include "ISession.h"
#include "JLogger.h"
#include "JReplaySaveWorkerMgr.h"
#include "JServer.h"
#include "JPlayerChar.h"

void JCommander::ProcessPacket(const JTickClass& tick)
{
	m_session->ProcessPacket(tick, [this](const JTickClass& tick, PACKET_HEADER* data)->bool {
		return this->ProcessPacket(tick, data);
	});
}
void JCommander::BroadCast_ToZoneServer(const shared_ptr<PACKET_HEADER>& packet)
{
	auto serverPtr = m_server.lock();
	if (serverPtr)
	{
		serverPtr->BroadCastToCommander(packet);
	}
}
bool JCommander::ProcessPacket(const JTickClass& tick, PACKET_HEADER* data)
{
	GetReplaySaveWorker().Push(tick.tickCount, m_session->GetSessionID(), data->size, (char*)data);
	switch (data->command)
	{
		case PACKET_COMMAND::PACKET_CS_LOGIN:
			OnPacket(reinterpret_cast<PKS_CS_LOGIN*>(data));
			break;
		case PACKET_COMMAND::PACKET_CS_LOGOUT:
			OnPacket(reinterpret_cast<PKS_CS_LOGOUT*>(data));
			break;
		case PACKET_COMMAND::PACKET_CS_CHAT:
			OnPacket(reinterpret_cast<PKS_CS_CHAT*>(data));
			break;
		case PACKET_COMMAND::PACKET_CS_MOVE:
			OnPacket(reinterpret_cast<PKS_CS_MOVE*>(data));
			break;
		default:
			JLogger.Error("wrong packet (command:%d)", data->command);
			return false;
	}
	return true;
}

void JCommander::OnPacket(PKS_CS_LOGIN* packet)
{
	Init(packet->commanderID);
	JLogger.Log("Login : %s", packet->commanderID);
	PKS_SC_LOGIN_ACK ack;
	ack.command = PACKET_COMMAND::PACKET_SC_LOGIN_ACK;
	ack.size = sizeof(PKS_SC_LOGIN_ACK);
	strcpy_s(ack.commanderID, sizeof(ack.commanderID), m_commanderName.c_str());
	auto packetPtr = std::dynamic_pointer_cast<PACKET_HEADER>(std::make_shared<PKS_SC_LOGIN_ACK>(ack));
	Send(packetPtr);
	
	//
	auto out = std::make_shared<PKS_SC_MOVE>();
	out->command = PACKET_COMMAND::PACKET_SC_MOVE;
	out->size = sizeof(PKS_SC_MOVE);
	out->dest = m_char->GetPosition();
	auto movePtr = std::dynamic_pointer_cast<PACKET_HEADER>(out);
	Send(movePtr);
}

void JCommander::OnPacket(PKS_CS_LOGOUT* packet)
{
	//soket disconnect는 클라의 요청에 의해 이루어질 것이며, 해당 값은 IsDisconnected로 알 수 있다.
	//JCommander는 IsDisconnected가 먼저 판별된 이후 아래 Close 함수에서 요청한 내용이 완료되었을 시 파괴할 수 있다.
	//현재는 바로 파괴 가능한 상태
	JLogger.Log("Logout : %s", m_commanderName.c_str());
	Close();
}
void JCommander::OnPacket(PKS_CS_CHAT* packet)
{
	JLogger.Log("Chat : %s", packet->chat);
	auto serverPtr = m_server.lock();
	if (serverPtr)
	{
		PKS_SC_CHAT ack;
		ack.command = PACKET_COMMAND::PACKET_SC_CHAT;
		ack.size = sizeof(PKS_SC_CHAT);
		strcpy_s(ack.chat, packet->chat);
		auto packetPtr = std::dynamic_pointer_cast<PACKET_HEADER>(std::make_shared<PKS_SC_CHAT>(ack));
		serverPtr->BroadCastToCommander(packetPtr);
	}

}

void JCommander::OnPacket(PKS_CS_MOVE* packet)
{
	JLogger.Log("Chat : %s", packet->dest.ToString().c_str());

	//월드 및 섹션 체크
	//포지션 업데이트
	m_char->ReqMove(packet->dest);

}
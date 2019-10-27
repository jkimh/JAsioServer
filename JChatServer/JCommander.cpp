#include "pch.h"
#include "JCommander.h"
#include "JServer.h"
#include "ISession.h"
#include <functional>
#include "JLogger.h"
#include "JReplaySaveWorker.h"

JCommander::JCommander(const std::shared_ptr<ISession>& session, std::shared_ptr<JServer>& server)
	: m_session(session), m_server(server), m_isLogout(false)
{
}


JCommander::~JCommander()
{
	JLogger.Log("Commander destroy : %s", m_commanderName.c_str());
}

void JCommander::Init(const char* name)
{
	m_commanderName = name;
}

void JCommander::Update()
{
	
}

void JCommander::Send(std::shared_ptr<PACKET_HEADER>& packet)
{
	m_session->PostSend(false, packet->size, packet);
}

void JCommander::ProcessPacket(const uint64_t& tickCount)
{
	m_session->ProcessPacket(tickCount, [this](const uint64_t& tick, PACKET_HEADER* data)->bool {
		return this->ProcessPacket(tick, data);
	});
}
bool JCommander::ProcessPacket(const uint64_t& tickCount, PACKET_HEADER* data)
{
	GetReplaySaveWorker().Push(tickCount, m_session->GetSessionID(), data->size, (char*)data);
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
	auto packetPtr = std::dynamic_pointer_cast<PACKET_HEADER>(std::make_shared<PKS_SC_LOGIN_ACK>(ack));
	Send(packetPtr);
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
	auto m_serverPtr = m_server.lock();
	if (m_serverPtr)
	{
		PKS_SC_CHAT ack;
		ack.command = PACKET_COMMAND::PACKET_SC_CHAT;
		ack.size = sizeof(PKS_SC_CHAT);
		strcpy_s(ack.chat, packet->chat);
		auto packetPtr = std::dynamic_pointer_cast<PACKET_HEADER>(std::make_shared<PKS_SC_CHAT>(ack));
		m_serverPtr->BroadCastToCommander(packetPtr);
	}
	
}

void JCommander::Close()
{
	m_isLogout = true;
}

bool JCommander::IsReadyDie()
{
	return m_isLogout;
}
bool JCommander::IsDisConnected()
{
	return m_session->IsDisconnected();
}
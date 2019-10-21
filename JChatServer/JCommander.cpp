#include "pch.h"
#include "JCommander.h"
#include "JServer.h"
#include "JSession.h"
#include <functional>
#include "JLogger.h"

JCommander::JCommander(const std::shared_ptr<JSession>& session, JServer* server)
	: m_session(session), m_server(server), m_isLogout(false)
{
}


JCommander::~JCommander()
{
	JLogger.Log("Commander destroy : %s", m_commanderName);
}

void JCommander::Init(const char* name)
{
	m_commanderName = name;
}

void JCommander::Update()
{
	
}

void JCommander::Send(PACKET_HEADER* packet)
{
	m_session->PostSend(false, packet->size, (char*) packet);
}

void JCommander::ProcessPacket()
{
	m_session->ProcessPacket([this](PACKET_HEADER* data)->bool {
		return this->ProcessPacket(data);
	});
}
bool JCommander::ProcessPacket(PACKET_HEADER* data)
{
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
			//err
			//packet list �ʱ�ȭ
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
	Send(&ack);
}
void JCommander::OnPacket(PKS_CS_LOGOUT* packet)
{
	//soket disconnect�� Ŭ���� ��û�� ���� �̷���� ���̸�, �ش� ���� IsDisconnected�� �� �� �ִ�.
	//JCommander�� IsDisconnected�� ���� �Ǻ��� ���� �Ʒ� Close �Լ����� ��û�� ������ �Ϸ�Ǿ��� �� �ı��� �� �ִ�.
	//����� �ٷ� �ı� ������ ����
	JLogger.Log("Logout : %s", m_commanderName);
	Close();
}
void JCommander::OnPacket(PKS_CS_CHAT* packet)
{
	JLogger.Log("Chat : %s", packet->chat);
	PKS_SC_CHAT ack;
	ack.command = PACKET_COMMAND::PACKET_SC_CHAT;
	ack.size = sizeof(PKS_SC_CHAT);
	strcpy_s(ack.chat, packet->chat);
	m_server->BroadCastToCommander(&ack);
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
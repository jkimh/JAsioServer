#include "stdafx.h"
#include "JCommander.h"
#include "JServer.h"
#include "ISession.h"
#include <functional>
#include "JLogger.h"
#include "JCommanderCommand.h"
#include "JPlayerChar.h"

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
	if (m_processCommand)
	{
		if (!m_processCommand->Update())
		{
			m_processCommand->End();
			m_processCommand = nullptr;
		}
	}
}

void JCommander::Send(std::shared_ptr<PACKET_HEADER>& packet)
{
	m_session->PostSend(false, packet->size, packet);
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
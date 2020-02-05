#include "stdafx.h"
#include "JCommander.h"
#include "JServer.h"
#include "ISession.h"
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
	auto this_sharedPtr(this->shared_from_this());
	m_char = make_shared<JPlayerChar>(this_sharedPtr);
}

void JCommander::Update(const JTickClass& tick)
{
	if (m_processCommand)
	{
		if (!m_processCommand->Update())
		{
			m_processCommand->End();
			m_processCommand = nullptr;
		}
	}
	m_char->Update(tick);
}

void JCommander::Send(const std::shared_ptr<PACKET_HEADER>& packet)
{
	m_session->PostSend(false, packet->size, packet);
}

void JCommander::Send(const std::shared_ptr<PACKET_HEADER>& packet, int size)
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
const Vector3& JCommander::GetPosition()
{
	return m_char->GetPosition();
}

void JCommander::UpdateViews(const vector<shared_ptr<JCommander>>& commanderList)
{
	const int viewDist = 50;
	m_views.clear();
	for (auto commander : commanderList)
	{
		if (commander.get() == this)
			continue;

		auto dist = CalDist(GetPosition(), commander->GetPosition());
		if (viewDist < dist)
			continue;

		m_views.push_back(commander);
	}

	int viewCount = m_views.size();
	if (viewCount == 0)
		return;
	auto out = std::make_shared<PKS_SC_VIEW>();
	out->command = PACKET_COMMAND::PACKET_SC_VIEW;
	out->size = sizeof(PKS_SC_VIEW);

	if (viewCount >= 1)
	{
		auto commander = m_views[0].lock();
		if (commander)
		{
			strcpy_s(out->commanderID1[0], commander->GetCommanderName().c_str());
			out->position1[0] = commander->GetPosition();
		}
	}
	else
	{
		strcpy_s(out->commanderID1[0], "None");
	}
	if (viewCount >= 2)
	{
		auto commander = m_views[1].lock();
		if (commander)
		{
			strcpy_s(out->commanderID1[1], commander->GetCommanderName().c_str());
			out->position1[1] = commander->GetPosition();
		}
	}
	else
	{
		strcpy_s(out->commanderID1[1], "None");
	}
	
	if (viewCount >= 3)
	{
		auto commander = m_views[2].lock();
		if (commander)
		{
			strcpy_s(out->commanderID1[2], commander->GetCommanderName().c_str());
			out->position1[2] = commander->GetPosition();
		}
	}
	else
	{
		strcpy_s(out->commanderID1[2], "None");
	}

	if (viewCount >= 4)
	{
		auto commander = m_views[3].lock();
		if (commander)
		{
			strcpy_s(out->commanderID1[3], commander->GetCommanderName().c_str());
			out->position1[3] = commander->GetPosition();
		}
	}
	else
	{
		strcpy_s(out->commanderID1[3], "None");
	}

	if (viewCount >= 5)
	{
		auto commander = m_views[4].lock();
		if (commander)
		{
			strcpy_s(out->commanderID1[4], commander->GetCommanderName().c_str());
			out->position1[4] = commander->GetPosition();
		}
	}
	else
	{
		strcpy_s(out->commanderID1[4], "None");
	}
	auto viewPtr = std::dynamic_pointer_cast<PACKET_HEADER>(out);
	Send(viewPtr);
}
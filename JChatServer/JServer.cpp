#include "pch.h"
#include "JServer.h"
#include "JSession.h"
#include "JCommander.h"
#include "JLogger.h"

JServer::JServer(boost::asio::io_context & io_context, short port)
	: m_ioContext(io_context), m_acceptor(io_context, tcp::endpoint(tcp::v4(), port)), m_newSessionID(0)
{
	m_isUsingPreCommanders.clear();
	PostAccept();
}

JServer::~JServer()
{
}

void JServer::PostAccept()
{
	std::shared_ptr<JSession> session = std::make_shared<JSession>(m_ioContext, ++m_newSessionID);
	m_acceptor.async_accept(session->GetSocket(),
		boost::bind(&JServer::handle_accept, this, session,
			boost::asio::placeholders::error));
}

void JServer::PreUpdateCommanders(uint64_t tickCount)
{
	{
		//�ѹ��� �õ��غ��� �̹� ������̸� ���� �����ӿ� �����Ѵ�.
		//���� ���� ��û �� 1���� ������ ���� �������� ����
		if (m_isUsingPreCommanders.test_and_set(std::memory_order_acquire))
		{
			JLogger.Log("commander move to next frame (m_isUsingPreCommanders locked)");
			return;
		}
		if (m_preCommanders.size() < 1)
		{
			m_isUsingPreCommanders.clear(std::memory_order_release);
			return;
		}
		//todo : push replayinfo
		m_commanders.insert(m_commanders.end(), m_preCommanders.begin(), m_preCommanders.end());
		m_preCommanders.clear();
		m_isUsingPreCommanders.clear(std::memory_order_release);
	}
}
void JServer::ProcessPacket(uint64_t tickCount)
{
	for (auto commander : m_commanders)
	{
		commander->ProcessPacket(tickCount);
	}
}
void JServer::UpdateCommanders(uint64_t tickCount)
{
	for (auto it = m_commanders.begin(); it != m_commanders.end(); )
	{
		auto& commander = *it;
		if (commander->IsDisConnected() || commander->IsReadyDie())
		{
			commander->Close();
			m_readyDieCommanders.push_back(*it);
			it = m_commanders.erase(it);
			continue;
		}
		commander->Update();
		it++;

	}

	for (auto it = m_readyDieCommanders.begin(); it != m_readyDieCommanders.end(); )
	{
		auto& commander = *it;
		if (commander->IsReadyDie())
		{			
			if (commander.use_count() == 1)
			{
				it = m_readyDieCommanders.erase(it);
			} 
			else
			{
				JLogger.Error("[Error] : commander ref count is not 1");
			}
		}
	}
}
void JServer::BroadCastToCommander(std::shared_ptr<PACKET_HEADER>& packet)
{
	for (auto commander : m_commanders)
	{
		commander->Send(packet);
	}
}
void JServer::handle_accept(const std::shared_ptr<JSession>& session, const boost::system::error_code & error)
{
	if (!error)
	{
		session->PostReceive();		
		{
			while (m_isUsingPreCommanders.test_and_set(std::memory_order_acquire));
			auto this_sharedPtr(this->shared_from_this());
			m_preCommanders.push_back(std::make_shared<JCommander>(std::dynamic_pointer_cast<ISession>(session), this_sharedPtr));
			m_isUsingPreCommanders.clear(std::memory_order_release);
		}
	}
	//client�κ��� ������ ������ �� �ٽ� ����Ѵ�.
	PostAccept();
}

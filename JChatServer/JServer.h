#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "JSCPacket.h"

using boost::asio::ip::tcp;
class JSession;
class JCommander;
class JServer
{
public:
	JServer(boost::asio::io_context& io_context, short port);
	~JServer();
	void PostAccept();
	void ProcessPacket();
	void UpdateCommanders();
	void PreUpdateCommanders();
	void BroadCastToCommander(PACKET_HEADER* packet);
private:
	void handle_accept(const std::shared_ptr<JSession>& session, const boost::system::error_code& error);

private:
	std::vector<std::shared_ptr<JCommander>> m_preCommanders;
	std::vector<std::shared_ptr<JCommander>> m_commanders;
	std::vector<std::shared_ptr<JCommander>> m_readyDieCommanders;
	std::atomic_flag m_isUsingPreCommanders;
	boost::asio::io_context& m_ioContext;
	tcp::acceptor m_acceptor;
	unsigned int m_newSessionID;
};


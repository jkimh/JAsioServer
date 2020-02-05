#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "JSCPacket.h"
#include "JTickClass.h"

using boost::asio::ip::tcp;
class JSession;
class JCommander;
using namespace JSharedLib;
class JServer : public std::enable_shared_from_this<JServer>
{
public:
	JServer();
	~JServer();
	void ProcessPacket(const JTickClass& tick);
	void UpdateCommanders(const JTickClass& tick);
	virtual void PreUpdateCommanders(const JTickClass& tick);
	void BroadCastToCommander(const std::shared_ptr<PACKET_HEADER>& packet);
	size_t GetCommanderCount() { return m_commanders.size(); }
	void UpdateView();
protected:
	std::vector<std::shared_ptr<JCommander>> m_preCommanders;
	std::atomic_flag m_isUsingPreCommanders;
	uint32_t m_newSessionID;
	std::vector<std::shared_ptr<JCommander>> m_commanders;
private:
	std::vector<std::shared_ptr<JCommander>> m_readyDieCommanders;
};

class JSocketServer : public JServer
{
public:
	JSocketServer(boost::asio::io_context& io_context, short port);
	void PostAccept();
private:
	void handle_accept(const std::shared_ptr<JSession>& session, const boost::system::error_code& error);
protected:
	boost::asio::io_context& m_ioContext;
	tcp::acceptor m_acceptor;

};

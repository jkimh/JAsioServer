#pragma once

#include <iostream>
#include <queue>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <functional>
#include "JSCPacket.h"
#include "ISession.h"

using boost::asio::ip::tcp;
#define RECV_BUF_SIZE 1024
#define RECV_PROCESS_BUF_SIZE RECV_BUF_SIZE * 4 //��Ŷ ���μ������� ����� ���� ũ��

class JSession : public ISession
{
public:
	JSession(boost::asio::io_context& io_context, const uint32_t& sessionID);
	~JSession();
	tcp::socket& GetSocket(){ return m_socket; }
	void PostReceive();
	char* ResizeBuffer(char* buffer, size_t beforeSize, size_t afterSize);

	virtual void PostSend(const bool& isImmediately, const size_t& size, const std::shared_ptr<PACKET_HEADER>& data) override;
	virtual bool ProcessPacket(const JTickClass& tick, std::function<bool(const JTickClass&, PACKET_HEADER*)> packetProcess) override;
	virtual bool IsDisconnected() override  { return m_isDisconnected; }
	virtual uint32_t GetSessionID() override  { return m_sessionID; }
	virtual std::shared_ptr<PACKET_HEADER> PopLastSendPacket() override;
private:
	void handle_write(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_read(const boost::system::error_code& error, size_t bytes_transferred);

private:
	tcp::socket m_socket;
	std::string m_writeMessage;	

	std::array<char, RECV_BUF_SIZE> m_receiveBuffer;
	std::array<char, RECV_PROCESS_BUF_SIZE> m_packetRecvProcessBuffer;
	char* m_packetRecvBuffer; //resize�� ������ ���� ����
	size_t m_packetRecvBufferSize;
	int m_packetRecvBufferMark;
	int m_packetRecvProcessBufferMark;
	std::atomic_flag m_isUsingRecvBuffer;
	std::mutex m_mutexRecvBuffer;

	std::queue<std::shared_ptr<PACKET_HEADER>> m_sendDataQueue;
	std::mutex m_mutexSendQueue;

	bool m_isDisconnected;
	uint32_t m_sessionID;
};
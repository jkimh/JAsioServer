#pragma once

#include <iostream>
#include <queue>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <functional>
#include "JSCPacket.h"

using boost::asio::ip::tcp;
#define RECV_BUF_SIZE 1024
#define RECV_PROCESS_BUF_SIZE RECV_BUF_SIZE * 4 //패킷 프로세스에서 사용할 버퍼 크기

class JSession
{
public:
	JSession(boost::asio::io_context& io_context);
	~JSession();
	tcp::socket& GetSocket(){ return m_socket; }
	bool IsDisconnected() { return m_isDisconnected; }
	void PostReceive();
	char* ResizeBuffer(char* buffer, size_t beforeSize, size_t afterSize);

	void PostSend(const bool isImmediately, const size_t size, std::shared_ptr<PACKET_HEADER>& data);
	bool ProcessPacket(std::function<bool(PACKET_HEADER*)> packetProcess);
private:
	void handle_write(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_read(const boost::system::error_code& error, size_t bytes_transferred);

private:
	tcp::socket m_socket;
	std::string m_writeMessage;	

	std::array<char, RECV_BUF_SIZE> m_receiveBuffer;
	std::array<char, RECV_PROCESS_BUF_SIZE> m_packetRecvProcessBuffer;
	char* m_packetRecvBuffer; //resize가 가능한 가변 버퍼
	size_t m_packetRecvBufferSize;
	int m_packetRecvBufferMark;
	int m_packetRecvProcessBufferMark;
	std::atomic_flag m_isUsingRecvBuffer;
	std::mutex m_mutexRecvBuffer;

	std::queue<std::shared_ptr<PACKET_HEADER>> m_sendDataQueue;
	std::mutex m_mutexSendQueue;

	bool m_isDisconnected;
};
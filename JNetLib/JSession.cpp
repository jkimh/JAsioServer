#include "stdafx.h"
#include "JSession.h"
#include "JSCPacket.h"
#include "JLogger.h"


JSession::JSession(boost::asio::io_context& io_context, const uint32_t& sessionID)
	: m_socket(io_context), m_packetRecvBufferMark(0), m_packetRecvProcessBufferMark(0), m_isDisconnected(false), m_sessionID(sessionID),
	m_packetRecvBuffer(new char[RECV_BUF_SIZE * 2]),
	m_packetRecvBufferSize(RECV_BUF_SIZE * 2)
{
	m_isUsingRecvBuffer.clear();
}

JSession::~JSession()
{
	delete[] m_packetRecvBuffer;
	while (!m_sendDataQueue.empty())
	{
		m_sendDataQueue.pop();
	}
	JLogger.Log("JSession destory");
}

void JSession::PostReceive()
{
	m_receiveBuffer.fill({});
	m_socket.async_read_some(
		boost::asio::buffer(m_receiveBuffer, RECV_BUF_SIZE),
		boost::bind(&JSession::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void JSession::PostSend(const bool& isImmediately, const size_t& size, std::shared_ptr<PACKET_HEADER>& data)
{
	if (!isImmediately)
	{
		std::lock_guard<std::mutex> lock(m_mutexSendQueue);
		m_sendDataQueue.push(data);
		if (m_sendDataQueue.size() > 1)
			return;
	}
	else
	{
	}
	boost::asio::async_write(m_socket, boost::asio::buffer((char*)data.get(), size),
		boost::bind(&JSession::handle_write, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
	);
}

void JSession::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		if (error == boost::asio::error::eof)
		{
			JLogger.Log("handle_write : ������ ���������ϴ�.");
		}
		else
		{
			JLogger.Error("handle_write : error No : %d, error Msg : %s", error.value(), error.message().c_str());
		}
		m_isDisconnected = true; //todo : ���� ���� �� ������ ���� �ı��Ǿ� ǥ�ð� ���� ����
		return;
	}
	{
		std::lock_guard<std::mutex> lock(m_mutexSendQueue);
		m_sendDataQueue.pop();
		if (m_sendDataQueue.empty())
			return;
	}
	auto &dataPtr = m_sendDataQueue.front();
	PACKET_HEADER* pHeader = reinterpret_cast<PACKET_HEADER*>(dataPtr.get());
	PostSend(true, pHeader->size, m_sendDataQueue.front());
}

char* JSession::ResizeBuffer(char* buffer, size_t beforeSize, size_t afterSize)
{
	char* tempBuffer = new char[afterSize]();
	JLogger.Log("buffer resize %u > %u", beforeSize, afterSize);
	memcpy_s(tempBuffer, afterSize, buffer, beforeSize);
	delete[] buffer;
	return tempBuffer;
}

bool JSession::ProcessPacket(const uint64_t& tickCount, std::function<bool(const uint64_t&, PACKET_HEADER*)> packetProcess)
{
	int copySize = 0;
	//lock�� �ּ�ȭ �ϱ� ���� ���۸� �ϳ� �� �ξ� �����Ѵ�.
	{
		int remainBufSize = (int)m_packetRecvProcessBuffer.size() - m_packetRecvProcessBufferMark - 1;
		if (m_isUsingRecvBuffer.test_and_set(std::memory_order_acquire))
		{
			return true;
		}
		//m_packetRecvBuffer �� ��� ũ�⸦ �ø��� ������ �ֱ� ������ �� Ŀ�� ���ɼ��� ����. 
		//�̷��� ���� ���� ���� ũ�⸦ �ø����� �ʰ�, m_packetRecvBufferUsingLogicThread ũ�⸸ŭ�� ó���ϵ��� ��.
		if (remainBufSize < m_packetRecvBufferMark)
		{
			memcpy_s(&m_packetRecvProcessBuffer[m_packetRecvProcessBufferMark], remainBufSize, &m_packetRecvBuffer[0], remainBufSize);
			
			char tempBuffer[RECV_PROCESS_BUF_SIZE] = { 0, };
			memcpy_s(&tempBuffer[0], RECV_PROCESS_BUF_SIZE, &m_packetRecvBuffer[remainBufSize], m_packetRecvBufferSize - remainBufSize);
			memcpy_s(&m_packetRecvBuffer[0], m_packetRecvBufferSize, &tempBuffer[0], m_packetRecvBufferSize - remainBufSize);

			m_packetRecvBufferMark -= remainBufSize;
			copySize = remainBufSize;
		}
		else
		{
			memcpy(&m_packetRecvProcessBuffer[m_packetRecvProcessBufferMark], &m_packetRecvBuffer[0], m_packetRecvBufferMark);
			copySize = m_packetRecvBufferMark;
			m_packetRecvBufferMark = 0;
		}
		m_isUsingRecvBuffer.clear(std::memory_order_release);
	}

	size_t remainPacketData = m_packetRecvProcessBufferMark + copySize;
	int readData = 0;

	while (remainPacketData > 0)
	{
		if (remainPacketData < sizeof(PACKET_HEADER))
			break;

		PACKET_HEADER* header = (PACKET_HEADER*)&m_packetRecvProcessBuffer[readData];
		if (header->size <= remainPacketData)
		{
			if (!packetProcess(tickCount, reinterpret_cast<PACKET_HEADER*>(header)))
			{
				return false;
			}
			remainPacketData -= header->size;
			readData += (int)header->size;
		}
		else
			break;
	}

	if (remainPacketData > 0)
	{
		char tempBuffer[RECV_BUF_SIZE] = { 0, };
		memcpy_s(&tempBuffer[0], RECV_BUF_SIZE, &m_packetRecvProcessBuffer[readData], remainPacketData);
		memcpy_s(&m_packetRecvProcessBuffer[0], remainPacketData, &tempBuffer[0], remainPacketData);
	}
	m_packetRecvProcessBufferMark = (int)remainPacketData;
	return true;
}

void JSession::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		if (error == boost::asio::error::eof)
		{
			JLogger.Log("handle_read : ������ ���������ϴ�.");
		}
		else
		{
			JLogger.Error("handle_read : error No : %d, error Msg : %s", error.value(), error.message().c_str());
		}
		m_isDisconnected = true;
		return;
	}

	//m_isUsingRecvBuffer lock
	{
		//m_packetRecvBufferMark�� ä���� ��Ŷ�� ó���ϱ� ���� �ٽ� �ҷ��� 
		//���� ������ ���ڸ���, m_packetRecvBuffer ũ�⸦ �÷����� (�ִ��� �ø��� �ʾƾ��Ѵ�.)
		size_t afterSize = m_packetRecvBufferMark + bytes_transferred;
		while (m_isUsingRecvBuffer.test_and_set(std::memory_order_acquire));
		if (afterSize > m_packetRecvBufferSize)
		{
			m_packetRecvBuffer = ResizeBuffer(m_packetRecvBuffer, m_packetRecvBufferSize, m_packetRecvBufferSize * 2);
			m_packetRecvBufferSize *= 2;
		}

		memcpy(&m_packetRecvBuffer[m_packetRecvBufferMark], m_receiveBuffer.data(), bytes_transferred);
		m_packetRecvBufferMark = (int)afterSize;
		m_isUsingRecvBuffer.clear(std::memory_order_release);
	}
	PostReceive();
}
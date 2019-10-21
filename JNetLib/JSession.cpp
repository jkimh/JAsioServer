#include "stdafx.h"
#include "JSession.h"
#include "JSCPacket.h"
#include "JLogger.h"


JSession::JSession(boost::asio::io_context& io_context)
	: m_socket(io_context), m_packetRecvBufferMark(0), m_packetRecvProcessBufferMark(0), m_isDisconnected(false),
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
		delete[] m_sendDataQueue.front();
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

void JSession::PostSend(const bool isImmediately, const size_t size, char* data)
{
	char* sendData = nullptr;
	if (!isImmediately)
	{
		sendData = new char[size];
		memcpy_s(sendData, size, data, size);
		{
			std::lock_guard<std::mutex> lock(m_mutexSendQueue);
			m_sendDataQueue.push(sendData);
		}
	}
	else
	{
		sendData = data;
	}

	if (!isImmediately && m_sendDataQueue.size() > 1)
		return;

	boost::asio::async_write(m_socket, boost::asio::buffer(sendData, size),
		boost::bind(&JSession::handle_write, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
	);
}

void JSession::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
	char* pData = nullptr;
	{
		std::lock_guard<std::mutex> lock(m_mutexSendQueue);
		delete[] m_sendDataQueue.front();
		m_sendDataQueue.pop();
		if (m_sendDataQueue.empty())
			return;
		pData = m_sendDataQueue.front();
	}

	PACKET_HEADER* pHeader = (PACKET_HEADER*)pData;
	PostSend(true, pHeader->size, pData);
}

char* JSession::ResizeBuffer(char* buffer, size_t beforeSize, size_t afterSize)
{
	char* tempBuffer = new char[afterSize]();
	JLogger.Log("buffer resize %u > %u", beforeSize, afterSize);
	memcpy_s(tempBuffer, afterSize, buffer, beforeSize);
	delete[] buffer;
	return tempBuffer;
}

bool JSession::ProcessPacket(std::function<bool(PACKET_HEADER*)> packetProcess)
{
	int copySize = 0;
	//lock을 최소화 하기 위해 버퍼를 하나 더 두어 복사한다.
	{
		int remainBufSize = (int)m_packetRecvProcessBuffer.size() - m_packetRecvProcessBufferMark - 1;
		std::lock_guard<std::mutex> lock(m_mutexRecvBuffer);
		//m_packetRecvBuffer 의 경우 크기를 늘리는 로직이 있기 때문에 더 커질 가능성이 있음. 
		//이러한 경우는 같이 버퍼 크기를 늘리지는 않고, m_packetRecvBufferUsingLogicThread 크기만큼만 처리하도록 함.
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
			if (!packetProcess(reinterpret_cast<PACKET_HEADER*>(header)))
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
			JLogger.Log("handle_read : 연결이 끊어졌습니다.");
		}
		else
		{
			JLogger.Error("handle_read : error No : %d, error Msg : %s", error.value(), error.message());
		}
		m_isDisconnected = true;
	}
	else
	{
		{
			//m_packetRecvBufferMark에 채워진 패킷을 처리하기 전에 다시 불려서 
			//남은 공간이 모자르면, m_packetRecvBuffer 크기를 늘려주자 (최대한 늘리지 않아야한다.)
			size_t afterSize = m_packetRecvBufferMark + bytes_transferred;
			std::lock_guard<std::mutex> lock(m_mutexRecvBuffer);
			if (afterSize > m_packetRecvBufferSize)
			{
				m_packetRecvBuffer = ResizeBuffer(m_packetRecvBuffer, m_packetRecvBufferSize, m_packetRecvBufferSize * 2);
				m_packetRecvBufferSize *= 2;
			}
			memcpy(&m_packetRecvBuffer[m_packetRecvBufferMark], m_receiveBuffer.data(), bytes_transferred);
			m_packetRecvBufferMark = (int)afterSize;
		}
		PostReceive();
	}
}
#include "stdafx.h"
#include "JReplaySaveWorker.h"
#include "JLogger.h"

bool operator<(JReplay_Info a, JReplay_Info b)
{
	return a.tickCount < b.tickCount;
}
bool operator>(JReplay_Info a, JReplay_Info b)
{
	return a.tickCount > b.tickCount;
}
JReplaySaveWorker& GetReplaySaveWorker()
{
	static JReplaySaveWorker worker;
	return worker;
}

JReplaySaveWorker::JReplaySaveWorker() : m_end(false), m_saveFileName("temp.txt"), m_workerThread(nullptr), m_isRunning(false)
{

}


JReplaySaveWorker::~JReplaySaveWorker()
{
	if (!m_workerThread)
	{
		m_workerThread->detach();
		delete m_workerThread;
	}
}

void JReplaySaveWorker::SetFileName(const char * fileName)
{
	m_saveFileName = fileName;
}

void JReplaySaveWorker::Push(const uint64_t& tickCount, const uint32_t& sessionID, const size_t& buffersize, const char* buffer)
{
	if (!m_isRunning)
		return;
	{
		std::lock_guard<std::mutex> lock(m_mutexBuffer);
		JReplay_Info newInfo;
		newInfo.tickCount = tickCount;
		newInfo.sessionID = sessionID;
		newInfo.packetBuffer.insert(newInfo.packetBuffer.end(), &buffer[0], &buffer[buffersize - 1]);
		m_packetBufferInfos.push_back(newInfo);
		m_cvBuffer.notify_one();
	}
}

void JReplaySaveWorker::RunThread()
{
	m_workerThread = new std::thread(&JReplaySaveWorker::Run, this);
	m_isRunning = true;
}

void JReplaySaveWorker::End()
{
	m_end = true;
}

void JReplaySaveWorker::Run()
{
	std::ofstream outf;
	outf.open(m_saveFileName.c_str(), std::ios::binary);
	if (!outf.is_open())
	{
		JLogger.Error("open fail");
		return;
	}
	while (!m_end)
	{
		std::unique_lock<std::mutex> lock(m_mutexBuffer);
		m_cvBuffer.wait(lock, [&] { return !m_packetBufferInfos.empty() || m_end; });
		m_packetBufferInfosForProcess.assign(m_packetBufferInfos.begin(), m_packetBufferInfos.end());
		m_packetBufferInfos.clear();
		lock.unlock();

		for (auto bufferInfo : m_packetBufferInfosForProcess)
		{
			auto replayInfo = JReplay_Info::Serialize(bufferInfo);
			outf.write(&replayInfo[0], replayInfo.size());
			outf << std::endl;
		}
		m_packetBufferInfosForProcess.clear();

	}
	outf.close();
}

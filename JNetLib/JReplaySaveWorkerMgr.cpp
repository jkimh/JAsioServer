#include "stdafx.h"
#include "JReplaySaveWorkerMgr.h"
#include "JReplaySaveWorker.h"
#include "JReplaySaveWorkerForTest.h"

using namespace std;
JReplaySaveWorkerMgr& GetReplaySaveWorker()
{
	static JReplaySaveWorkerMgr replay;
	return replay;
}
JReplaySaveWorkerMgr::JReplaySaveWorkerMgr()
	: m_saveWorker(nullptr)
{
}


JReplaySaveWorkerMgr::~JReplaySaveWorkerMgr()
{
}

void JReplaySaveWorkerMgr::Init(bool isTest)
{
	if (isTest)
	{
		m_saveWorker = make_unique<JReplaySaveWorkerForTest>();
	}
	else
	{
		m_saveWorker = make_unique<JReplaySaveWorker>();

	}
}

void JReplaySaveWorkerMgr::SetFileName(const char * fileName)
{
	if (m_saveWorker == nullptr)
		return;
	m_saveWorker->SetFileName(fileName);
}

void JReplaySaveWorkerMgr::Push(const uint64_t & tickCount, const uint32_t & sessionID, const size_t & buffersize, const char * buffer)
{
	if (m_saveWorker == nullptr)
		return;
	m_saveWorker->Push(tickCount, sessionID, buffersize, buffer);
}

void JReplaySaveWorkerMgr::RunThread()
{
	if (m_saveWorker == nullptr)
		return;
	m_saveWorker->RunThread();
}

void JReplaySaveWorkerMgr::End()
{
	if (m_saveWorker == nullptr)
		return;
	m_saveWorker->End();
	m_saveWorker = nullptr;
}

std::vector<JReplay_Info>& JReplaySaveWorkerMgr::GetPacketList()
{
	if (m_saveWorker == nullptr)
	{
		static std::vector<JReplay_Info> nullvector;
		return nullvector;
	}
	return m_saveWorker->GetPacketList();
}

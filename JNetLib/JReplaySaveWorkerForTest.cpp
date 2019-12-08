#include "stdafx.h"
#include "JReplaySaveWorkerForTest.h"

JReplaySaveWorkerForTest::JReplaySaveWorkerForTest()
{
}


JReplaySaveWorkerForTest::~JReplaySaveWorkerForTest()
{
}

void JReplaySaveWorkerForTest::SetFileName(const char * fileName)
{
}

void JReplaySaveWorkerForTest::Push(const uint64_t & tickCount, const uint32_t & sessionID, const size_t & buffersize, const char * buffer)
{
	JReplay_Info newInfo;
	newInfo.tickCount = tickCount;
	newInfo.sessionID = sessionID;
	newInfo.packetBuffer.insert(newInfo.packetBuffer.end(), &buffer[0], &buffer[buffersize - 1]);
	m_packetBufferInfos.push_back(newInfo);
}

void JReplaySaveWorkerForTest::RunThread()
{
}

void JReplaySaveWorkerForTest::End()
{
}

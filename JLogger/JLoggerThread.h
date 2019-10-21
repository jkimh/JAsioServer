#pragma once
#include "JLogger.h"
enum LOG_LEVEL
{
	LOG_INFO,
	LOG_ERROR,
	LOG_CRITICAL
};
struct JLoggerLogInfo
{
	int logIdx;
	int funcLine;
	LOG_LEVEL logLv;
	std::string funcName;
	std::string logString;
};
class JLoggerThread
{
public:
	JLoggerThread();
	~JLoggerThread();

	void Run();
	void LoopLogging();
	void MoveQueue();
	void ProcessLogging();
	void Log(const JLoggerLogInfo& logInfo);
	void PrintConsole(const JLoggerLogInfo& logInfo);


	std::thread m_logThread;
	std::mutex m_queueMutex;
	std::queue<JLoggerLogInfo> m_loginfoPrevQueue;
	std::queue<JLoggerLogInfo> m_loginfoMoveQueue;
};


#include "stdafx.h"
#include "JLogger.h"
#include <cstdlib>
#include <cstdio>
#include <stdarg.h>
#include <windows.h>
#include <io.h>
#include <direct.h>

#define MAX_BUF_SIZE  4096
#define COLOR_ERROR "\x1b[31m"

JLoggerClass& GetJLogger(const char *funcName, int line)
{
	static JLoggerClass logger;
	logger.SetFileName(funcName);
	logger.SetLine(line);
	return logger;
}

JLoggerClass::JLoggerClass() : m_logFilePath("None"), m_isIgnoreNormalLog(false)
{
}


JLoggerClass::~JLoggerClass()
{
	if (m_fileOut.is_open())
	{
		m_fileOut.close();
	}
}

bool JLoggerClass::Init(const char* logCategory, const char* fileName, bool isIgnoreNormalLog)
{
	std::string logFolderName = "LOG_" + std::string(logCategory);
	m_logFilePath = logFolderName + "\\" + fileName;
	if (_access(logFolderName.c_str(), 0) == -1)
	{
		if (_mkdir(logFolderName.c_str()) == -1)
		{
			std::cout << "log folder create fail" << std::endl;
			return false;
		}
	}
	m_fileOut.open(m_logFilePath.c_str());
	m_isIgnoreNormalLog = isIgnoreNormalLog;
	JLogger.Log("Log write start");
	return true;
}


void JLoggerClass::Log(const char *format, ...)
{
	if (m_isIgnoreNormalLog)
		return;
	va_list ap;
	char buf[MAX_BUF_SIZE];
	va_start(ap, format);
	vsprintf_s(buf, format, ap);
	va_end(ap);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	char logbuf[MAX_BUF_SIZE];
	sprintf_s(logbuf, "[LOG][File:%s][Line:%d] %s", GetFuncName().c_str(), GetLine(), buf);
	std::cout << logbuf << std::endl;
	//m_fileOut << logbuf;
}

void JLoggerClass::Error(const char *format, ...)
{
	va_list ap;
	char buf[MAX_BUF_SIZE];
	va_start(ap, format);
	vsprintf_s(buf, format, ap);
	va_end(ap);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000C | FOREGROUND_INTENSITY);
	char logbuf[MAX_BUF_SIZE];
	sprintf_s(logbuf, "[ERROR][File:%s][Line:%d] %s", GetFuncName().c_str(), GetLine(), buf);
	std::cout << logbuf << std::endl;
	m_fileOut << logbuf;
}

const std::string& JLoggerClass::GetLogFileName()
{
	return m_logFilePath;
}
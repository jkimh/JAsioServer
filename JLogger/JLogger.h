#pragma once
/*
로그를 남기기 위한 라이브러리
파일, 콘솔창 출력 구현
주의 :
 - 해당 프로젝트만 참조하여 로그 기능을 사용할 수 있어야한다. 
	(공통 자료형 관련 프로젝트 외 프로젝트 참조 금지)
*/
#include <fstream>
class JLoggerClass
{
public:
	JLoggerClass();
	~JLoggerClass();

	bool Init(const char* logCategory, const char* fileName, bool isIgnoreNormalLog); //카테고리명으로 로그폴더 이름을 만든다.
	void Log(const char *format, ...);
	void Error(const char *format, ...);
	const std::string& GetLogFileName();

	inline void SetFileName(const char* funcName)
	{
		m_funcName = funcName;
	}
	inline std::string GetFuncName()
	{
		return m_funcName;
	}
	inline void SetLine(int line)
	{
		m_line = line;
	}
	inline int GetLine()
	{
		return m_line;
	}

private:
	std::ofstream m_fileOut;
	std::string m_funcName;
	std::string m_logFilePath;
	int m_line;
	bool m_isIgnoreNormalLog;
};

JLoggerClass& GetJLogger(const char *funcName, int line);


#define JLogger GetJLogger(__FILE__, __LINE__)
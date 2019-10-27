#include "stdafx.h"
#include "JSharedFunc.h"
#include "JSharedConst.h"

namespace JSharedFunc
{
	tm GetCurTimeStruct()
	{
		auto now = std::chrono::system_clock::now();
		auto now_time_t = std::chrono::system_clock::to_time_t(now);
		struct tm t;
		localtime_s(&t, &now_time_t);
		return t;
	}
	std::string GetCurTimeStr()
	{
		auto now = std::chrono::system_clock::now();
		auto now_time_t = std::chrono::system_clock::to_time_t(now);
		struct tm t;
		localtime_s(&t, &now_time_t);

		char timeStr[FILE_NAME_LEN];
		sprintf_s(timeStr, "%04d%02d%02d_%02d%02d%02d", t.tm_year + 1900, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
		return timeStr;
	}

	std::string GetLogFileName(const char* LogName)
	{
		char fileName[FILE_NAME_LEN];
		sprintf_s(fileName, "%s_%s.txt", LogName, GetCurTimeStr().c_str());
		return fileName;
	}
	std::vector<std::string> SplitStringUsingStrTok(std::string_view source, std::string_view token)
	{
		std::vector<std::string> result;
		size_t pos = source.find(token, 0);
		size_t startPos = 0;
		size_t tokenSize = token.size();
		while (pos == std::string::npos)
		{
			result.push_back(std::string(source.substr(startPos, pos - startPos)));
			size_t startPos = pos + tokenSize;
			pos = source.find(token, startPos);
		}
		return result;
	}
	std::vector<std::string> SplitString(std::string_view source, char token)
	{
		std::vector<std::string> result;
		std::stringstream sourceStream;
		sourceStream << source;
		std::string subString;
		while (std::getline(sourceStream, subString, token))
		{
			result.push_back(subString);
		}
		return result;
	}
}
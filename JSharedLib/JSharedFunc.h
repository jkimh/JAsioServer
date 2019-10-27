#pragma once
#include <iostream>
namespace JSharedFunc
{
	tm GetCurTimeStruct();
	std::string GetCurTimeStr();
	std::string GetLogFileName(const char* LogName);
	//std::vector<std::string> SplitString(const std::string& source, const std::string& token);
	std::vector<std::string> SplitString(std::string_view source, char token = ' ');
	std::vector<std::string> SplitStringUsingStrTok(std::string_view source, std::string_view token);
}
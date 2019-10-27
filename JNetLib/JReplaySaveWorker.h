#pragma once
#include <JSharedFunc.h>
#include <JLogger.h>
#include <optional>
struct JReplay_Info
{
	uint64_t tickCount;	//20자리
	uint32_t sessionID; //10자리
	std::vector<char> packetBuffer;
	//json으로 변경 예정
	static std::vector<char> Serialize(const JReplay_Info& info)
	{
		std::vector<char> result;
		std::string infos = std::to_string(info.tickCount) + "|" + std::to_string(info.sessionID).c_str() + "|"; //1|2|buffer
		result.insert(result.end(), infos.begin(), infos.end());
		result.insert(result.end(), info.packetBuffer.begin(), info.packetBuffer.end());
		return result;
	}
	static std::optional<JReplay_Info> DeSerialize(std::string_view buffer)
	{
		JReplay_Info info;
		auto strList = JSharedFunc::SplitString(buffer, '|');
		if (strList.size() != 3)
		{
			if (buffer != "")
			{
				JLogger.Error("wrong format (%s)", strList[0].c_str());
			}
			return {};
		}
		info.tickCount = static_cast<uint64_t>(std::atoi(strList[0].c_str()));
		info.sessionID = static_cast<uint32_t>(std::atoi(strList[1].c_str()));
		info.packetBuffer.insert(info.packetBuffer.end(), strList[2].begin(), strList[2].end());
		return info;
	}
};

bool operator<(JReplay_Info a, JReplay_Info b);
bool operator>(JReplay_Info a, JReplay_Info b);
//패킷을 파일로 저장하는 클래스 (thread 이용에 주의)
class JReplaySaveWorker
{
public:
	JReplaySaveWorker();
	~JReplaySaveWorker();
	void SetFileName(const char* fileName);
	void Push(const uint64_t& tickCount, const uint32_t& sessionID, const size_t& buffersize, const char* buffer);
	void RunThread();
	void End();
private:
	void Run();

private:
	std::mutex m_mutexBuffer;
	std::condition_variable m_cvBuffer;

	std::vector<JReplay_Info> m_packetBufferInfos;
	std::vector<JReplay_Info> m_packetBufferInfosForProcess;
	std::thread *m_workerThread;
	std::string m_saveFileName;
	
	bool m_end;
	bool m_isRunning;
};

JReplaySaveWorker& GetReplaySaveWorker();
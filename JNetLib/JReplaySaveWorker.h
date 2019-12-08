#pragma once

struct JReplay_Info
{
	uint64_t tickCount;	//20자리
	uint32_t sessionID; //10자리
	std::vector<char> packetBuffer;
	//json으로 변경 예정
	static std::vector<char> Serialize(const JReplay_Info& info);
	static JReplay_Info DeSerialize(const std::string& buffer); // std::optional<JReplay_Info>
	bool operator==(bool b);
};

bool operator<(JReplay_Info a, JReplay_Info b);
bool operator>(JReplay_Info a, JReplay_Info b);
//패킷을 파일로 저장하는 클래스 (thread 이용에 주의)
class IReplaySaverWorker
{
public:
	virtual void SetFileName(const char* fileName) = 0;
	virtual void Push(const uint64_t& tickCount, const uint32_t& sessionID, const size_t& buffersize, const char* buffer) = 0;
	virtual void RunThread() = 0;
	virtual void End() = 0; 
	virtual std::vector<JReplay_Info>& GetPacketList() = 0;
};
class JReplaySaveWorker : public IReplaySaverWorker
{
public:
	JReplaySaveWorker();
	~JReplaySaveWorker();
	virtual void SetFileName(const char* fileName) override;
	virtual void Push(const uint64_t& tickCount, const uint32_t& sessionID, const size_t& buffersize, const char* buffer) override;
	virtual void RunThread() override;
	virtual void End() override;
	virtual std::vector<JReplay_Info>& GetPacketList() override { return m_packetBufferInfos;	}
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



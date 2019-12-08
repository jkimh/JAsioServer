#pragma once
#include "JSCPacket.h"
#include "JTickClass.h"
using namespace JSharedLib;
class JServer;
class ISession;
class JCommanderCommand;
class JPlayerChar;
class JCommander
{
public:
	JCommander(const std::shared_ptr<ISession>& session, std::shared_ptr<JServer>& server);
	~JCommander(); 
	void Init(const char* name);
	void ProcessPacket(const JTickClass& tick);
	bool ProcessPacket(const JTickClass& tick, PACKET_HEADER* data);
	void Update();
	void Send(std::shared_ptr<PACKET_HEADER>& packet);
	bool IsReadyDie();
	bool IsDisConnected();
	void Close();
	std::shared_ptr<ISession>& GetSession() { return m_session;	};

private:
	void OnPacket(PKS_CS_LOGIN* packet);
	void OnPacket(PKS_CS_LOGOUT* packet);
	void OnPacket(PKS_CS_CHAT* packet);

private:
	std::shared_ptr<ISession> m_session;
	std::weak_ptr<JServer> m_server;
	std::shared_ptr<JCommanderCommand> m_processCommand;
	std::shared_ptr<JPlayerChar> m_char;
	std::string m_commanderName;
	bool m_isLogout;
};


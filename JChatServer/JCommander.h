#pragma once
#include "JSCPacket.h"
class JServer;
class JSession;
class JCommander
{
public:
	JCommander(const std::shared_ptr<JSession>& session, std::shared_ptr<JServer>& server);
	~JCommander(); 
	void Init(const char* name);
	void ProcessPacket();
	void Update();
	bool ProcessPacket(PACKET_HEADER* data);
	void Send(std::shared_ptr<PACKET_HEADER>& packet);
	bool IsReadyDie();
	bool IsDisConnected();
	void Close();

private:
	void OnPacket(PKS_CS_LOGIN* packet);
	void OnPacket(PKS_CS_LOGOUT* packet);
	void OnPacket(PKS_CS_CHAT* packet);

private:
	std::shared_ptr<JSession> m_session;
	std::weak_ptr<JServer> m_server;
	std::string m_commanderName;
	bool m_isLogout;
};


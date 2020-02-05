#pragma once
#include "JSCPacket.h"
#include "JTickClass.h"
using namespace JSharedLib;
using namespace std;
class JServer;
class ISession;
class JCommanderCommand;
class JPlayerChar;
class JCommander : public std::enable_shared_from_this<JCommander>
{
public:
	JCommander(const shared_ptr<ISession>& session, shared_ptr<JServer>& server);
	~JCommander(); 
	void Init(const char* name);
	void ProcessPacket(const JTickClass& tick);
	bool ProcessPacket(const JTickClass& tick, PACKET_HEADER* data);
	void Update(const JTickClass& tick);
	void Send(const shared_ptr<PACKET_HEADER>& packet);
	void Send(const shared_ptr<PACKET_HEADER>& packet, int size);
	bool IsReadyDie();
	bool IsDisConnected();
	void Close();
	shared_ptr<ISession>& GetSession() { return m_session;	};
	void BroadCast_ToZoneServer(const shared_ptr<PACKET_HEADER>& packet);
	const Vector3& GetPosition();
	void UpdateViews(const vector<shared_ptr<JCommander>>& commanderList);
	const string& GetCommanderName() { return m_commanderName; }

private:
	void OnPacket(PKS_CS_LOGIN* packet);
	void OnPacket(PKS_CS_LOGOUT* packet);
	void OnPacket(PKS_CS_CHAT* packet);
	void OnPacket(PKS_CS_MOVE* packet);

private:
	shared_ptr<ISession> m_session;
	weak_ptr<JServer> m_server;
	shared_ptr<JCommanderCommand> m_processCommand;
	shared_ptr<JPlayerChar> m_char;
	vector<weak_ptr<JCommander>> m_views; 
	string m_commanderName;
	bool m_isLogout;
};


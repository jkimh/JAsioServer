#include "pch.h"
#include "JReplaySaveWorkerMgr.h"
#include "JServerForUnitTest.h"
#include "JTestSession.h"
#include "JCommander.h"
#include "JTickClass.h"
#include "JSCPacket.h"
#include "JReplaySession.h"
#include "JReplayServer.h"
#include "JReplayReader.h"

using namespace std;

//1pc���� �׽�Ʈ Ŭ����
//1pc�� ��� ��� üũ ������ �Ȱ��� üũ�� �� �ֱ� ������ �Ʒ��� ���� ��� üũ �Լ��� �־��ִ� ���� �����ϴ�.
//pc�� �������̶�� �ٸ� ����� ����� ��.
void SetTestSessionPacketLogin(shared_ptr<JTestSession>& session, const char* testname,
	vector<function<void(shared_ptr<JServer>&, shared_ptr<ISession>&)>>& checkResult);
void SetTestSessionPacketChat(shared_ptr<JTestSession>& session, const char* teststring,
	vector<function<void(shared_ptr<JServer>&, shared_ptr<ISession>&)>>& checkResult);
void SetTestSessionPacketLogout(shared_ptr<JTestSession>& session,
	vector<function<void(shared_ptr<JServer>&, shared_ptr<ISession>&)>>& checkResult);
TEST(ReplayTest, Replay_Login_Chat_Logout_1PC)
{
	const int testTickCount = 50;
	GetReplaySaveWorker().Init(true);
	auto server = make_shared<JServerForUnitTest>();
	auto session = make_shared<JTestSession>();
	auto commander = make_shared<JCommander>(dynamic_pointer_cast<ISession>(session), dynamic_pointer_cast<JServer>(server));	
	server->AddCommander(commander);
	vector<function<void(shared_ptr<JServer>&, shared_ptr<ISession>&)>> checkResultFuncList;

	const char name[32] = "test1";
	const char teststr[128] = "test string";
	SetTestSessionPacketLogin(session, name, checkResultFuncList);
	SetTestSessionPacketChat(session, teststr, checkResultFuncList);
	SetTestSessionPacketLogout(session, checkResultFuncList);

	//session�� commander�� �ı��ǵ� shared_ptr�� �����־ �����ȴ�. �̸� ���� ����� üũ
	commander = nullptr;
	JTickClass tick;
	tick.now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	for (int i = 0; i < testTickCount; i++)
	{
		//������ 0.1�� �����ٰ� ����
		tick.tickCount++;
		tick.elapsedTime = 100;
		tick.now += 100;

		server->PreUpdateCommanders(tick);
		server->ProcessPacket(tick);
		server->UpdateCommanders(tick);
	}

	for (auto checkfunc : checkResultFuncList)
	{
		checkfunc(dynamic_pointer_cast<JServer>(server), dynamic_pointer_cast<ISession>(session));
	}
	/*
	{
		//�α��� üũ
		auto loginResultPacket = static_pointer_cast<PKS_SC_LOGIN_ACK>(session->PopLastSendPacket());
		EXPECT_NE(loginResultPacket, nullptr);
		EXPECT_EQ(loginResultPacket->command, PACKET_COMMAND::PACKET_SC_LOGIN_ACK);
		//ä�� üũ
		auto chatResultPacket = static_pointer_cast<PKS_SC_CHAT>(session->PopLastSendPacket());
		EXPECT_NE(chatResultPacket, nullptr);
		EXPECT_EQ(chatResultPacket->command, PACKET_COMMAND::PACKET_SC_CHAT);
		EXPECT_STREQ(chatResultPacket->chat, teststr);
		//�α׾ƿ� üũ
		EXPECT_EQ(server->GetCommanderCount(), 0);
	}
	*/
	//���÷��� ���� �ҷ�����
	auto packetList = GetReplaySaveWorker().GetPacketList();
	GetReplayReader().LoadReplayInfo(packetList);
	GetReplaySaveWorker().End();
	auto serverReplay = make_shared<JReplayServer>();
	tick.tickCount = 0;
	tick.elapsedTime = 0;
	tick.now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	std::vector<std::shared_ptr<JReplaySession>> replaySessionList;
	for (int i = 0; i < testTickCount; i++)
	{
		tick.tickCount++;
		tick.elapsedTime = 100;
		tick.now += 100;

		serverReplay->PreUpdateCommandersForTest(tick, replaySessionList);
		serverReplay->ProcessPacket(tick);
		serverReplay->UpdateCommanders(tick);
	}
	EXPECT_EQ(replaySessionList.size(), 1);
	for (auto checkfunc : checkResultFuncList)
	{
		checkfunc(dynamic_pointer_cast<JServer>(serverReplay), dynamic_pointer_cast<ISession>(replaySessionList[0]));
	}
	/*
	{
		EXPECT_EQ(replaySessionList.size(), 1);
		//�α��� üũ
		auto loginResultPacket = static_pointer_cast<PKS_SC_LOGIN_ACK>(replaySessionList[0]->PopLastSendPacket());
		EXPECT_NE(loginResultPacket, nullptr);
		EXPECT_EQ(loginResultPacket->command, PACKET_COMMAND::PACKET_SC_LOGIN_ACK);
		//ä�� üũ
		auto chatResultPacket = static_pointer_cast<PKS_SC_CHAT>(replaySessionList[0]->PopLastSendPacket());
		EXPECT_NE(chatResultPacket, nullptr);
		EXPECT_EQ(chatResultPacket->command, PACKET_COMMAND::PACKET_SC_CHAT);
		EXPECT_STREQ(chatResultPacket->chat, teststr);
		//�α׾ƿ� üũ
		EXPECT_EQ(serverReplay->GetCommanderCount(), 0);
	}
	*/
}

void SetTestSessionPacketLogin(shared_ptr<JTestSession>& session, const char* testname,
	vector<function<void(shared_ptr<JServer>&, shared_ptr<ISession>&)>>& checkResult)
{
	// login -> PKS_SC_LOGIN_ACK üũ
	PKS_CS_LOGIN loginPacket;
	loginPacket.command = PACKET_COMMAND::PACKET_CS_LOGIN;
	loginPacket.size = sizeof(PKS_CS_LOGIN);
	memset(loginPacket.commanderID, 0, 32);
	memcpy_s(loginPacket.commanderID, 32, testname, strlen(testname));
	session->AddRecvPacket(5, make_unique<PKS_CS_LOGIN>(loginPacket));

	checkResult.push_back([testname](shared_ptr<JServer>& server, shared_ptr<ISession>& session) {
		auto loginResultPacket = static_pointer_cast<PKS_SC_LOGIN_ACK>(session->PopLastSendPacket());
		EXPECT_STREQ(loginResultPacket->commanderID, testname);
		EXPECT_NE(loginResultPacket, nullptr);
		EXPECT_EQ(loginResultPacket->command, PACKET_COMMAND::PACKET_SC_LOGIN_ACK);
	});
}
void SetTestSessionPacketChat(shared_ptr<JTestSession>& session, const char* teststring,
	vector<function<void(shared_ptr<JServer>&, shared_ptr<ISession>&)>>& checkResult)
{
	//chat -> PKS_SC_CHAT üũ
	PKS_CS_CHAT packet;
	packet.command = PACKET_COMMAND::PACKET_CS_CHAT;
	packet.size = sizeof(PKS_CS_CHAT);
	memset(packet.chat, 0, 128);
	memcpy_s(packet.chat, 128, teststring, strlen(teststring));
	session->AddRecvPacket(15, make_unique<PKS_CS_CHAT>(packet));

	checkResult.push_back([teststring](shared_ptr<JServer>& server, shared_ptr<ISession>& session) {
		auto chatResultPacket = static_pointer_cast<PKS_SC_CHAT>(session->PopLastSendPacket());
		EXPECT_NE(chatResultPacket, nullptr);
		EXPECT_EQ(chatResultPacket->command, PACKET_COMMAND::PACKET_SC_CHAT);
		EXPECT_STREQ(chatResultPacket->chat, teststring);
	});

}
void SetTestSessionPacketLogout(shared_ptr<JTestSession>& session,
	vector<function<void(shared_ptr<JServer>&, shared_ptr<ISession>&)>>& checkResult)
{
	PKS_CS_LOGOUT logoutPacket;
	logoutPacket.command = PACKET_COMMAND::PACKET_CS_LOGOUT;
	logoutPacket.size = sizeof(PKS_CS_LOGOUT);
	session->AddRecvPacket(25, make_unique<PKS_CS_LOGOUT>(logoutPacket));

	checkResult.push_back([](shared_ptr<JServer>& server, shared_ptr<ISession>& session) {
		EXPECT_EQ(server->GetCommanderCount(), 0);
	});
}
#include "pch.h"
#include "JCommander.h"
#include "JServer.h"
#include "JServerForUnitTest.h"
#include "ISession.h"
#include "JTestSession.h"
#include "JSCPacket.h"

using namespace std;

TEST(CommanderTest, Chat)
{
	auto server = make_shared<JServerForUnitTest>();
	auto session = make_shared<JTestSession>();
	auto commander = make_shared<JCommander>(dynamic_pointer_cast<ISession>(session), dynamic_pointer_cast<JServer>(server));
	server->AddCommander(commander);
	const char teststr[64] = "test string";
	PKS_CS_CHAT packet;
	packet.command = PACKET_COMMAND::PACKET_CS_CHAT;
	packet.size = sizeof(PKS_CS_CHAT);
	memset(packet.chat, 0, 128);
	memcpy_s(packet.chat, 128, teststr, strlen(teststr));
	commander->ProcessPacket(1, &packet);
	auto resultPacket = static_pointer_cast<PKS_SC_CHAT>(session->GetLastSendPacket());
	EXPECT_NE(resultPacket, nullptr);
	EXPECT_EQ(resultPacket->command, PACKET_COMMAND::PACKET_SC_CHAT);
	EXPECT_STREQ(resultPacket->chat, packet.chat);
}
#pragma once
#include "JPacket.h"

struct PKS_CS_LOGIN : PACKET_HEADER
{
	char commanderID[32];
};
struct PKS_SC_LOGIN_ACK : PACKET_HEADER
{
	char commanderID[32];
};
struct PKS_CS_LOGOUT : PACKET_HEADER
{
};

struct PKS_CS_CHAT : PACKET_HEADER
{
	char chat[128];
};

struct PKS_SC_CHAT : PACKET_HEADER
{
	char chat[128];
};
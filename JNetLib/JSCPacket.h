#pragma once
#include "JPacket.h"
#include "Vector3.h"
using namespace JSharedLib;

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

struct PKS_SC_MOVE : PACKET_HEADER
{
	Vector3 dest;
};

struct PKS_CS_MOVE : PACKET_HEADER
{
	Vector3 dest;
};

//�ӽ÷� 5��������
struct PKS_SC_VIEW : PACKET_HEADER
{
	char commanderID1[5][32];
	Vector3  position1[5];
};
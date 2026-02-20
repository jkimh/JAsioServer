#pragma once
#include <string.h>
#include <cstdint>

enum PACKET_COMMAND
{
	PACKET_CS_LOGIN,
	PACKET_SC_LOGIN_ACK,
	PACKET_CS_LOGOUT,
	PACKET_CS_CHAT,
	PACKET_SC_CHAT,
	PACKET_CS_MOVE,
	PACKET_SC_MOVE,
	PACKET_SC_VIEW
};

#pragma pack(push, 1)
struct PACKET_HEADER
{
	PACKET_HEADER() {}
	PACKET_HEADER(PACKET_COMMAND _command, uint32_t _size)
	{
		command = _command;
		size = _size;
	}
	PACKET_COMMAND command;
	uint32_t size;
};
#pragma pack(pop)
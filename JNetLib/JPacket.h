#pragma once
#include <string.h>
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

struct PACKET_HEADER
{
	PACKET_HEADER() {}
	PACKET_HEADER(PACKET_COMMAND _command, size_t _size)
	{
		command = _command;
		size = _size;
	}
	PACKET_COMMAND command;
	size_t size;
};
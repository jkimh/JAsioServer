#include "stdafx.h"
#include "JPlayerChar.h"
#include "JServer.h"
#include "JCommander.h"


JPlayerChar::JPlayerChar(const std::shared_ptr<JCommander>& owner)
	: m_owner(owner)
{
}


JPlayerChar::~JPlayerChar()
{
}

void JPlayerChar::Update(const JTickClass& tick)
{
	JCharacter::Update(tick);
}


void JPlayerChar::Move(const Vector3& direction, float distance)
{
	JCharacter::Move(direction, distance);

	auto userPtr = m_owner.lock();
	if (userPtr)
	{
		auto out = std::make_shared<PKS_SC_MOVE>();
		out->command = PACKET_COMMAND::PACKET_SC_MOVE;
		out->size = sizeof(PKS_SC_MOVE);
		out->dest = m_positionCur;
		auto packetPtr = std::dynamic_pointer_cast<PACKET_HEADER>(out);
		//userPtr->BroadCast_ToZoneServer(packetPtr);
		userPtr->Send(packetPtr); //임시로 자신 위치만 표기
	}
}
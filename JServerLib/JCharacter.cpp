#include "stdafx.h"
#include "JCharacter.h"
#include "JTickClass.h"

JCharacter::JCharacter()
	: m_positionCur(0.0f, 0.0f, 0.0f), m_positionDest(0.0f, 0.0f, 0.0f)
	, m_direction(0.0f, 0.0f, 0.0f), m_r(10.0f), m_speed(1.0f), m_isMoving(false)
{
}


JCharacter::~JCharacter()
{
}

void JCharacter::Update(const JTickClass& tick)
{
	if (m_isMoving)
	{
		if (m_positionDest == m_positionCur)
		{
			m_isMoving = false;
			return;
		}
		m_direction = JSharedLib::Normalize(m_positionDest - m_positionCur); 
		auto distance = sqrt(m_speed * tick.elapsedTime / 10000000);
		Move(m_direction, distance);
	}
}
/*
dest �������� ���� �̵� ��û
�����Ӹ��� dest �������� m_speed��ŭ �̵��Ѵ�.
���� ���� �� ���û
*/
void JCharacter::ReqMove(const Vector3& dest)
{
	m_positionDest = dest;
	m_isMoving = true;
}

void JCharacter::Move(const Vector3& direction, float distance)
{
	auto remainDistVector = m_positionDest - m_positionCur;
	if (remainDistVector.GetMagnitude() < distance)
	{
		m_positionCur = m_positionDest;
		return;
	}
	m_positionCur = m_positionCur + Vector3(direction.x * distance, direction.y * distance, direction.z * distance);
}
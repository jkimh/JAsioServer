#include "stdafx.h"
#include "JCharacter.h"
#include "JTickClass.h"

JCharacter::JCharacter()
	: m_positionCur(0.0f, 0.0f, 0.0f), m_positionDest(0.0f, 0.0f, 0.0f)
	, m_direction(0.0f, 0.0f, 0.0f), m_r(10.0f), m_speed(12.0f), m_isMoving(false)
{
}


JCharacter::~JCharacter()
{
}

void JCharacter::Update()
{

}
void JCharacter::ReqMove(const Vector3& dest)
{
	m_positionDest = dest;
	m_isMoving = true;
}

void JCharacter::Move(const Vector3& dest, float m_speed)
{

}
#pragma once
using namespace JSharedLib;

class JCharacter
{
public:
	JCharacter();
	~JCharacter();

	virtual void Update();

	void ReqMove(const Vector3& dest);
	void Move(const Vector3& dest, float m_speed);

private:
	Vector3 m_positionCur;
	Vector3 m_positionDest;
	Vector3 m_direction;
	float m_r;
	float m_speed;

	bool m_isMoving; //���߿� fsm�̳� �ൿƮ�� �������� ���� ����, ����� �̵��� ó���ϱ� ���� �ӽñ���
};

#pragma once
using namespace JSharedLib;

class JCharacter
{
public:
	JCharacter();
	~JCharacter();

	virtual void Update(const JTickClass& tick);

	void ReqMove(const Vector3& dest);
	const Vector3& GetPosition()
	{
		return m_positionCur;
	}
protected:
	virtual void Move(const Vector3& direction, float distance);

protected:
	Vector3 m_positionCur;
	Vector3 m_positionDest;
	Vector3 m_direction;
	float m_r;
	float m_speed;

	bool m_isMoving; //���߿� fsm�̳� �ൿƮ�� �������� ���� ����, ����� �̵��� ó���ϱ� ���� �ӽñ���
};

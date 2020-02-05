#pragma once
#include "JCharacter.h"

class JCommander;
class JPlayerChar : public JCharacter	
{
public:
	JPlayerChar(const std::shared_ptr<JCommander>& owner);
	~JPlayerChar();

	virtual void Update(const JTickClass& tick) override;
	virtual void Move(const Vector3& direction, float distance) override;
private:
	std::weak_ptr<JCommander> m_owner;
};


#pragma once
#include "JCharacter.h"
class JPlayerChar : public JCharacter	
{
public:
	JPlayerChar();
	~JPlayerChar();

	virtual void Update() override;
};


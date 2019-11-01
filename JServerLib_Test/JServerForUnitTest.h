#pragma once
#include "JServer.h"
class JServerForUnitTest :
	public JServer
{
public:
	JServerForUnitTest();
	~JServerForUnitTest();

	void AddCommander(std::shared_ptr<JCommander>& commander);
};


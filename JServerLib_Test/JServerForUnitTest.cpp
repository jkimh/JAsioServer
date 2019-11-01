#include "pch.h"
#include "JServerForUnitTest.h"


JServerForUnitTest::JServerForUnitTest()
{
}


JServerForUnitTest::~JServerForUnitTest()
{
}

void JServerForUnitTest::AddCommander(std::shared_ptr<JCommander>& commander)
{
	m_commanders.push_back(commander);
}

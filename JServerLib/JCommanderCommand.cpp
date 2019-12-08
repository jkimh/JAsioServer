#include "stdafx.h"
#include "JCommanderCommand.h"


JCommanderCommand::JCommanderCommand()
{
}


JCommanderCommand::~JCommanderCommand()
{
}

bool JLoginComCommand::Begin()
{
	//request Login
	return true;
}

bool JLoginComCommand::Update()
{
	//wait end login
	return true;
}

bool JLoginComCommand::End()
{
	//result process
	return true;
}

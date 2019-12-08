#pragma once
class JCommanderCommand
{
public:
	JCommanderCommand();
	~JCommanderCommand();
	virtual bool Begin() = 0;
	virtual bool Update() = 0;
	virtual bool End() = 0;
};

class JLoginComCommand : public JCommanderCommand
{
	// Inherited via JCommanderCommand
	virtual bool Begin() override;
	virtual bool Update() override;
	virtual bool End() override;
};
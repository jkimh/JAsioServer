#include "stdafx.h"
#include "JTickClass.h"

namespace JSharedLib
{
	JTickClass::JTickClass()
		:tickCount(0), elapsedTime(0), now(0)
	{
	}


	JTickClass::~JTickClass()
	{
	}

	void JTickClass::Update()
	{
		uint64_t _now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		elapsedTime = _now - now;
		now = _now;
		tickCount++;
	}
}
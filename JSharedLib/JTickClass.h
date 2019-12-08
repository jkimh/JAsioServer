#pragma once
namespace JSharedLib
{
	struct JTickClass
	{
		JTickClass();
		JTickClass(JTickClass& in)
		{
			tickCount = in.tickCount;
			elapsedTime = in.elapsedTime;
			now = in.now;
		}
		~JTickClass();
		void Update();
		
		uint64_t tickCount;
		uint64_t now;
		uint64_t elapsedTime;
	};
}


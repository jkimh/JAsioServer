// JChatServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "JServer.h"
#include "JLogger.h"

using boost::asio::ip::tcp;
int main()
{
	JLogger.Init("Server", "Log_Server", true);
	boost::asio::io_context ioContext;
	JServer server(ioContext, 9001);
	std::thread thread(boost::bind(&boost::asio::io_context::run, &ioContext));

	UINT64 lastTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	double nsPerTick = 1000000000 / 12.0f; // 12프레임

	int ticks = 0;
	int frames = 0;
	int tickCount = 0;

	//UINT64 lastTimer = lastTime;

	double delta = 0;
	while (1)
	{
		UINT64 now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		delta += (now - lastTime) / nsPerTick;
		lastTime = now;
		while (delta >= 1)
		{
			ticks++;
			tickCount++;

			server.PreUpdateCommanders();
			server.ProcessPacket();
			server.UpdateCommanders();

			delta--;
		}
		try
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}
		catch (int e)
		{
			JLogger.Error("error : %d", e);
		}
		/*
		//초당 ticks 체크
		if (now - lastTimer >= 1000000000)
		{
			lastTimer += 1000000000;
			JLogger.Log("Ticks : %d, tickCount : %d", ticks, tickCount);
			ticks = 0;
		}
		*/
	}
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

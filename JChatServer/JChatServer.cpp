// JChatServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "JServer.h"
#include "JLogger.h"

using boost::asio::ip::tcp;
void ServerLogicThread(std::shared_ptr<JServer>& server);
int main()
{
	JLogger.Init("Server", "Log_Server", true);

	boost::asio::io_context ioContext;
	std::shared_ptr<JServer> server = std::make_shared<JServer>(ioContext, 9001);
	std::thread constextThread(boost::bind(&boost::asio::io_context::run, &ioContext));
	std::thread serverThread(std::bind(&ServerLogicThread, server));

	double delta = 0;
	while (1)
	{
		char temp[100];
		std::cin >> temp;

		if (0 == strncmp(temp, "end", 100))
		{
			serverThread.detach();
			constextThread.detach();
			break;
		}
	}
}

void ServerLogicThread(std::shared_ptr<JServer>& server)
{
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

			server->PreUpdateCommanders();
			server->ProcessPacket();
			server->UpdateCommanders();

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


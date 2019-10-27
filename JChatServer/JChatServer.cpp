// JChatServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "JServer.h"
#include "JLogger.h"
#include "JReplaySaveWorker.h"
#include "JReplayReader.h"
#include "JReplayServer.h"

using boost::asio::ip::tcp;
void ServerLogicThread(std::shared_ptr<JServer>& server);
int main(int argc, char* argv[])
{
	JLogger.Init("Server", "Log_Server", false);
	bool isReplayServer = false;
	bool useSavePacketForReplay = false;
	if (argc == 2)
	{
		if (strncmp(argv[1], "-ReplayServer", strlen("-ReplayServer")) == 0)
		{
			isReplayServer = true;
		}
		else if (strncmp(argv[1], "-ReplaySave", strlen("-ReplaySave")) == 0)
		{
			useSavePacketForReplay = true;
		}
	}

	if (useSavePacketForReplay)
	{
		GetReplaySaveWorker().SetFileName("packet_result.txt");
		GetReplaySaveWorker().RunThread();
	}

	boost::asio::io_context ioContext;
	std::thread *serverThread = nullptr;
	if (isReplayServer)
	{
		GetReplayReader().LoadReplayInfo("packet_result.txt");
		std::shared_ptr<JServer> server = std::dynamic_pointer_cast<JServer>(std::make_shared<JReplayServer>(ioContext, 9001));
		serverThread = new std::thread(std::bind(&ServerLogicThread, server));
	}
	else
	{
		std::shared_ptr<JServer> server = std::make_shared<JServer>(ioContext, 9001);
		serverThread = new std::thread(std::bind(&ServerLogicThread, server));
	}
	std::thread constextThread(boost::bind(&boost::asio::io_context::run, &ioContext));
	double delta = 0;
	while (1)
	{
		char temp[100];
		std::cin >> temp;

		if (0 == strncmp(temp, "end", 100))
		{
			serverThread->detach();
			constextThread.detach();
			if (useSavePacketForReplay)
			{
				GetReplaySaveWorker().End();
			}
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

			server->PreUpdateCommanders(tickCount);
			server->ProcessPacket(tickCount);
			server->UpdateCommanders(tickCount);

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


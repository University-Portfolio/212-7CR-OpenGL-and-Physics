#include "GameLogic.h"
#include "Logger.h"
#include "Misc.h"
#include "Game.h"

#include <chrono>
#include <time.h>


GameLogic::GameLogic()
	: tick_rate(60), 
	  total_sleep_time(1.0f/(float)(tick_rate))
{
}


GameLogic::~GameLogic()
{
	if (thread)
	{
		thread->join();
		delete thread;
		thread = nullptr;
	}
}

void GameLogic::Launch() 
{
	LOG(Log, "Launching GameLogic thread");
	thread = new std::thread(&GameLogic::LaunchMainLoop, this);
}

inline float GetTime()
{
	return glfwGetTime();
}

void GameLogic::LaunchMainLoop()
{
	g_game->SetLogicReady();

	//Wait until ready
	while (!g_game->IsReady())
	{
		int actual_sleep_time = (int)(total_sleep_time * 1000);
		std::this_thread::sleep_for(std::chrono::milliseconds(actual_sleep_time));
	}

	float last_update = GetTime();
	float delta_time = 0;
	float last_sleep_time = 0;


	while (!g_game->IsClosedRequested())
	{
		float start_time = GetTime();
		last_tick_end_time = start_time + total_sleep_time;

		//Tick logic
		Tick(delta_time);

		float current_time = GetTime();
		delta_time = current_time - last_update;
		last_update = current_time;


		last_sleep_time += total_sleep_time - (current_time - start_time);
		
		if (last_sleep_time > 0.0f)
		{
			int actual_sleep_time = (int)(last_sleep_time * 1000);

			float sleep_start = GetTime();
			std::this_thread::sleep_for(std::chrono::milliseconds(actual_sleep_time));
			last_sleep_time -= GetTime() - sleep_start;
		}

		if (last_sleep_time < 0)
			last_sleep_time = 0.0f;
	}

	//Ensure other threads have registered close request
	g_game->Close();

	LOG(Log, "Terminating GameLogic");

	//Clean up current world
	World* world = g_game->GetWorld();
	if (world)
		world->UnloadLogicResources(this);
}

void GameLogic::Tick(float delta_time)
{
	//Track ticks this second, for debug
	second_counter += delta_time;
	ticks_this_second++;

	if (second_counter > 1.0f)
	{
		second_counter -= 1.0f;
		ticks_last_second = ticks_this_second;
		ticks_this_second = 0;
	}

	World* world = g_game->GetWorld();
	if (world)
		world->LogicTick(this, delta_time);
}
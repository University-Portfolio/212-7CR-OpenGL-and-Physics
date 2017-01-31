#pragma once

#include <GLFW/glfw3.h>
#include <vec2.hpp>

class Window
{
private:
	GLFWwindow* window;
	glm::ivec2 desired_resolution;

	//Variables to keep track of current tick rate (debug logging)
	float second_counter = 0;
	int ticks_this_second = 0;
	int ticks_last_second = 0;

public:
	Window(const glm::ivec2 desired_resolution);
	~Window();

	void Launch();
	void SetVSync(const bool on);

	inline int GetCurrentTickRate() { return ticks_last_second; }

private:
	void LaunchMainLoop();
	void Tick(float delta_time);

};

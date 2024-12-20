#pragma once

#include <SDL2/SDL.h>
#include <iostream>

class Cust_Window
{
public:
	Cust_Window();
	~Cust_Window();


	void Run();
	bool IsValid() { return m_isValid; }

private:
	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	bool m_isValid = false;
	bool m_isRunning = false;


	void ProcessEvents();
};


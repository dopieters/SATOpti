#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include"Geom.h"

struct Polygon;

class Cust_Window
{
public:
	Cust_Window();
	~Cust_Window();


	void Run();
	bool IsValid() const { return m_isValid; }

private:
	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	bool m_isValid = false;
	bool m_isRunning = false;

	const int nVertices = 10;
	Polygon pol1;
	Polygon pol2;
	
	void DrawPolygons();
	void ProcessEvents();
};


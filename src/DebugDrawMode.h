#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include "Geom.h"

struct Polygon;

class DebugDrawMode
{
public:
	DebugDrawMode();
	~DebugDrawMode();

	// main function to run the debug mode
	void Run();

	// return whereever the initialization was successful
	bool IsValid() const { return m_isValid; }

private:
	// SDL wnd
	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	// Valid when m_window and m_isRunning 
	// mem properly allocated
	bool m_isValid = false;

	// Running variable
	// false to quit the run function
	bool m_isRunning = false;

	bool m_isPolIntersect = false;

	// Number of vertices for the convex polygons
	int nVertices = 100;

	// false hide min pol
	// true show min pol
	bool m_isShowMinPol = false;
	bool m_isShowMinPolDebug = false;


	void DrawPolygons() const;
	void DrawPolygon(const Polygon& A) const;
	void DrawHyperPlanes(const Vector v, const float min, const float max) const;

	// Printout the different commands
	void PrintCommand() const;
	void ProcessEvents();

	void MakePolygons();
	Polygon pol1;
	Polygon pol2;

	Polygon pol1Red;
	Polygon pol2Red;
};
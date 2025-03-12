#pragma once

#include "Geom.h"
#include "DrawWindow.h"


class DebugDrawMode final: public DrawWindow
{
public:
	DebugDrawMode();
	~DebugDrawMode();

	// main function to run the debug mode
	void Run();

private:

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
	

	// Printout the different commands
	void PrintCommand() const;
	void ProcessEvents();

	void MakePolygons();
	Geom::Polygon pol1;
	Geom::Polygon pol2;

	Geom::Polygon pol1Red;
	Geom::Polygon pol2Red;
};
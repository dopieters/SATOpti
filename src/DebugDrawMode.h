#pragma once

#include "Geom.h"
#include "DrawWindow.h"



// Draw debug mode
// Use to 
// - visualize polygons generation
// - visualize reduced SAT algorithm
// - debug collisions algorithm

class DebugDrawMode: public DrawWindow
{
public:
	DebugDrawMode();
	~DebugDrawMode();

	// main function to run the debug mode
	void Run();

protected:

	// Running variable
	// false to quit the run function
	bool bRunning = false;

	bool bPolIntersect = false;

	// Number of Vertices for the convex polygons
	int iVertices = 100;

	// false hide min pol
	// true show min pol
	bool bShowMinPol = false;
	bool bShowMinPolDebug = false;

	// Render the polygons on screen
	void DrawPolygons() const;
	

	// Printout the different commands
	void PrintCommand() const;

	// process the different events
	void ProcessEvents();

	void MakePolygons();
	Geom::Polygon PolygonA;
	Geom::Polygon PolygonB;

	Geom::Polygon ReducedPolygonA;
	Geom::Polygon ReducedPolygonB;
};
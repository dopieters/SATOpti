
#pragma once




#include "DrawWindow.h"




// Draw debug mode
// Use to 
// - visualize GJK different steps
// - debug GJK implementation
class DebugDrawModeGJK final : public DrawWindow {
public:
	DebugDrawModeGJK();
	~DebugDrawModeGJK();


	// Run the debug mode
	void Run();


private:
	bool bRunning = false;

	// processes the different events
	void ProcessEvents();

	// Function to generate the polygons
	void MakePolygons();

	// Step throw the events
	void UpdateSimplex();


	// the two polygons
	Geom::Polygon PolygonA;
	Geom::Polygon PolygonB;

	// The simplex to process GJK
	Geom::Simplex GJKSimplex;

	// GJK update direction
	Geom::Vector GJKDir = { 0, 1 };

};
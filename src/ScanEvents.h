#pragma once
#include "DrawWindow.h"
#include "Geom.h"
#include <vector>




class ScanEvents final : public DrawWindow{

public:
	ScanEvents(std::vector<std::pair<Polygon, Polygon>> pairPolygons);
	~ScanEvents();

	void ScanPairOfPolygons();

private:
	void ProcessEvents();
	void DrawPolygonPairs();


private:
	// pair to draw
	std::vector<std::pair<Polygon, Polygon>> m_EventToDraw;
	int currentPolPair = 0;

};

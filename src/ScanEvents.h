#pragma once
#include "DrawWindow.h"
#include "Geom.h"
#include <vector>


// Class that look for discrepancies between methods
// and save the events


class ScanEvents final : public DrawWindow{

public:
	ScanEvents(std::vector<std::pair<Geom::Polygon, Geom::Polygon>> pairPolygons, bool save = true);
	~ScanEvents();

	void ScanPairOfPolygons();

private:
	void ProcessEvents();
	void DrawPolygonPairs();

	void DoPolygonColTests();


private:
	// pair to draw
	std::vector<std::pair<Geom::Polygon, Geom::Polygon>> PolygonPairs;
	std::pair<Geom::Polygon, Geom::Polygon> RedPolygonsPair;
	int iCurrentPair = 0;
	bool bSave = true;

};

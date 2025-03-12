#pragma once
#include "DrawWindow.h"
#include "Geom.h"
#include <vector>




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
	std::vector<std::pair<Geom::Polygon, Geom::Polygon>> m_EventToDraw;
	std::pair<Geom::Polygon, Geom::Polygon> pairOfRedPol;
	int m_CurrentPolPair = 0;
	bool m_isSave = true;

};

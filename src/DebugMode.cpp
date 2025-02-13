#include "DebugMode.h"
#include <cassert>
#include "Params.h"

#include "MenuInterface.h"

#include "DebugDrawMode.h"
#include "ScanEvents.h"
#include "EventIO.h"


DebugMode::DebugMode():
	m_QuitDebugMode(false)
{
}

DebugMode::~DebugMode()
{
}

void DebugMode::Run()
{

	MenuInterface menu("Choose a debug mode");
	menu.AddOptions("Debug with drawing", [&]() {RunDrawDebugMode();});
	menu.AddOptions("Scan method consistency", [&]() {RunDebugMethodConsistency();});
	menu.AddOptions("Load events", [&]() {RunLoadEvent();});
	menu.AddOptions("Quit", [&]() {Quit();});

	while (!m_QuitDebugMode) {
		menu.RunInterface();
	}

}


void DebugMode::RunDrawDebugMode()
{
	DebugDrawMode db;
	if (db.IsValid()) {
		db.Run();
	}
}

void DebugMode::RunDebugMethodConsistency()
{
	std::vector<std::pair<Polygon, Polygon>> pairToDraw;

	{
		Polygon A, B;
		for (int nbTests = 0; nbTests < 1000; ++nbTests) {
			A = MakeConvexPol(100); B = MakeConvexPol(100);

			bool bIntersectBForce = PolygonsInterTestBForce(A, B);
			bool bIntersectSAT = PolygonInterTestSAT(A, B);
			bool bIntersectSATOpti = PolygonInterTestSATOpti(A, B);

			if (bIntersectBForce != bIntersectSAT || bIntersectBForce != bIntersectSATOpti) {
				std::cout << "Inconsistency Between methods observed" << std::endl;
				pairToDraw.emplace_back(std::pair<Polygon, Polygon>(A, B));
			}
		}
	}
	

	if (!pairToDraw.empty()) {
		std::cout << "A total of " << pairToDraw.size() << " tests have shown inconsistency";
		ScanEvents wnd(pairToDraw);
		if (wnd.IsValid()) { wnd.ScanPairOfPolygons(); }
	}



}

void DebugMode::RunLoadEvent()
{
	auto events = loadPairsOfPolygons("events.bin");

	if (events.empty()) return;

	std::cout << "A total of " << events.size() << " have been loaded";
	ScanEvents wnd(events, false);
	if (wnd.IsValid()) { wnd.ScanPairOfPolygons(); }

}

#include "DebugMode.h"
#include <cassert>
#include "Params.h"

#include "MenuInterface.h"
#include "EventIO.h"

#include "DebugDrawMode.h"
#include "ScanEvents.h"
#include "DebugDrawModeGJK.h"


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
	menu.AddOptions("Draw GJK debug mode", [&]() {RunDrawDebugGJK();});
	menu.AddOptions("Scan method consistency", [&]() {RunDebugMethodConsistency();});
	menu.AddOptions("Load events", [&]() {RunLoadEvent();});
	menu.AddOptions("Back to main menu", [&]() {Quit();});

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

void DebugMode::RunDrawDebugGJK()
{
	DebugDrawModeGJK db;
	if (db.IsValid()) {
		db.Run();
	}
}

void DebugMode::RunDebugMethodConsistency()
{
	std::vector<std::pair<Polygon, Polygon>> pairToDraw;

	{
		Polygon A, B;
		for (int nbTests = 0; nbTests < 10000; ++nbTests) {
			A = MakeConvexPol(20); B = MakeConvexPol(20);

			bool bIntersectBForce = PolygonsInterTestBForce(A, B);
			bool bIntersectSAT = PolygonInterTestSAT(A, B);
			bool bIntersectSATOpti = PolygonInterTestSATOpti(A, B);
			bool bIntersectGJK = PolygonInterTestGJK(A, B);
			bool bIntersectSATOptiItVertex = PolygonInterTestSATOptiItera(A, B);

			if (bIntersectBForce != bIntersectSAT || bIntersectBForce != bIntersectSATOpti || bIntersectBForce != bIntersectGJK
				|| bIntersectBForce != bIntersectSATOptiItVertex
				) {
				std::cout << "Inconsistency Between methods observed" << std::endl;
				pairToDraw.emplace_back(std::pair<Polygon, Polygon>(A, B));
			}
		}
	}
	

	if (!pairToDraw.empty()) {
		std::cout << "A total of " << pairToDraw.size() << " tests have shown inconsistency \n";
		ScanEvents wnd(pairToDraw);
		if (wnd.IsValid()) { wnd.ScanPairOfPolygons(); }
	}



}

void DebugMode::RunLoadEvent()
{
	auto events = loadPairsOfPolygons("events.bin");

	if (events.empty()) return;

	std::cout << "A total of " << events.size() << " have been loaded \n";
	ScanEvents wnd(events, false);
	if (wnd.IsValid()) { wnd.ScanPairOfPolygons(); }

}

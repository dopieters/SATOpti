#include "DebugMode.h"
#include <cassert>
#include "Params.h"

#include "MenuInterface.h"

#include "DebugDrawMode.h"


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
	menu.AddOptions("Load event", [&]() {RunDebugMethodConsistency();});
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
	Polygon A, B;
	for (int ii = 0; ii < 1000; ++ii) {
		A = MakeConvexPol(100); B = MakeConvexPol(100);

		bool bIntersectBForce = PolygonsInterTestBForce(A, B);
		bool bIntersectSAT = PolygonInterTestSAT(A, B);
		bool bIntersectSATOpti = PolygonInterTestSATOpti(A, B);

		if (bIntersectBForce != bIntersectSAT) {
			std::cout << "Brute force and SAT produce different results" << std::endl;
		}

		if (bIntersectBForce != bIntersectSATOpti) {
			std::cout << "Brute force and SATOpti produce different results" << std::endl;
		}



	}


}

void DebugMode::RunLoadEvent()
{
}

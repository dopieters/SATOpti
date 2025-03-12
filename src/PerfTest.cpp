#include "PerfTest.h"
#include "Utilities.h"
#include "matplot/matplot.h"


PerfTest::PerfTest(int nCollTest, std::vector<int> NbsVerticesTest):
	nTest(nCollTest), polNbVerticesTestList(NbsVerticesTest)
{
}


template<typename Func>
void PerfTest::TestMethod(const std::string& funcName, const std::vector<std::pair<Geom::Polygon, Geom::Polygon>>& pols, Func f)
{
	if (pols.empty()) return;

	int nInter = 0;
	float interTime = 0.f;
	float noInterTime = 0.f;
	for (const auto& pairPol : pols) {
		const auto resBForces = measureExecutionTime(f, pairPol.first,
			pairPol.second);

		if (resBForces.first) {
			interTime += resBForces.second;
			++nInter;
		}
		else {
			noInterTime += resBForces.second;
		}
	}
	interTime = nInter == 0 ? -1 : interTime / nInter;
	noInterTime = pols.size() - nInter == 0 ? -1 : noInterTime / nInter;
	PrintResult(funcName, pols[0].first.vertices.size(), interTime, noInterTime, nInter);
}

void PerfTest::Run()
{

	{
		std::vector<std::pair<Geom::Polygon, Geom::Polygon>> pols;
		pols.reserve(polNbVerticesTestList.size());

		for (const auto nVert : polNbVerticesTestList) {
			pols.clear();

			// gen polygons
			for (int ii = 0; ii < nTest; ++ii) {
				pols.push_back({ Geom::MakeConvexPol(nVert), Geom::MakeConvexPol(nVert) });
			}

			// measure times
			TestMethod("Brute force", pols, Geom::PolygonsInterTestBForce);
			TestMethod("SAT", pols, Geom::PolygonInterTestSAT);
			TestMethod("SAT Opti", pols, Geom::PolygonInterTestSATOpti);
			TestMethod("SAT Opti iterativ", pols, Geom::PolygonInterTestSATOptiItera);
			TestMethod("GJK", pols, Geom::PolygonInterTestGJK);

		}
	}

}

void PerfTest::PrintResult(const std::string& funcName, int polVert, float timeInter, float timeNoInter, int nInter) const
{
	std::cout << funcName << " with " << polVert << " vertices: \n"
		<< "- Number of polygon pair with intersect is : " << nInter << "\n"
		<< "- Average time with inter : " << timeInter << "ms \n"
		<< "- Average time with no inter : " << timeNoInter << "ms \n";
}

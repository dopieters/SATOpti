#include "PerfTest.h"
#include "Utilities.h"

PerfTest::PerfTest(int nCollTest, std::vector<int> NbsVerticesTest):
	nTest(nCollTest), polNbVerticesTestList(NbsVerticesTest)
{
}

void PerfTest::Run()
{

	{
		std::vector<std::pair<Polygon, Polygon>> pols;
		pols.reserve(polNbVerticesTestList.size());

		for (const auto nVert : polNbVerticesTestList) {
			pols.clear();

			// gen polygons
			for (int ii = 0; ii < nTest; ++ii) {
				pols.push_back({ MakeConvexPol(nVert), MakeConvexPol(nVert) });
			}

			// measure times
			{
				int nInter = 0;
				float interTime = 0.f;
				float noInterTime = 0.f;
				for (const auto& pairPol : pols) {
					const auto resBForces = measureExecutionTime(PolygonsInterTestBForce, pairPol.first,
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
				PrintResult("Brute force", nVert, interTime, noInterTime);
			}

			{
				int nInter = 0;
				float interTime = 0.f;
				float noInterTime = 0.f;
				for (const auto& pairPol : pols) {
					const auto resBForces = measureExecutionTime(PolygonInterTestSAT, pairPol.first,
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
				PrintResult("SAT", nVert, interTime, noInterTime);
			}

			{
				int nInter = 0;
				float interTime = 0.f;
				float noInterTime = 0.f;
				for (const auto& pairPol : pols) {
					const auto resBForces = measureExecutionTime(PolygonInterTestSATOpti, pairPol.first,
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
				PrintResult("SAT opti", nVert, interTime, noInterTime);
			}

		}
	}
}

void PerfTest::PrintResult(const std::string& funcName, int polVert, float timeInter, float timeNoInter) const
{
	std::cout << funcName << " with " << polVert << " vertices: \n"
		<< "- Average time with inter : " << timeInter << "ms \n"
		<< "- Average time with no inter : " << timeNoInter << "ms \n";
}

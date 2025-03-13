#include "PerfTest.h"
#include "Utilities.h"
#include "matplot/matplot.h"


PerfTest::PerfTest(int nCollTest, std::vector<int> NbsVerticesTest):
	nTest(nCollTest), polNbVerticesTestList(NbsVerticesTest)
{
}


template<typename Func>
void PerfTest::TestMethod(const std::string& funcName, const std::vector<std::pair<Geom::Polygon, Geom::Polygon>>& pols, Func f, float& avgTimeInter, float& avgTimeNoInter)
{
	if (pols.empty()) return;

	int nInter = 0;
	avgTimeInter = 0.f;
	avgTimeNoInter = 0.f;
	for (const auto& pairPol : pols) {
		const auto resBForces = measureExecutionTime(f, pairPol.first,
			pairPol.second);

		if (resBForces.first) {
			avgTimeInter += resBForces.second;
			++nInter;
		}
		else {
			avgTimeNoInter += resBForces.second;
		}
	}
	avgTimeInter = nInter == 0 ? -1 : avgTimeInter / nInter;
	avgTimeNoInter = pols.size() - nInter == 0 ? -1 : avgTimeNoInter / nInter;
	PrintResult(funcName, pols[0].first.vertices.size(), avgTimeInter, avgTimeNoInter, nInter);
}

void PerfTest::Run()
{

	std::vector<float> BForceResInter(polNbVerticesTestList.size()), BForceResNoInter(polNbVerticesTestList.size());
	std::vector<float> BSATInter(polNbVerticesTestList.size()), BSATNoInter(polNbVerticesTestList.size());
	std::vector<float> BGJKInter(polNbVerticesTestList.size()), BGJKNoInter(polNbVerticesTestList.size());
	std::vector<float> BSATRedPolInter(polNbVerticesTestList.size()), BSATRedPolNoInter(polNbVerticesTestList.size());
	std::vector<float> BSATRedPolIterativeInter(polNbVerticesTestList.size()), BSATRedPolIterativeNoInter(polNbVerticesTestList.size());


	std::vector<float> logX;
	{
		std::vector<std::pair<Geom::Polygon, Geom::Polygon>> pols;
		pols.reserve(polNbVerticesTestList.size());


		

		int i = 0;
		for (const auto nVert : polNbVerticesTestList) {
			logX.push_back(std::log10(nVert));
			pols.clear();

			// gen polygons
			for (int ii = 0; ii < nTest; ++ii) {
				pols.push_back({ Geom::MakeConvexPol(nVert), Geom::MakeConvexPol(nVert) });
			}

			// measure times
			//TestMethod("Brute force", pols, Geom::PolygonsInterTestBForce, BForceResInter[i], BForceResNoInter[i]);
			//TestMethod("SAT", pols, Geom::PolygonInterTestSAT, BSATInter[i], BSATNoInter[i]);
			TestMethod("SAT Opti", pols, Geom::PolygonInterTestSATOpti, BSATRedPolInter[i], BSATRedPolNoInter[i]);
			TestMethod("SAT Opti iterativ", pols, Geom::PolygonInterTestSATOptiItera, BSATRedPolIterativeInter[i], BSATRedPolIterativeNoInter[i]);
			TestMethod("GJK", pols, Geom::PolygonInterTestGJK, BGJKInter[i], BGJKNoInter[i]);

			++i;

		}
	}


	auto DrawResult = [&](const std::vector<std::vector<float>>& Yaxis, std::string title, float YaxisMax = -1) {
		//Create new fig
		auto figure = matplot::figure(true);
		figure->size(1200, 800);
		if (YaxisMax> 0) matplot::ylim({ 0, YaxisMax });
		matplot::title(title);
		matplot::ylabel("Time (ms)");
		matplot::xlabel("N_{vertices}");
		matplot::plot(polNbVerticesTestList, Yaxis);
		matplot::xticks({ 0, 1000, 5000, 10000 });
		auto l = ::matplot::legend({ "SAT Opti", "Iterative", "GJK" });
		l->location(matplot::legend::general_alignment::topleft);
		l->font_size(10);
		matplot::save(title +".png");
		matplot::show();

		};
	
	// Draw results when there is intersection
	{
		std::vector<std::vector<float>> YInter;
		//YInter.push_back(BForceResInter);
		//YInter.push_back(BSATInter);
		YInter.push_back(BSATRedPolInter);
		YInter.push_back(BSATRedPolIterativeInter);
		YInter.push_back(BGJKInter);
		DrawResult(YInter, "Polygon intersecting", std::max(BSATRedPolIterativeInter[polNbVerticesTestList.size() - 1], BGJKInter[polNbVerticesTestList.size() - 1]));

	}

	// Draw results when there is no intersection
	{
		std::vector<std::vector<float>> YNoInter;
		//YNoInter.push_back(BForceResNoInter);
		//YNoInter.push_back(BSATNoInter);
		YNoInter.push_back(BSATRedPolNoInter);
		YNoInter.push_back(BSATRedPolIterativeNoInter);
		YNoInter.push_back(BGJKNoInter);

		DrawResult(YNoInter, "Polygon not intersecting");
	}

}

void PerfTest::PrintResult(const std::string& funcName, int polVert, float timeInter, float timeNoInter, int nInter) const
{
	std::cout << funcName << " with " << polVert << " vertices: \n"
		<< "- Number of polygon pair with intersect is : " << nInter << "\n"
		<< "- Average time with inter : " << timeInter << "ms \n"
		<< "- Average time with no inter : " << timeNoInter << "ms \n";
}

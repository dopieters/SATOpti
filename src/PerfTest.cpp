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
	//PrintResult(funcName, pols[0].first.vertices.size(), avgTimeInter, avgTimeNoInter, nInter);
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
			//TestMethod("SAT Opti", pols, Geom::PolygonInterTestSATOpti, BSATRedPolInter[i], BSATRedPolNoInter[i]);
			TestMethod("SAT Opti iterative", pols, Geom::PolygonInterTestSATOptiItera, BSATRedPolIterativeInter[i], BSATRedPolIterativeNoInter[i]);
			TestMethod("GJK", pols, Geom::PolygonInterTestGJK, BGJKInter[i], BGJKNoInter[i]);

			++i;

		}
	}


	auto DrawResult = [&](const std::vector<std::vector<float>>& Yaxis, std::string title, const std::vector<std::string>& legend, float YaxisMax = -1) {
		//Create new fig
		auto figure = matplot::figure(true);
		figure->size(1200, 800);
		if (YaxisMax> 0) matplot::ylim({ 0, YaxisMax });
		matplot::title(title);
		matplot::ylabel("Time (ms)");
		matplot::xlabel("N_{vertices}");
		matplot::plot(polNbVerticesTestList, Yaxis);
		matplot::xticks({ 0, 100, 200, 300, 400, 500 });
		auto l = ::matplot::legend(legend);

		l->location(matplot::legend::general_alignment::topleft);
		l->font_size(10);
		matplot::save(title +".png");
		matplot::show();

		};
	//
	//// Draw results when there is intersection
	//{
	//	std::vector<std::vector<float>> YInter;
	//	std::vector<std::string> legend;
	//	//YInter.push_back(BForceResInter); legend.push_back("Brute force");
	//	YInter.push_back(BSATInter);legend.push_back("SAT");
	//	YInter.push_back(BSATRedPolInter); legend.push_back("SAT Opti");
	//	YInter.push_back(BSATRedPolIterativeInter); legend.push_back("Iterative");		
	//	YInter.push_back(BGJKInter); legend.push_back("GJK");
	//	DrawResult(YInter, "Polygon intersecting", legend);
	//		//std::max(BSATRedPolIterativeInter[polNbVerticesTestList.size() - 1], BGJKInter[polNbVerticesTestList.size() - 1]));

	//}

	//// Draw zoom version on small Y when there is no intersection
	//{
	//	std::vector<std::vector<float>> YInter;
	//	std::vector<std::string> legend;
	//	YInter.push_back(std::vector<float>(BSATInter.begin(), BSATInter.begin() + 5));legend.push_back("SAT");
	//	YInter.push_back(std::vector<float>(BSATRedPolInter.begin(), BSATRedPolInter.begin() + 5)); legend.push_back("SAT Opti");
	//	YInter.push_back(std::vector<float>(BSATRedPolIterativeInter.begin(), BSATRedPolIterativeInter.begin() + 5)); legend.push_back("Iterative");
	//	YInter.push_back(std::vector<float>(BGJKInter.begin(), BGJKInter.begin() + 5)); legend.push_back("GJK");

	//	DrawResult(YInter, "Polygon intersecting zoomed", legend);
	//}


	// Draw results when there is intersection with only GJK and SAT opti
	{
		std::vector<std::vector<float>> YInter;
		std::vector<std::string> legend;
		YInter.push_back(BSATRedPolIterativeInter); legend.push_back("Iterative");
		YInter.push_back(BGJKInter); legend.push_back("GJK");
		DrawResult(YInter, "Polygon Intersecting", legend);
	}


	//// Draw results when there is no intersection
	//{
	//	std::vector<std::vector<float>> YNoInter;
	//	std::vector<std::string> legend;
	//	//YNoInter.push_back(BForceResNoInter); legend.push_back("Brute force");
	//	YNoInter.push_back(BSATInter);legend.push_back("SAT");
	//	YNoInter.push_back(BSATRedPolNoInter); legend.push_back("SAT Opti");
	//	YNoInter.push_back(BSATRedPolIterativeNoInter); legend.push_back("Iterative");
	//	YNoInter.push_back(BGJKNoInter); legend.push_back("GJK");

	//	DrawResult(YNoInter, "Polygon not intersecting", legend);
	//}

	//// Draw zoom version on small Y when there is no intersection
	//{
	//	std::vector<std::vector<float>> YNoInter;
	//	std::vector<std::string> legend;
	//	YNoInter.push_back(std::vector<float>(BSATNoInter.begin(), BSATNoInter.begin() + 5));legend.push_back("SAT");
	//	YNoInter.push_back(std::vector<float>(BSATRedPolNoInter.begin(), BSATRedPolNoInter.begin() + 5)); legend.push_back("SAT Opti");
	//	YNoInter.push_back(std::vector<float>(BSATRedPolIterativeNoInter.begin(), BSATRedPolIterativeNoInter.begin() + 5)); legend.push_back("Iterative");
	//	YNoInter.push_back(std::vector<float>(BGJKNoInter.begin(), BGJKNoInter.begin() + 5)); legend.push_back("GJK");

	//	DrawResult(YNoInter, "Polygon not intersecting zoomed", legend);
	//}


	// Draw results when there is no intersection only sat opti gjk and no inter
	{
		std::vector<std::vector<float>> YNoInter;
		std::vector<std::string> legend;
		YNoInter.push_back(BSATRedPolIterativeNoInter); legend.push_back("Iterative");
		YNoInter.push_back(BGJKNoInter); legend.push_back("GJK");
		DrawResult(YNoInter, "Polygon not Intersecting", legend);
	}


}

void PerfTest::PrintResult(const std::string& funcName, int polVert, float timeInter, float timeNoInter, int nInter) const
{
	std::cout << funcName << " with " << polVert << " vertices: \n"
		<< "- Number of polygon pair with intersect is : " << nInter << "\n"
		<< "- Average time with inter : " << timeInter << "ms \n"
		<< "- Average time with no inter : " << timeNoInter << "ms \n";
}

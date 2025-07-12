#include "PerfTest.h"
#include "Utilities.h"
#include "matplot/matplot.h"
#include <filesystem>


PerfTest::PerfTest(int nCollTest, std::vector<int> NbsVerticesTest):
	nTest(nCollTest), polNbVerticesTestList(NbsVerticesTest)
{
	Initialize();
}

void PerfTest::Initialize() {
	//res.AddMethods("Brute force", Geom::PolygonsInterTestBForce);
	//res.AddMethods("SAT", Geom::PolygonInterTestSAT);
	//res.AddMethods("SAT Opti", Geom::PolygonInterTestSATOpti);
	res.AddMethods("SAT Opti iterative", Geom::PolygonInterTestSATOptiItera);
	res.AddMethods("GJK", Geom::PolygonInterTestGJK);



	std::filesystem::create_directories("../img");

}


void PerfTest::TestMethod(CollisionMethod& method, const std::vector<std::pair<Geom::Polygon, Geom::Polygon>>& pols)
{
	if (pols.empty()) return;

	std::vector<double> interTime;
	std::vector<double> noInterTime;

	for (const auto& pairPol : pols) {
		const auto resBForces = measureExecutionTime(method.fcn, pairPol.first, pairPol.second);

		if (resBForces.first) {
			interTime.push_back(resBForces.second);
		}
		else {
			noInterTime.push_back(resBForces.second);
		}
	}

	std::cout << "vertices " << pols[0].first.vertices.size() << "\n";
	std::cout << method.name << " " << interTime.size() << " pols intersection \n";
	std::cout << method.name << " " << noInterTime.size() << " pols not intersecting \n";


	auto CalcWelford = [](const std::vector<double>& data) -> std::pair<double, double> {
		double mean = 0.0;
		double M2 = 0.0;
		int n = 0;
		for (double x : data) {
			++n;
			double delta = x - mean;
			mean += delta / n;
			double delta2 = x - mean;
			M2 += delta * delta2;
		}
		if (n < 2) return { mean, 0.0 };
		return { mean, M2 / (n - 1) };
		};


	auto [avgInter, varInter] = CalcWelford(interTime);
	auto [avgNoInter, varNoInter] = CalcWelford(noInterTime);

	auto CalcAverage = [](std::vector<double> measures) -> double
		{
			const int size = measures.size();
			assert(size > 0);
			double avg = 0.f;
			for (auto time : measures) {

				avg += time;
			}

			avg /= size;
			return avg;
		};


	method.interRes.mu.push_back(avgInter);
	method.noInterRes.mu.push_back(avgNoInter);

	method.interRes.sigma.push_back(varInter);
	method.noInterRes.sigma.push_back(varNoInter);

}

void PerfTest::DrawResults(std::vector<std::string> methodNames)
{

	std::vector<std::string> markers = { "o", "s", "d", "^", "v", "*", "x" };
	std::vector<std::string> colors = { "blue", "red", "green", "magenta", "cyan", "black", "orange" };


	std::string methodName = "../img/";
	for (const auto& name : methodNames) {
		methodName += name;
	}

	

	auto DrawResult = [&](const bool isCollision) {
		//Create new fig
		auto figure = matplot::figure(true);
		matplot::hold(true);
		figure->size(1200, 800);
		for (const auto& name : methodNames) {
			int methodIndex = res.GetMethodIndexByName(name);
			if (methodIndex < 0) continue;

			const auto methRes = res.GetMethodResultsByIndex(methodIndex, isCollision);
			auto h = matplot::errorbar(polNbVerticesTestList, methRes.mu, methRes.sigma);
			h->display_name(name);

			//h->marker(markers[methodIndex % markers.size()]);
			h->color(colors[methodIndex % colors.size()]);
			std::cout << "Ploting method :" << name << std::endl;

		}


		matplot::legend()->location(matplot::legend::general_alignment::bottomright); // Show legend only if something was plotted

		const std::string title = isCollision ? "Colliding Polygons" : "Non Colliding Polygons";
		matplot::title(title);
		matplot::ylabel("Average time (ms)");
		matplot::xlabel("Polygon vertices");

		std::string img = methodName + title + ".png";
		img.erase(std::remove(img.begin(), img.end(), ' '), img.end());
		matplot::save(img);
		matplot::show();
		
		};



	DrawResult(true);
	DrawResult(false);

	
	
}

void PerfTest::Run()
{
	std::vector<std::pair<Geom::Polygon, Geom::Polygon>> pols;
	pols.reserve(polNbVerticesTestList.size());

	for (const auto nVert : polNbVerticesTestList) {
		pols.clear();

		// gen polygons
		for (int ii = 0; ii < nTest; ++ii) {
			pols.push_back({ Geom::MakeConvexPol(nVert), Geom::MakeConvexPol(nVert) });
		}


		for (auto& method: res.methods) {
			TestMethod(method, pols);
		}
	}


	DrawResults({ "SAT Opti iterative", "GJK" });

}



void PerfTest::PrintResult(const std::string& funcName, int polVert, float timeInter, float timeNoInter, int nInter) const
{
	std::cout << funcName << " with " << polVert << " vertices: \n"
		<< "- Number of polygon pair with intersect is : " << nInter << "\n"
		<< "- Average time with inter : " << timeInter << "ms \n"
		<< "- Average time with no inter : " << timeNoInter << "ms \n";
}

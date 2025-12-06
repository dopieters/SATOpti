#include "PerfTest.h"
#include "Utilities.h"
#include "matplot/matplot.h"
#include <filesystem>


PerfTest::PerfTest(int InNbTest, std::vector<int> InVertexCounts):
	iNbTest(InNbTest), VertexCounts(InVertexCounts)
{
	Initialize();
}

void PerfTest::Initialize() {
	//PerfResults.AddMethods("Brute force", Geom::PolygonsInterTestBForce);
	//PerfResults.AddMethods("SAT", Geom::PolygonInterTestSAT);
	//PerfResults.AddMethods("SAT Opti", Geom::PolygonInterTestSATOpti);
	PerfResults.AddMethods("SAT Opti iterative", Geom::PolygonInterTestSATOptiItera);
	PerfResults.AddMethods("GJK", Geom::PolygonInterTestGJK);



	std::filesystem::create_directories("../img");

}


void PerfTest::TestMethod(CollisionMethod& method, const std::vector<std::pair<Geom::Polygon, Geom::Polygon>>& pols)
{
	if (pols.empty()) return;

	std::vector<double> interTime;
	std::vector<double> noInterTime;

	for (const auto& pairPol : pols) {
		const auto resBForces = measureExecutionTime(method.Fcn, pairPol.first, pairPol.second);

		if (resBForces.first) {
			interTime.push_back(resBForces.second);
		}
		else {
			noInterTime.push_back(resBForces.second);
		}
	}

	std::cout << "Vertices " << pols[0].first.Vertices.size() << "\n";
	std::cout << method.sName << " " << interTime.size() << " pols intersection \n";
	std::cout << method.sName << " " << noInterTime.size() << " pols not intersecting \n";

	// Calculate the mean and variance in one pass
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


	method.IntersectionResult.mu.push_back(avgInter);
	method.NoIntersectionResult.mu.push_back(avgNoInter);

	method.IntersectionResult.sigma.push_back(varInter);
	method.NoIntersectionResult.sigma.push_back(varNoInter);

}

void PerfTest::DrawResults(std::vector<std::string> methodNames)
{

	const std::vector<std::string> markers = { "o", "s", "d", "^", "v", "*", "x" };
	const std::vector<std::string> colors = { "blue", "red", "green", "magenta", "cyan", "black", "orange" };


	std::string methodName = "../img/";
	for (const auto& sName : methodNames) {
		methodName += sName;
	}

	

	auto DrawResult = [&](const bool isCollision) {
		//Create new fig
		auto figure = matplot::figure(true);
		matplot::hold(true);
		figure->size(1200, 800);
		for (const auto& sName : methodNames) {
			int methodIndex = PerfResults.GetMethodIndexByName(sName);
			if (methodIndex < 0) continue;

			const auto methRes = PerfResults.GetMethodResultsByIndex(methodIndex, isCollision);
			auto h = matplot::errorbar(VertexCounts, methRes.mu, methRes.sigma);
			h->display_name(sName);

			//h->marker(markers[methodIndex % markers.size()]);
			h->color(colors[methodIndex % colors.size()]);
			std::cout << "Ploting method :" << sName << std::endl;

		}


		matplot::legend()->location(matplot::legend::general_alignment::bottomright); // Show legend only if something was plotted

		const std::string title = isCollision ? "Colliding Polygons" : "Non Colliding Polygons";
		matplot::title(title);
		matplot::ylabel("Average time (ms)");
		matplot::xlabel("Polygon Vertices");

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
	pols.reserve(VertexCounts.size());

	for (const auto nVert : VertexCounts) {
		pols.clear();

		// gen polygons
		for (int ii = 0; ii < iNbTest; ++ii) {
			pols.push_back({ Geom::MakeConvexPol(nVert), Geom::MakeConvexPol(nVert) });
		}


		for (auto& method: PerfResults.Methods) {
			TestMethod(method, pols);
		}
	}


	DrawResults({ "SAT Opti iterative", "GJK" });

}



void PerfTest::PrintResult(const std::string& funcName, int polVert, float timeInter, float timeNoInter, int nInter) const
{
	std::cout << funcName << " with " << polVert << " Vertices: \n"
		<< "- Number of polygon pair with intersect is : " << nInter << "\n"
		<< "- Average time with inter : " << timeInter << "ms \n"
		<< "- Average time with no inter : " << timeNoInter << "ms \n";
}

#pragma once
#include "Geom.h"
#include <vector>
#include <string>
#include <cassert>
#include"functional"


struct Results {
	std::vector<double> mu;
	std::vector<double> sigma;
};

struct CollisionMethod {
	std::string sName;
	std::function<bool(const Geom::Polygon& RESTRICT A, const Geom::Polygon& RESTRICT B) >  Fcn;

	Results IntersectionResult;
	Results NoIntersectionResult;
};


struct PerformanceResults {
	std::vector<CollisionMethod> Methods;
	
	void AddMethods(const std::string& InName, std::function<bool(const Geom::Polygon& RESTRICT A, const Geom::Polygon& RESTRICT B) > InFcn) {
		Results empty; empty.mu.clear(); empty.sigma.clear();
		Methods.emplace_back(CollisionMethod{ InName, InFcn, empty, empty});
	}


	int GetMethodIndexByName(const std::string& InName) {
		int Index = -1;
		for (int ii = 0; ii < Methods.size(); ++ii) {
			if (Methods[ii].sName == InName) {
				Index = ii;
				break;
			}
		}

		return Index;
	}

	Results& GetMethodResultsByIndex(int InInd, bool isIntersection) {
		// Make sure the mindex exist
		assert(InInd >= 0 && InInd < Methods.size());

		return isIntersection ? Methods[InInd].IntersectionResult: Methods[InInd].NoIntersectionResult;
	}

};


// Class that run the differents tests
class PerfTest
{
public:
	PerfTest(int InNbTest, std::vector<int> InVertexCounts);
	~PerfTest() {};

	void Run();

private:
	const int iNbTest;
	std::vector<int> VertexCounts;
	PerformanceResults PerfResults;

	void Initialize();
	void PrintResult(const std::string& funcName, int polVert, float timeInter, float timeNoInter, int nInter) const;
	void TestMethod(CollisionMethod& method, const std::vector<std::pair<Geom::Polygon, Geom::Polygon>>& pols);
	void DrawResults(std::vector<std::string> methodNames);
};



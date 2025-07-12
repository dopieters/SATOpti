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
	std::string name;
	std::function<bool(const Geom::Polygon& RESTRICT A, const Geom::Polygon& RESTRICT B) >  fcn;

	Results interRes;
	Results noInterRes;
};


struct PerformanceResults {
	std::vector<CollisionMethod> methods;
	
	void AddMethods(const std::string& name, std::function<bool(const Geom::Polygon& RESTRICT A, const Geom::Polygon& RESTRICT B) > fcn) {
		Results emptyRes; emptyRes.mu.clear(); emptyRes.sigma.clear();
		methods.emplace_back(CollisionMethod{ name, fcn, emptyRes, emptyRes});
	}


	int GetMethodIndexByName(const std::string& name) {
		int retIndex = -1;
		for (int ind = 0; ind < methods.size(); ++ind) {
			if (methods[ind].name == name) {
				retIndex = ind;
				break;
			}
		}

		return retIndex;
	}

	Results& GetMethodResultsByIndex(int ind, bool isIntersection) {
		// Make sure the mindex exist
		assert(ind >= 0 && ind < methods.size());

		return isIntersection ? methods[ind].interRes: methods[ind].noInterRes;
	}

};

class PerfTest
{
public:
	PerfTest(int nCollTest, std::vector<int> NbsVerticesTest);
	~PerfTest() {};

	void Run();

private:
	const int nTest;
	std::vector<int> polNbVerticesTestList;
	PerformanceResults res;

	void Initialize();
	void PrintResult(const std::string& funcName, int polVert, float timeInter, float timeNoInter, int nInter) const;
	void TestMethod(CollisionMethod& method, const std::vector<std::pair<Geom::Polygon, Geom::Polygon>>& pols);
	void DrawResults(std::vector<std::string> methodNames);
};



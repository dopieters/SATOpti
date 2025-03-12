#pragma once
#include "Geom.h"
#include <vector>
#include <string>


struct TestResults
{
	float avgTimeInter = 0.f;
	float avgTimeNoInter = 0.f;

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


	void PrintResult(const std::string& funcName, int polVert, float timeInter, float timeNoInter, int nInter) const;
	template <typename Func>
	void TestMethod(const std::string& funcName, const std::vector<std::pair<Geom::Polygon, Geom::Polygon>>& pairPol, Func f, float& avgTimeInter, float& avgTimeNoInter);
};



#pragma once
#include "Geom.h"
#include <vector>
#include <string>

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
	void TestMethod(const std::string& funcName, const std::vector<std::pair<Polygon, Polygon>>& pairPol, Func f);
};



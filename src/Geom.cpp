#include "Geom.h"
#include <random>
#include <cassert> // assert


Vertex MakeRandomVertexPt() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);

	Vertex pt = { dis(gen), dis(gen) };
	return pt;
}


bool CompareByAngle(const Point& a, const Point& b) {
	return atan2(a.y, a.x) < atan2(b.y, b.x);
}


Polygon MakeConvexPol(int nVertices) {
	assert(nVertices >= 3 && "A minimum of 3 vertices is needed");
	srand(unsigned(time(NULL)));

	const int nVert = std::max(nVertices, 3);

	Polygon pol;
	pol.vertices.reserve(nVert);

	// Generate two lists of random X and Y coordinates
	std::vector<float> x; x.reserve(nVert);
	std::vector<float> y; y.reserve(nVert);
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> disAxis(-1.f, 1.f);
		for (int ii = 0; ii < nVert; ++ii) {
			x.push_back(disAxis(gen));
			y.push_back(disAxis(gen));
		}
	}

	assert(x.size() == y.size() && "The same number of point should be generated");

	// Sort them
	std::sort(x.begin(), x.end());
	std::sort(y.begin(), y.end());

	//Isolate the extreme points
	float minX = x[0];
	float maxX = x.back();
	float minY = y[0];
	float maxY = y.back();

	// Randomly divide the interior points into two chains
	// Extract the vector components
	std::vector<float> xVec; xVec.reserve(nVert);
	std::vector<float> yVec; yVec.reserve(nVert);
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::bernoulli_distribution bDis(0.5); // generate boolean
		// xVec
		{
			float lastTop = minX; float lastBot = minX;
			for (int ii = 1; ii < nVert-1; ++ii) {
				if (bDis(gen)) {
					xVec.emplace_back(x[ii] - lastTop);
					lastTop = x[ii];
				}
				else {
					xVec.emplace_back(lastBot - x[ii]);
					lastBot = x[ii];
				}
			}
			xVec.emplace_back(maxX - lastTop);
			xVec.emplace_back(lastBot - maxX);

		}

		// yVec
		{

			float lastTop = minY; float lastBot = minY;
			for (int ii = 1; ii < nVert-1; ++ii) {
				if (bDis(gen)) {
					yVec.emplace_back(y[ii] - lastTop);
					lastTop = y[ii];
				}
				else {
					yVec.emplace_back(lastBot - y[ii]);
					lastBot = y[ii];
				}
			}

			yVec.emplace_back(maxY - lastTop);
			yVec.emplace_back(lastBot - maxY);
		}
	}

	// Randomly pair up the X- and Y-components
	std::vector<Vector> vecs; vecs.reserve(nVert);
	{
		std::random_device rd;
		std::shuffle(yVec.begin(), yVec.end(), rd);

		for (int ii = 0; ii < nVert; ++ii) {
			vecs.emplace_back(Vector{xVec[ii],yVec[ii]});
		}
	}

	// Sort the vectors by angle
	std::sort(vecs.begin(), vecs.end(), CompareByAngle);

	// Lay them end-to-end to form a polygon
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> disCenter(-1.f, 1.f);
	float polX = disCenter(gen), polY = disCenter(gen);
	for (const auto& vec : vecs) {
		polX += vec.x;
		polY += vec.y;
		pol.vertices.emplace_back(Vertex{polX, polY});
	}


	return pol;
}
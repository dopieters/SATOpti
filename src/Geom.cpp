#include "Geom.h"
#include <random>
#include <cassert> // assert
#include <limits> // numeric_limits


Vertex MakeRandomVertexPt() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);

	Vertex pt = { dis(gen), dis(gen) };
	return pt;
}

bool CompareByAngle(const Vertex& a, const Vertex& b)
{
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

bool DoPolygonsIntersects(const Polygon& A, const Polygon& B)
{
	bool bInterBForce = PolygonsInterTestBForce(A, B);
	bool bInterSAT = PolygonInterTestSAT(A, B);

	assert(bInterBForce == bInterSAT && "Intersection tests disagree");

	return bInterBForce;
}

bool PolygonsInterTestBForce(const Polygon& A, const Polygon& B)
{
	// Test if edges intersects
	{
		const int nAVert = A.vertices.size();
		const int nBVert = B.vertices.size();

		for (int i = 0; i < nAVert; ++i) {
			const int nextA = (i + 1) % nAVert;
			for (int j = 0; j < nBVert; ++j)
			{
				const int nextB = (j + 1) % nBVert;
				if (SegmentIntersect(A.vertices[i], A.vertices[nextA],
					B.vertices[j], B.vertices[nextB]))
				{
					return true;
				}
			}
		}
	}

	// Test if point inside each other
	{
		if (PolygonIncludeInEachOther(A, B)) {
			return true;
		}
	}


	// no intersection found
	return false;
}

bool PolygonInterTestSAT(const Polygon& A, const Polygon& B)
{
	std::vector<Vector> axisToTestAgainst;
	axisToTestAgainst.reserve(A.vertices.size() + B.vertices.size());

	// A's axes
	{
		int nVertA = A.vertices.size();
		for (int ii = 0; ii < nVertA; ++ii) {
			const Vector edgeVec = A.vertices[ii] - A.vertices[(ii + 1) % nVertA];
			axisToTestAgainst.emplace_back(Vector{-edgeVec.y , edgeVec.x});
		}
	}

	// B's axes
	{
		int nVertB = B.vertices.size();
		for (int ii = 0; ii < nVertB; ++ii) {
			const Vector edgeVec = B.vertices[ii] - B.vertices[(ii + 1) % nVertB];
			axisToTestAgainst.emplace_back(Vector{ -edgeVec.y , edgeVec.x });
		}
	}

	// Test axes
	for (const Vector axis : axisToTestAgainst) {
		auto AProj = GetMinMaxPolygonProjAxis(A, axis);
		auto BProj = GetMinMaxPolygonProjAxis(B, axis);

		if (AProj.first > BProj.second || AProj.second < BProj.first) {
			return false;
		}

	}

	return true;
}

std::pair<float, float> GetMinMaxPolygonProjAxis(const Polygon& A, Vector d)
{
	float minProj = std::numeric_limits<float>::infinity();
	float maxProj = - std::numeric_limits<float>::infinity();

	{
		int nVertA = A.vertices.size();
		for (int ii = 0; ii < nVertA; ++ii) {
			float proj = A.vertices[ii].x * d.x + A.vertices[ii].y * d.y;
			minProj = std::min(minProj, proj);
			maxProj = std::max(maxProj, proj);
		}
	}

	return std::pair<float, float>(minProj, maxProj);
}

float CrossProd2D(Vector Va, Vector Vb)
{
	return Va.x * Vb.y - Va.y * Vb.x;
}

bool SegmentIntersect(Point A, Point B, Point C, Point D)
{
	// check the winding of triangles ABD - ABC
	// and CDB - CDA
	if (CrossProd2D(B-A, D-A) * CrossProd2D(B-A, C-A) <= 0 
		&& CrossProd2D(D-C, A-C) * CrossProd2D(D-C, B-C) <= 0) {
		return true;
	}
	else {
		return false;
	}
}

bool PolygonIncludeInEachOther(const Polygon& A, const Polygon& B)
{
	// A inside B
	{
		const int nAVert = A.vertices.size();
		for (int ii = 0; ii < nAVert; ++ii) {
			if (IsPointInsidePolygon(A.vertices[ii], B)) {
				return true;
			}
		}
	}

	// B inside A
	{
		const int nBVert = B.vertices.size();
		for (int ii = 0; ii < nBVert; ++ii) {
			if (IsPointInsidePolygon(B.vertices[ii], A)) {
				return true;
			}
		}
	}

	return false;
}

bool IsPointInsidePolygon(Point A, const Polygon& pol)
{
	// Check if point A on the same side of the edges
	{
		float sign = CrossProd2D(A - pol.vertices[0], pol.vertices[1] - pol.vertices[0]);
		int NpolVert = pol.vertices.size();
		for (int ii = 1; ii < NpolVert; ++ii) {
			Point V1 = pol.vertices[ii];
			Point V2 = pol.vertices[(ii + 1) % NpolVert];

			float cross = CrossProd2D(A - V1, V2 - V1);
			if (sign * cross < 0) {
				return false;
			}
		}
	}

	return true;
}

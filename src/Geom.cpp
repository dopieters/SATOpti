#include <cassert> // assert
#include <limits> // numeric_limits
#include <functional>
#include <future>

#include "Geom.h"
#include "Utilities.h"


#include "Random.h"



bool CompareByAngle(const Vertex a, const Vertex b)
{
	return atan2(a.y, a.x) < atan2(b.y, b.x);
}


Polygon MakeConvexPol(int nVertices) {
	assert(nVertices >= 3 && "A minimum of 3 vertices is   ");

	const int nVert = std::max(nVertices, 3);

	Polygon pol;
	pol.vertices.reserve(nVert);

	// Generate two lists of random X and Y coordinates
	std::vector<float> x; x.reserve(nVert);
	std::vector<float> y; y.reserve(nVert);
	{
		static std::uniform_real_distribution<float> disAxis(-1.f, 1.f);
		for (int ii = 0; ii < nVert; ++ii) {
			x.push_back(RandomCustom::GetRdnFloatUniform(disAxis));
			y.push_back(RandomCustom::GetRdnFloatUniform(disAxis));
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
		// xVec
		{
			float lastTop = minX; float lastBot = minX;
			for (int ii = 1; ii < nVert-1; ++ii) {
				if (RandomCustom::Get5050TrueFalse()) {
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
				if (RandomCustom::Get5050TrueFalse()) {
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
		std::random_device& rd = RandomCustom::GetRandomDevice();
		std::shuffle(yVec.begin(), yVec.end(), rd);

		for (int ii = 0; ii < nVert; ++ii) {
			vecs.emplace_back(Vector{xVec[ii],yVec[ii]});
		}
	}

	// Sort the vectors by angle
	std::sort(vecs.begin(), vecs.end(), CompareByAngle);

	// Lay them end-to-end to form a polygon
	{
		static std::uniform_real_distribution<float> disCenter(-2.f, 2.f);
		float polX = RandomCustom::GetRdnFloatUniform(disCenter), polY = RandomCustom::GetRdnFloatUniform(disCenter);
		pol.baryCenter = { 0.f, 0.f };
		for (const auto& vec : vecs) {
			polX += vec.x;
			polY += vec.y;
			pol.vertices.emplace_back(Vertex{ polX, polY });
			pol.baryCenter += Vertex{ polX, polY };
		}
		// store baryCenter
		pol.baryCenter = (1.f / vecs.size()) * pol.baryCenter;
	}


	return pol;
}


bool DoPolygonsIntersects(const Polygon& RESTRICT A, const Polygon& RESTRICT B)
{
	assert(A.vertices.size() >= 3 && B.vertices.size() > 3 && "Pol min vertices is 3");

	bool bInterBForce = measureExecutionTime(PolygonsInterTestBForce, "Brute force", A, B);
	bool bInterSAT = measureExecutionTime(PolygonInterTestSAT, "SAT", A, B);
	bool bInterSATOpti = measureExecutionTime(PolygonInterTestSATOpti, "New SAT", A, B);

	assert(bInterBForce == bInterSAT && "Brut force and SAT test should agree with each other");
	assert(bInterBForce == bInterSAT && "Brut force and SAT Opti test should agree with each other");


	return bInterBForce;
}

bool PolygonsInterTestBForce(const Polygon& RESTRICT A, const Polygon& RESTRICT B)
{

	assert(A.vertices.size() >= 3 && B.vertices.size() > 3 && "Pol min vertices is 3");

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

	//// Test if point inside each other
	//{
	//	if (PolygonIncludeInEachOther(A, B)) {
	//		return true;
	//	}
	//}

	// Test if one vertex is inside the other polygon
	{
		if (IsPointInsidePolygon(A.vertices[0], B)
			|| IsPointInsidePolygon(B.vertices[0], A)) {
			return true;
		}
	}


	// no intersection found
	return false;
}

bool PolygonInterTestSAT(const Polygon& RESTRICT A, const Polygon& RESTRICT B)
{
	assert(A.vertices.size() >= 3 && B.vertices.size() > 3 && "Pol min vertices is 3");

	std::vector<Vector> axisToTestAgainst;
	axisToTestAgainst.reserve(A.vertices.size() + B.vertices.size());

	auto computePolEdgesNorm = [&axisToTestAgainst](const Polygon& RESTRICT pol ) {
		const int nVert = pol.vertices.size();
		int ii = 0;
		for (; ii+3 < nVert-1; ii+=4) {
			const Vector edgeVec1 = pol.vertices[ii] - pol.vertices[ii + 1];
			const Vector edgeVec2 = pol.vertices[ii+1] - pol.vertices[ii + 2];
			const Vector edgeVec3 = pol.vertices[ii+2] - pol.vertices[ii + 3];
			const Vector edgeVec4 = pol.vertices[ii+3] - pol.vertices[ii + 4];
			axisToTestAgainst.emplace_back(Vector{ -edgeVec1.y , edgeVec1.x });
			axisToTestAgainst.emplace_back(Vector{ -edgeVec2.y , edgeVec2.x });
			axisToTestAgainst.emplace_back(Vector{ -edgeVec3.y , edgeVec3.x });
			axisToTestAgainst.emplace_back(Vector{ -edgeVec4.y , edgeVec4.x });
		}

		for (; ii + 3 < nVert - 1; ii += 4) {
			const Vector edgeVec = pol.vertices[ii] - pol.vertices[ii + 1];
			axisToTestAgainst.emplace_back(Vector{ -edgeVec.y , edgeVec.x });
		}

		const Vector edgeVec = pol.vertices[ii] - pol.vertices[0];
		axisToTestAgainst.emplace_back(Vector{ -edgeVec.y , edgeVec.x });
		};

	// A's axes
	computePolEdgesNorm(A);

	// B's axes
	computePolEdgesNorm(B);

	// Test axes
	{
		//ScopeTimeMeasure test("Max");
		for (const Vector axis : axisToTestAgainst) {
			std::pair<float, float> AProj = { -GetMaxPolygonProjAxis(A, -axis), GetMaxPolygonProjAxis(A, axis) };
			std::pair<float, float> BProj = { -GetMaxPolygonProjAxis(B, -axis), GetMaxPolygonProjAxis(B, axis) };
			if (AProj.first > BProj.second || AProj.second < BProj.first) {
				return false;
			}

		}
	}

	return true;

}

bool PolygonInterTestSATForRedPol(const Polygon& RESTRICT A, const Polygon& RESTRICT B)
{
	assert(A.vertices.size() >= 3 && B.vertices.size() >= 3 && "Pol min vertices is 3");

	std::vector<Vector> axisToTestAgainst;
	axisToTestAgainst.reserve(A.vertices.size() + B.vertices.size());

	auto computePolEdgesNorm = [&axisToTestAgainst](const Polygon& RESTRICT pol) {
		const int nVert = pol.vertices.size();
		int ii = 0;
		for (; ii + 3 < nVert - 1; ii += 4) {
			const Vector edgeVec1 = pol.vertices[ii] - pol.vertices[ii + 1];
			const Vector edgeVec2 = pol.vertices[ii + 1] - pol.vertices[ii + 2];
			const Vector edgeVec3 = pol.vertices[ii + 2] - pol.vertices[ii + 3];
			const Vector edgeVec4 = pol.vertices[ii + 3] - pol.vertices[ii + 4];
			axisToTestAgainst.emplace_back(Vector{ -edgeVec1.y , edgeVec1.x });
			axisToTestAgainst.emplace_back(Vector{ -edgeVec2.y , edgeVec2.x });
			axisToTestAgainst.emplace_back(Vector{ -edgeVec3.y , edgeVec3.x });
			axisToTestAgainst.emplace_back(Vector{ -edgeVec4.y , edgeVec4.x });
		}

		for (; ii + 3 < nVert - 1; ii += 4) {
			const Vector edgeVec = pol.vertices[ii] - pol.vertices[ii + 1];
			axisToTestAgainst.emplace_back(Vector{ -edgeVec.y , edgeVec.x });
		}
		};

	// A's axes
	{
		computePolEdgesNorm(A);
	}

	// B's axes
	{
		computePolEdgesNorm(B);
	}

	// Test axes
	{
		for (const Vector axis : axisToTestAgainst) {
			std::pair<float, float> AProj = { -GetMaxPolygonProjAxis(A, -axis), GetMaxPolygonProjAxis(A, axis) };
			std::pair<float, float> BProj = { -GetMaxPolygonProjAxis(B, -axis), GetMaxPolygonProjAxis(B, axis) };
			if (AProj.first > BProj.second || AProj.second < BProj.first) {
				return false;
			}

		}
	}

	return true;
}


bool PolygonInterTestSATOpti(const Polygon& RESTRICT A, const Polygon& RESTRICT B)
{
	assert(A.vertices.size() >= 3 && B.vertices.size() > 3 && "Pol min vertices is 3");

	Vector barAxis = B.baryCenter - A.baryCenter;

	auto aMax = GetMaxPolygonProjAxis(A, barAxis);
	auto bMin = - GetMaxPolygonProjAxis(B, -barAxis);

	// check if barycenter inside each other
	if (IsPointInsidePolygon(A.baryCenter, B) || IsPointInsidePolygon(B.baryCenter, A)) {
		return true;
	}

	// check if this axis is not a separating axis
	if (aMax < bMin) {
		return false;
	}

	// compute reduced polygon
#if 1
	// non async method has shown quicker results (at 500 vertices)
	Polygon C = PolygonComputeReducePol(A, barAxis, bMin, true);
	Polygon D = PolygonComputeReducePol(B, barAxis, aMax, false);

# else
	
	std::future<Polygon> resultC = std::async(std::launch::async, PolygonComputeReducePol, A, barAxis, bMin, true);
	std::future<Polygon> resultD = std::async(std::launch::async, PolygonComputeReducePol, B, barAxis, aMax, false);

	C = resultC.get();
	D = resultD.get();
#endif


	
	

	

	// return test on new polygon
#if 0
	return PolygonInterTestSAT(C, D);
#else // Do not compute closing edge norm as barAxis act as it
	if (C.vertices.size() == A.vertices.size() || D.vertices.size() == B.vertices.size()) {
		return PolygonInterTestSAT(C, D);
	}
	else {
		return PolygonInterTestSATForRedPol(C, D);
	}
#endif
}

std::pair<float, float> GetMinMaxPolygonProjAxis(const Polygon& RESTRICT A, const Vector d)
{
	assert(A.vertices.size() >= 3 && "Pol min vertices is 3");

	float minProj = std::numeric_limits<float>::infinity();
	float maxProj = - std::numeric_limits<float>::infinity();

	{
		const int nVertA = A.vertices.size(); 
		int ii = 0; // Loop unrolling 
		for (; ii + 3 < nVertA; ii += 4) { 
			const float proj1 = A.vertices[ii].x * d.x + A.vertices[ii].y * d.y; 
			const float proj2 = A.vertices[ii+1].x * d.x + A.vertices[ii+1].y * d.y; 
			const float proj3 = A.vertices[ii+2].x * d.x + A.vertices[ii+2].y * d.y; 
			const float proj4 = A.vertices[ii+3].x * d.x + A.vertices[ii+3].y * d.y; 
			minProj = std::min({minProj, proj1, proj2, proj3, proj4}); 
			maxProj = std::max({maxProj, proj1, proj2, proj3, proj4}); } 
		// Process remaining vertices 
		for (; ii < nVertA; ++ii) {
			const float proj = A.vertices[ii].x * d.x + A.vertices[ii].y * d.y;
			minProj = std::min(minProj, proj);
			maxProj = std::max(maxProj, proj);
		}
	
	}

	return { minProj, maxProj };
}

float GetMaxPolygonProjAxis(const Polygon& RESTRICT A, const Vector d)
{
	assert(A.vertices.size() >= 3  && "Pol min vertices is 3");

	float maxProj = A.vertices[0].x * d.x + A.vertices[0].y * d.y; 
	const int nVertA = A.vertices.size(); 
	int dir = (A.vertices[0].x * d.x + A.vertices[0].y * d.y) < 
							(A.vertices[1].x * d.x + A.vertices[1].y * d.y) ? 1 : -1; 
	int nextInd = (dir == 1) ? 1 : nVertA - 1; 
	while (true) { 
		float nexProj = A.vertices[nextInd].x * d.x + A.vertices[nextInd].y * d.y; 
		// use < to avoid problem with collinear points
		if (nexProj < maxProj || (nextInd == nVertA - 1 && dir == 1) || (nextInd == 0 && dir == -1)) break; 
		maxProj = nexProj; nextInd += dir; 
	}

	return maxProj;
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

bool PolygonIncludeInEachOther(const Polygon& RESTRICT A, const Polygon& RESTRICT B)
{
	assert(A.vertices.size() >= 3 && B.vertices.size() > 3 && "Pol min vertices is 3");

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

bool IsPointInsidePolygon(Point A, const Polygon& RESTRICT pol)
{
	assert(pol.vertices.size() >= 3  && "Pol min vertices is 3");

	bool isInside = IsPointInsidePolygonRec(A, pol, 1, pol.vertices.size()-1);
	return isInside;
	// Check if point A on the same side of the edges
	{
		float sign = CrossProd2D(A - pol.vertices[0], pol.vertices[1] - pol.vertices[0]);
		int NpolVert = pol.vertices.size();
		for (int ii = 1; ii < NpolVert; ++ii) {
			Point V1 = pol.vertices[ii];
			Point V2 = pol.vertices[(ii + 1) % NpolVert];

			float cross = CrossProd2D(A - V1, V2 - V1);
			if (sign * cross < 0) {
				assert(isInside == false && "Should be false");
				return false;
			}
		}
	}

	assert(isInside == true && "Should be true");
	return true;
}

bool IsPointInsidePolygonRec(Point A, const Polygon& RESTRICT pol, const int left, const int right)
{
	assert(pol.vertices.size() >= 3 && "Pol min vertices is 3");

	if (right - left <= 1) {
		return IsPointInsideTriangle(A, pol.vertices[0], pol.vertices[left], pol.vertices[right]);
	}

	const int mid = (left + right) / 2;

	const Point v0 = pol.vertices[0]; 
	const Point vMid = pol.vertices[mid]; 


	if (CrossProd2D(A - v0, vMid - v0) > 0) {
		return IsPointInsidePolygonRec(A, pol, left, mid);
	}
	else { 
		return IsPointInsidePolygonRec(A, pol, mid, right);
	}
}


bool IsPointInsideTriangle(const Point pt, const Point v0, const Point v1, const Point v2) {
	Point d0 = v1 - v0;
	Point d1 = v2 - v0;
	Point d2 = pt - v0;
	
	float areaV1V0P = CrossProd2D(d2, d0);
	float areaV0V2P = CrossProd2D(d1, d2);

	if (areaV1V0P * areaV0V2P < 0) return false;

	Point d3 = v1 - pt;
	Point d4 = v2 - pt;
	float areaV1V2P = CrossProd2D(d4, d3);

	return areaV1V0P * areaV1V2P < 0 ? false : true;

}

Polygon PolygonComputeReducePol(const Polygon& RESTRICT A, const Vector axis, const float limit, const bool isAbvLmtPol)
{
	assert(A.vertices.size() >= 3 && "Pol min vertices is 3");

	if (A.vertices.size() == 3) return A;

	Polygon newPol;
	newPol.vertices.reserve(A.vertices.size());
	{
		// Define the lambda 
		using LimitCompFunc = std::function<bool(float)>; 
		// Use the defined type for the ternary operator 
		LimitCompFunc limitComp = isAbvLmtPol ?
			LimitCompFunc([&](float proj) { return proj >= limit; }) :
			LimitCompFunc([&](float proj) { return proj <= limit;});

		const int nVertA = A.vertices.size();

		const float projPrev = A.vertices[nVertA - 1].x * axis.x + A.vertices[nVertA - 1].y * axis.y;
		bool isPrevProjValid = limitComp(projPrev);
		const float proj = A.vertices[0].x * axis.x + A.vertices[0].y * axis.y;
		bool isProjValid = limitComp(proj);
		for (int ii = 0; ii < nVertA; ++ii) {

			{
				const int nextInd = (ii + 1) % nVertA;
				const float projNext = A.vertices[nextInd].x * axis.x + A.vertices[nextInd].y * axis.y;
				const bool isNextProjValid = limitComp(projNext);
				if (isPrevProjValid || isProjValid || isNextProjValid) {
					newPol.vertices.push_back(A.vertices[ii]);
				}
				isPrevProjValid = isProjValid;
				isProjValid = isNextProjValid;
			}
		}
	}
	return newPol;
}

void Polygon::CalculateBarycenter()
{
	baryCenter = { 0.f, 0.f };
	for (const auto& vert : vertices) {
		baryCenter += Vertex{ vert.x, vert.y };
	}
	
	baryCenter = (1.f / vertices.size()) * baryCenter;

}

#include <cassert> // assert
#include <limits> // numeric_limits
#include <functional>
#include <future>

#include "Geom.h"
#include "Utilities.h"


#include "Random.h"

namespace Geom {

	bool CompareByAngle(const Vertex a, const Vertex b)
	{
		return atan2(a.y, a.x) < atan2(b.y, b.x);
	}


	Polygon MakeConvexPol(int iVertices) {
		assert(iVertices >= 3 && "A minimum of 3 Vertices is   ");

		const int nVert = std::max(iVertices, 3);

		Polygon pol;
		pol.Vertices.reserve(nVert);

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
				for (int ii = 1; ii < nVert - 1; ++ii) {
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
				for (int ii = 1; ii < nVert - 1; ++ii) {
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
				vecs.emplace_back(Vector{ xVec[ii],yVec[ii] });
			}
		}

		// Sort the vectors by angle
		std::sort(vecs.begin(), vecs.end(), CompareByAngle);

		// Lay them end-to-end to form a polygon
		{
			static std::uniform_real_distribution<float> disCenter(-2.f, 2.f);
			float polX = RandomCustom::GetRdnFloatUniform(disCenter), polY = RandomCustom::GetRdnFloatUniform(disCenter);
			pol.Barycenter = { 0.f, 0.f };
			for (const auto& vec : vecs) {
				polX += vec.x;
				polY += vec.y;
				pol.Vertices.emplace_back(Vertex{ polX, polY });
				pol.Barycenter += Vertex{ polX, polY };
			}
			// store Barycenter
			pol.Barycenter = (1.f / vecs.size()) * pol.Barycenter;
		}


		return pol;
	}


	bool DoPolygonsIntersects(const Polygon& RESTRICT A, const Polygon& RESTRICT B)
	{
		assert(A.Vertices.size() >= 3 && B.Vertices.size() >= 3 && "Pol min Vertices is 3");

		bool bInterBForce = measureExecutionTime(PolygonsInterTestBForce, "Brute force", A, B);
		bool bInterSAT = measureExecutionTime(PolygonInterTestSAT, "SAT", A, B);
		bool bInterSATOpti = measureExecutionTime(PolygonInterTestSATOpti, "New SAT", A, B);
		bool bInterGJK = measureExecutionTime(PolygonInterTestGJK, "GJK", A, B);

		/*assert(bInterBForce == bInterSAT && "Brut force and SAT test should agree with each other");
		assert(bInterBForce == bInterSATOpti && "Brut force and SAT Opti test should agree with each other");*/


		return bInterBForce;
	}

	bool PolygonsInterTestBForce(const Polygon& RESTRICT A, const Polygon& RESTRICT B)
	{

		assert(A.Vertices.size() >= 3 && B.Vertices.size() >= 3 && "Pol min Vertices is 3");

		// Test if edges intersects
		{
			const int nAVert = A.Vertices.size();
			const int nBVert = B.Vertices.size();

			for (int i = 0; i < nAVert; ++i) {
				const int nextA = (i + 1) % nAVert;
				for (int j = 0; j < nBVert; ++j)
				{
					const int nextB = (j + 1) % nBVert;
					if (SegmentIntersect(A.Vertices[i], A.Vertices[nextA],
						B.Vertices[j], B.Vertices[nextB]))
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

		// Test if one vertex is inside the other polygon
		{
			if (IsPointInsidePolygon(A.Vertices[0], B)
				|| IsPointInsidePolygon(B.Vertices[0], A)) {
				return true;
			}
		}


		// no intersection found
		return false;
	}

	bool PolygonInterTestSAT(const Polygon& RESTRICT A, const Polygon& RESTRICT B)
	{
		assert(A.Vertices.size() >= 3 && B.Vertices.size() >= 3 && "Pol min Vertices is 3");

		std::vector<Vector> axisToTestAgainst;
		axisToTestAgainst.reserve(A.Vertices.size() + B.Vertices.size());

		auto computePolEdgesNorm = [&axisToTestAgainst](const Polygon& RESTRICT pol) {
			const int nVert = pol.Vertices.size();
			for (int ii = 0; ii < nVert; ++ii) {
				const int nextInd = (ii + 1) % nVert;
				const Vector edgeVec = pol.Vertices[nextInd] - pol.Vertices[ii];
				axisToTestAgainst.emplace_back(Vector{ -edgeVec.y, edgeVec.x });
			}
			};

		// A's axes
		computePolEdgesNorm(A);

		// B's axes
		computePolEdgesNorm(B);

		// Test axes
		{
			for (const Vector axis : axisToTestAgainst) {
				std::pair<float, float> AProj = GetMinMaxPolygonProjAxis(A, axis);// { -GetMaxPolygonProjAxis(A, -axis), GetMaxPolygonProjAxis(A, axis) };
				std::pair<float, float> BProj = GetMinMaxPolygonProjAxis(B, axis); // { -GetMaxPolygonProjAxis(B, -axis), GetMaxPolygonProjAxis(B, axis) };
				if (AProj.first > BProj.second || AProj.second < BProj.first) {
					return false;
				}

			}
		}

		return true;

	}


	bool PolygonInterTestSATOpti(const Polygon& RESTRICT A, const Polygon& RESTRICT B)
	{
		assert(A.Vertices.size() >= 3 && B.Vertices.size() >= 3 && "Pol min Vertices is 3");

		// check if barycenter inside each other
		if (IsPointInsidePolygon(A.Barycenter, B) || IsPointInsidePolygon(B.Barycenter, A)) {
			return true;
		}

		Vector barAxis = B.Barycenter - A.Barycenter;
		//barAxis = barAxis / barAxis.Mag();
		auto aMax = GetMaxPolygonProjAxis(A, barAxis);
		auto bMin = -GetMaxPolygonProjAxis(B, -barAxis);

		// check if this axis is not a separating axis
		if (aMax < bMin) {
			return false;
		}

		// compute reduced polygon
		Polygon C = PolygonComputeReducePol(A, barAxis, bMin, true);
		Polygon D = PolygonComputeReducePol(B, barAxis, aMax, false);

		return PolygonInterTestSAT(C, D);

	}

	bool PolygonInterTestSATOptiItera(const Polygon& RESTRICT A, const Polygon& RESTRICT B)
	{
		assert(A.Vertices.size() >= 3 && B.Vertices.size() >= 3 && "Pol min Vertices is 3");

		// check if barycenter inside each other
		if (IsPointInsidePolygon(A.Barycenter, B) || IsPointInsidePolygon(B.Barycenter, A)) {
			return true;
		}

		Vector barAxis = B.Barycenter - A.Barycenter;
		//barAxis = barAxis / barAxis.Mag();
		auto aMax = GetMaxPolygonProjAxis(A, barAxis);
		auto bMin = -GetMaxPolygonProjAxis(B, -barAxis);

		// check if this axis is not a separating axis
		if (aMax < bMin) {
			return false;
		}

		// compute reduced polygon
		Polygon C = PolygonComputeReducePol(A, barAxis, bMin, true);
		Polygon D = PolygonComputeReducePol(B, barAxis, aMax, false);

		if (C.Vertices.size() >= A.Vertices.size() || D.Vertices.size() >= B.Vertices.size()) {
			return PolygonInterTestSAT(C, D);
		}
		else {
			return PolygonInterTestSATOptiItera(C, D);
		}


	}

	bool PolygonInterTestGJK(const Polygon& RESTRICT A, const Polygon& RESTRICT B)
	{
		assert(A.Vertices.size() >= 3 && B.Vertices.size() >= 3 && "Pol min Vertices is 3");


		auto SimplexSupportPoint = [&](const Vector d) {
			return GetFurthestPoint(A, d) - GetFurthestPoint(B, -d);
			};



		Simplex GJKSimplex;
		Vector dir = { 0, 1 }; // initial direction chosen arbitrarly
		Point simpSup = SimplexSupportPoint(dir);
		GJKSimplex.Add(SimplexSupportPoint(simpSup));
		dir = -simpSup;


		while (true) {
			simpSup = SimplexSupportPoint(dir);
			if (DotProduct(simpSup, dir) < 0) {
				return false;
			}

			GJKSimplex.Add(simpSup);

			if (GJKSimplex.UpdateSimplex(dir)) {
				return true;
			}
		}



		return false;
	}

	std::pair<float, float> GetMinMaxPolygonProjAxis(const Polygon& RESTRICT A, const Vector d)
	{
		assert(A.Vertices.size() >= 3 && "Pol min Vertices is 3");

		float minProj = A.Vertices[0].x * d.x + A.Vertices[0].y * d.y;
		float maxProj = minProj;

		{
			const int nVertA = A.Vertices.size();
			for (int ii = 0; ii < nVertA; ++ii) {
				const float proj = A.Vertices[ii].x * d.x + A.Vertices[ii].y * d.y;
				minProj = std::min(minProj, proj);
				maxProj = std::max(maxProj, proj);
			}

		}

		return { minProj, maxProj };
	}

	float GetMaxPolygonProjAxis(const Polygon& RESTRICT A, const Vector d)
	{
		assert(A.Vertices.size() >= 3 && "Pol min Vertices is 3");
		auto ProjIndAlongD = [&](int index) {
			return A.Vertices[index].x * d.x + A.Vertices[index].y * d.y;
			};


		const int nVertA = A.Vertices.size();

		float maxProj = 0;
		int MaxProjInd = 0;
		int dir = 0;
		{
			float proj0 = ProjIndAlongD(0); float proj1 = ProjIndAlongD(1);
			if (proj0 > proj1) {
				maxProj = proj0;
				dir = -1;
			}
			else {
				dir = 1;
				maxProj = proj1;
				MaxProjInd = 1;
			}
		}

		int nextProjInd = MaxProjInd + dir;
		if (nextProjInd < 0) nextProjInd += nVertA;
		float newProj = ProjIndAlongD(nextProjInd);
		while (maxProj <= newProj) {
			maxProj = newProj;
			MaxProjInd = nextProjInd;
			nextProjInd += dir;
			newProj = ProjIndAlongD(nextProjInd % nVertA);
		}

		return maxProj;
	}

	Point GetFurthestPoint(const Polygon& RESTRICT A, const Vector d)
	{
		auto ProjIndAlongD = [&](int index) {
			return A.Vertices[index].x * d.x + A.Vertices[index].y * d.y;
			};


		const int nVertA = A.Vertices.size();

		float maxProj = 0;
		int MaxProjInd = 0;
		int dir = 0;
		{
			float proj0 = ProjIndAlongD(0); float proj1 = ProjIndAlongD(1);
			if (proj0 > proj1) {
				maxProj = proj0;
				dir = -1;
			}
			else {
				dir = 1;
				maxProj = proj1;
				MaxProjInd = 1;
			}
		}

		int nextProjInd = MaxProjInd + dir;
		if (nextProjInd < 0) nextProjInd += nVertA;
		float newProj = ProjIndAlongD(nextProjInd);
		while (maxProj <= newProj) {
			maxProj = newProj;
			MaxProjInd = nextProjInd;
			nextProjInd += dir;
			newProj = ProjIndAlongD(nextProjInd % nVertA);
		}

		return A.Vertices[MaxProjInd];
	}

	

	bool SegmentIntersect(Point A, Point B, Point C, Point D)
	{
		// check the winding of triangles ABD - ABC
		// and CDB - CDA
		if (CrossProd2D(B - A, D - A) * CrossProd2D(B - A, C - A) <= 0
			&& CrossProd2D(D - C, A - C) * CrossProd2D(D - C, B - C) <= 0) {
			return true;
		}
		else {
			return false;
		}
	}

	bool PolygonIncludeInEachOther(const Polygon& RESTRICT A, const Polygon& RESTRICT B)
	{
		assert(A.Vertices.size() >= 3 && B.Vertices.size() > 3 && "Pol min Vertices is 3");

		// A inside B
		{
			const int nAVert = A.Vertices.size();
			for (int ii = 0; ii < nAVert; ++ii) {
				if (IsPointInsidePolygon(A.Vertices[ii], B)) {
					return true;
				}
			}
		}

		// B inside A
		{
			const int nBVert = B.Vertices.size();
			for (int ii = 0; ii < nBVert; ++ii) {
				if (IsPointInsidePolygon(B.Vertices[ii], A)) {
					return true;
				}
			}
		}

		return false;
	}

	bool IsPointInsidePolygon(Point A, const Polygon& RESTRICT Pol)
	{
		assert(Pol.Vertices.size() >= 3 && "Pol min Vertices is 3");

		bool isInside = IsPointInsidePolygonRec(A, Pol, 1, Pol.Vertices.size() - 1);
		return isInside;
		//// Check if point A on the same side of the edges
		//{
		//	float sign = CrossProd2D(A - Pol.Vertices[0], Pol.Vertices[1] - Pol.Vertices[0]);
		//	int NpolVert = Pol.Vertices.size();
		//	for (int ii = 1; ii < NpolVert; ++ii) {
		//		Point V1 = Pol.Vertices[ii];
		//		Point V2 = Pol.Vertices[(ii + 1) % NpolVert];

		//		float cross = CrossProd2D(A - V1, V2 - V1);
		//		if (sign * cross < 0) {
		//			assert(isInside == false && "Should be false");
		//			return false;
		//		}
		//	}
		//}

		//assert(isInside == true && "Should be true");
		//return true;
	}

	bool IsPointInsidePolygonRec(Point A, const Polygon& RESTRICT Pol, const int LeftIndex, const int RightIndex)
	{
		assert(Pol.Vertices.size() >= 3 && "Pol min Vertices is 3");

		if (RightIndex - LeftIndex <= 1) {
			return IsPointInsideTriangle(A, Pol.Vertices[0], Pol.Vertices[LeftIndex], Pol.Vertices[RightIndex]);
		}

		const int mid = (LeftIndex + RightIndex) / 2;

		const Point v0 = Pol.Vertices[0];
		const Point vMid = Pol.Vertices[mid];


		if (CrossProd2D(A - v0, vMid - v0) > 0) {
			return IsPointInsidePolygonRec(A, Pol, LeftIndex, mid);
		}
		else {
			return IsPointInsidePolygonRec(A, Pol, mid, RightIndex);
		}
	}


	bool IsPointInsideTriangle(const Point pt, const Vertex v0, const Vertex v1, const Vertex v2) {
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

	Polygon PolygonComputeReducePol(const Polygon& RESTRICT A, const Vector ProjAxis, const float limit, const bool isAbvLmt)
	{
		assert(A.Vertices.size() >= 3 && "Pol min Vertices is 3");

		if (A.Vertices.size() == 3) return A;

		Polygon newPol;
		newPol.Vertices.reserve(A.Vertices.size());
		{

			using LimitCompFunc = std::function<bool(float)>;
			LimitCompFunc limitComp = isAbvLmt ?
				LimitCompFunc([&](float proj) { return (proj - limit) >= 0; }) :
				LimitCompFunc([&](float proj) { return (proj - limit) <= 0; });

			auto getProj = [&](int index) {
				return A.Vertices[index].x * ProjAxis.x + A.Vertices[index].y * ProjAxis.y;
				};


			const int nVertA = A.Vertices.size();
			Point Barycenter{ 0,0 };
			bool isPrevProjValid = limitComp(getProj(nVertA - 1));
			bool isProjValid = limitComp(getProj(0));
			for (int ii = 0; ii < nVertA; ++ii) {

				{
					const bool isNextProjValid = limitComp(getProj((ii + 1) % nVertA));
					if (isPrevProjValid || isProjValid || isNextProjValid) {
						newPol.Vertices.push_back(A.Vertices[ii]);
						Barycenter += A.Vertices[ii];
					}
					isPrevProjValid = isProjValid;
					isProjValid = isNextProjValid;
				}
			}

			newPol.Barycenter = (1.f / newPol.Vertices.size()) * Barycenter;
		}

		assert(newPol.Vertices.size() >= 3 && "A polygon should have at least 3 Vertices");
		return newPol;
	}

	void Polygon::CalculateBarycenter()
	{
		Barycenter = { 0.f, 0.f };
		for (const auto& vert : Vertices) {
			Barycenter += Vertex{ vert.x, vert.y };
		}

		Barycenter = (1.f / Vertices.size()) * Barycenter;

	}

	void Simplex::Add(const Vertex& InVertex)
	{
		Vertices = { InVertex, Vertices[0], Vertices[1] };
		iSize = std::min(iSize + 1, 3);
	}

	bool Simplex::UpdateSimplex(Vector& OutDir)
	{
		switch (iSize) {
		case 0:
		case 1:
			assert(false && "Should never call this function with empty simplex");
			break;
		case 2:
			return LineUpdate(OutDir);
		case 3:
			return TriangleUpdate(OutDir);
		}

		return false;
	}

	bool Simplex::LineUpdate(Vector& OutDir)
	{
		Vector AB = Vertices[1] - Vertices[0];
		Vector AO = -Vertices[0];

		if (DotProduct(AB, AO) > 0) {
			OutDir = { -AB.y, AB.x };

			// two possibilities for the direction, 
			// confirm the right one is chosen
			if (DotProduct(AO, OutDir) < 0) {
				OutDir = -OutDir;
			}
		}
		else {
			Vertices = { Vertices[0] };
			OutDir = AO;
			iSize = 1;
		}

		return false;
	}

	bool Simplex::TriangleUpdate(Vector& OutDir)
	{

		Vector A = Vertices[0];
		Vector B = Vertices[1];
		Vector C = Vertices[2];

		double signedArea = 0.5 * (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));

		Vector AB = B - A;
		Vector AC = C - A;
		Vector AO = -A;

		Vector normalAB = (signedArea > 0) ? Vector{ AB.y, -AB.x } : Vector{ -AB.y, AB.x };
		if (DotProduct(normalAB, AO) > 0) {
			// Origin is outside AB, reduce simplex to AB
			Vertices = { A, B };
			OutDir = normalAB; // Update direction to point toward origin
			assert(!IsPointInsideTriangle({ 0,0 }, A, B, C) && "The point should be out of the triangle");
			return false;
		}

		// Check if the origin is outside AC
		Vector normalAC = (signedArea > 0) ? Vector{ -AC.y, AC.x } : Vector{ AC.y, -AC.x };
		if (DotProduct(normalAC, AO) > 0) {
			// Origin is outside AC, reduce simplex to AC
			Vertices = { A, C };
			OutDir = normalAC; // Update direction to point toward origin
			assert(!IsPointInsideTriangle({ 0,0 }, A, B, C) && "The point should be out of the triangle");
			return false;
		}



		return true;
	}

}

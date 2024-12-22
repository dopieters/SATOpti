#pragma once
#include <vector> // std::vector

constexpr double pi = 3.14159265358979323846;

struct Point{
	float x;
	float y;
};

typedef Point Vertex; 
typedef Point Vector;

Vertex MakeRandomVertexPt();

//Function to sort points by angle 
bool CompareByAngle(const Vertex& a, const Vertex& b);

struct Polygon
{
	std::vector<Vertex> vertices;
};

// algo based on Pavel-Valtr
// https://cglab.ca/~sander/misc/ConvexGeneration/convex.html
Polygon MakeConvexPol(int nVertices);

bool DoPolygonsIntersects(const Polygon& A, const Polygon& B);

bool SegmentIntersect(Point A, Point B, Point C, Point D);

// Check for every vertex if it is includes in the other polygon
bool PolygonIncludeInEachOther(const Polygon& A, const Polygon& B);

bool IsPointInsidePolygon(Point A, const Polygon& pol);




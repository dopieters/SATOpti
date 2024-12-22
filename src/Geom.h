#pragma once
#include <vector> // std::vector

constexpr double pi = 3.14159265358979323846;

struct Point{
	float x;
	float y;

	Point operator+(const Point other) const { 
		return { x + other.x, y + other.y }; 
	}

	Point operator-(const Point other) const {
		return { x - other.x, y - other.y };
	}

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

float CrossProd2D(Vector Va, Vector Vb);

// In 2D equivalent to check winding of points
bool SegmentIntersect(Point A, Point B, Point C, Point D);

// Check for every vertex if it is includes in the other polygon
bool PolygonIncludeInEachOther(const Polygon& A, const Polygon& B);

bool IsPointInsidePolygon(Point A, const Polygon& pol);




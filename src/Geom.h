#pragma once
#include <vector> // std::vector
#include <cmath> // std::sqrt

constexpr double pi = 3.14159265358979323846;

struct Point{
	float x;
	float y;

	Point operator-() const {
		return { -x , -y };
	}

	Point operator+(const Point other) const { 
		return { x + other.x, y + other.y }; 
	}

	Point& operator+=(const Point& other) {
		x += other.x; 
		y += other.y; 
		return *this; 
	}

	Point operator-(const Point other) const {
		return { x - other.x, y - other.y };
	}

	Point operator*(const float scalar) const {
		return { x * scalar, y * scalar };
	}

	friend Point operator* (const float scalar, const Point pt){
		return { pt.x * scalar, pt.y * scalar };
	}

	Point operator/(const float scalar) const {
		return { x / scalar, y / scalar };
	}

	friend Point operator/ (const float scalar, const Point pt){
		return { pt.x / scalar, pt.y / scalar };
	}


	float Mag() const{
		return std::sqrt(x * x + y * y);
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
	Point baryCenter;
};

// algo based on Pavel-Valtr
// https://cglab.ca/~sander/misc/ConvexGeneration/convex.html
Polygon MakeConvexPol(int nVertices);

bool DoPolygonsIntersects(const Polygon& A, const Polygon& B);
bool PolygonsInterTestBForce(const Polygon& A, const Polygon& B);
bool PolygonInterTestSAT(const Polygon& A, const Polygon& B);
bool PolygonInterTestSATForRedPol(const Polygon& A, const Polygon& B);
bool PolygonInterTestSATOpti(const Polygon& A, const Polygon& B);

// first minProj, second maxProj
std::pair<float, float> GetMinMaxPolygonProjAxis(const Polygon& A, const Vector d);
float GetMaxPolygonProjAxis(const Polygon& A, const Vector d);

float CrossProd2D(Vector Va, Vector Vb);

// In 2D equivalent to check winding of points
bool SegmentIntersect(Point A, Point B, Point C, Point D);

// Check for every vertex if it is includes in the other polygon
bool PolygonIncludeInEachOther(const Polygon& A, const Polygon& B);

bool IsPointInsidePolygon(Point A, const Polygon& pol);
bool IsPointInsidePolygonRec(Point A, const Polygon& pol, const int left, const int right );
bool IsPointInsideTriangle(const Point A, const Vertex v0, const Vertex v1, const Vertex v2);

Polygon PolygonComputeReducePol(const Polygon& A, const Vector axis, const float limit, const bool isAbvLimit);
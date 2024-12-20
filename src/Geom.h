#pragma once
#include <vector> // std::vector
#include <random>
#include <cassert> // assert

constexpr double pi = 3.14159265358979323846;

struct Vertex{
	float x;
	float y;
};

Vertex MakeRandomVertexPt(){
	std::random_device rd; 
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);

	Vertex pt = { dis(gen), dis(gen)};
	return pt;
}

//Function to sort points by angle 
bool compareByAngle(const Vertex& a, const Vertex& b) { 
	return atan2(a.y, a.x) < atan2(b.y, b.x); 
}

struct Polygon
{
	std::vector<Vertex> vertices;
};

Polygon MakeConvexPol(int nVertices) {
	assert(nVertices >= 3 && "A minimum of 3 vertices is needed");

	int nVert = std::max(nVertices, 3);

	Vertex center = MakeRandomVertexPt();

	Polygon pol;
	pol.vertices.reserve(nVert);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> disTheta(0.0,2* pi);

	for (int ii = 0; ii < nVert; ++ii) {
		float theta = disTheta(gen);
		float radius = 1;
		float x = center.x + radius*cos(theta);
		float y = center.y + radius*sin(theta);
		pol.vertices.emplace_back(Vertex{ x,y });

	}

	std::sort(pol.vertices.begin(), pol.vertices.end(), compareByAngle);

	return pol;
}



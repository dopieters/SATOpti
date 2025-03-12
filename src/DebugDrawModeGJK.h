#pragma once

#include "DrawWindow.h"


class DebugDrawModeGJK final : public DrawWindow {
public:
	DebugDrawModeGJK();
	~DebugDrawModeGJK();

	void Run();


private:
	bool m_isRunning = false;

	void ProcessEvents();
	void MakePolygons();
	void UpdateSimplex();

	Geom::Polygon pol1;
	Geom::Polygon pol2;

	Geom::Simplex simp;

	Geom::Vector dir = { 0, 1 };

};
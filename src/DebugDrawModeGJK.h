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

	Polygon pol1;
	Polygon pol2;

	Simplex simp;

	Vector dir = { 0, 1 };

};
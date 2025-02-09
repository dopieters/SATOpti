#pragma once
#include <SDL2/SDL.h>
#include "Geom.h"
#include <vector>


class DrawWindow {
public:
	DrawWindow();
	DrawWindow(std::vector<std::pair<Polygon, Polygon>> pairPolygons);
	~DrawWindow();

	void ScanPairOfPolygons();
	bool IsValid()const { return m_isValid; }

private:
	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	// Valid when m_window and m_isRunning 
	// mem properly allocated
	bool m_isValid = false;

	// pair to draw
	std::vector<std::pair<Polygon, Polygon>> m_EventToDraw;
	int currentPolPair = 0;

private:
	void ProcessEvents();
	void DrawPolygonPairs();
	void DrawPolygon(const Polygon& A) const;

};
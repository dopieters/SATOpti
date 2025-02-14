#pragma once
#include <SDL2/SDL.h>
#include "Geom.h"


class DrawWindow {
public:
	DrawWindow();
	~DrawWindow();

	bool IsValid()const { return m_isValid; }

protected:
	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	// Valid when m_window and m_isRunning 
	// mem properly allocated
	bool m_isValid = false;

	
protected:
	void DrawPolygon(const Polygon& A) const;
	void DrawHyperPlanes(const Vector v, const float min, const float max) const;

};
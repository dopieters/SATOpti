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
	
	
	Point m_CameraPos = { 0.f, 0.f };
	float m_Zoom = 1.f;

protected:
	void DrawPolygon(const Polygon& A) const;
	void DrawSimplex(const Simplex& s) const;
	void DrawHyperPlanes(const Vector v, const float min, const float max) const;

	void DrawOriginAxis();

	[[nodiscard]]Vector ToWindowsCoordinate(Vector v) const;

	void CameraMovementEvents(const SDL_Event& event);


};
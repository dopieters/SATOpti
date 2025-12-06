#pragma once
#include <SDL2/SDL.h>
#include "Geom.h"



// Debug class that implements de base behavior of the different debug modes
class DrawWindow {
public:
	DrawWindow();
	~DrawWindow();

	bool IsValid()const { return bValid; }

protected:
	SDL_Window* pWnd = nullptr;
	SDL_Renderer* pRenderer = nullptr;

	// Valid when pWnd and bRunning 
	// mem properly allocated
	bool bValid = false;
	
	// Point showing the Camera focus
	Geom::Point CameraPosition = { 0.f, 0.f };
	float fZoom = 1.f;

protected:
	void DrawPolygon(const Geom::Polygon& InPolygon) const;
	void DrawSimplex(const Geom::Simplex& InSimplex) const;
	void DrawHyperPlanes(const Geom::Vector InNormal, const float min, const float max) const;

	void DrawOriginAxis();

	[[nodiscard]] Geom::Vector ToWindowsCoordinate(Geom::Point InVertex) const;

	void CameraMovementEvents(const SDL_Event& event);


};
#include "DrawWindow.h"
#include <cassert>
#include <iostream>

#include "Params.h"
#include "Utilities.h"


DrawWindow::DrawWindow():
pWnd(nullptr), pRenderer(nullptr), bValid(false)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return;
	}

	// Create a window
	pWnd = SDL_CreateWindow("Hello SDL",
		100, 100, W_WIDTH, W_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (pWnd == nullptr)
	{
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return;
	}

	// Create a renderer
	pRenderer = SDL_CreateRenderer(pWnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (pRenderer == nullptr)
	{
		SDL_DestroyWindow(pWnd);
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return;
	}

	SDL_RenderClear(pRenderer);
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);


	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	SDL_RenderSetLogicalSize(pRenderer, W_WIDTH, W_HEIGHT);



	// properly managed to allocate needed memory
	bValid = true;
}


DrawWindow::~DrawWindow()
{
	if (!bValid) return;


	// Cleanup
	SDL_DestroyRenderer(pRenderer);
	SDL_DestroyWindow(pWnd);
	SDL_Quit();

	pRenderer = nullptr;
	pWnd = nullptr;
}



void DrawWindow::DrawPolygon(const Geom::Polygon& InPolygon) const {
	const int nVert = InPolygon.Vertices.size();

	if (nVert < 3) return;

	for (int ii = 0; ii < nVert - 1; ++ii) {
		SDL_RenderDrawLine(pRenderer, InPolygon.Vertices[ii].x * fZoom * DRAW_SCALE + SHIFTX + CameraPosition.x,
			InPolygon.Vertices[ii].y * fZoom * DRAW_SCALE + SHIFTY + CameraPosition.y,
			InPolygon.Vertices[ii + 1].x * fZoom * DRAW_SCALE + SHIFTX + CameraPosition.x,
			InPolygon.Vertices[ii + 1].y * fZoom * DRAW_SCALE + SHIFTY + CameraPosition.y);
	}
	SDL_RenderDrawLine(pRenderer, InPolygon.Vertices[nVert - 1].x * fZoom * DRAW_SCALE + SHIFTX + CameraPosition.x,
		InPolygon.Vertices[nVert - 1].y * fZoom *DRAW_SCALE + SHIFTY + CameraPosition.y,
		InPolygon.Vertices[0].x * fZoom * DRAW_SCALE + SHIFTX + CameraPosition.x,
		InPolygon.Vertices[0].y * fZoom * DRAW_SCALE + SHIFTY + CameraPosition.y);
}

void DrawWindow::DrawSimplex(const Geom::Simplex& InSimplex) const
{
	switch (InSimplex.iSize)
	{
	case 0:
		return;
	case 1:
		const Geom::Point p = ToWindowsCoordinate(InSimplex.Vertices[0]);
		SDL_RenderDrawPoint(pRenderer, p.x, p.y);
		break;
	case 2:
	{
		const Geom::Point p1 = ToWindowsCoordinate(InSimplex.Vertices[0]);
		const Geom::Point p2 = ToWindowsCoordinate(InSimplex.Vertices[1]);
		SDL_RenderDrawLine(pRenderer, p1.x, p1.y, p2.x, p2.y);
	}
		break;
	case 3:
	{
		const Geom::Point p1 = ToWindowsCoordinate(InSimplex.Vertices[0]);
		const Geom::Point p2 = ToWindowsCoordinate(InSimplex.Vertices[1]);
		const Geom::Point p3 = ToWindowsCoordinate(InSimplex.Vertices[2]);
		SDL_RenderDrawLine(pRenderer, p1.x, p1.y, p2.x, p2.y);
		SDL_RenderDrawLine(pRenderer, p1.x, p1.y, p3.x, p3.y);
		SDL_RenderDrawLine(pRenderer, p3.x, p3.y, p2.x, p2.y);
	}
		break;
	default:
		assert(false && "Simplex should always have a size inferior to 3");
		break;
	} 

}

void DrawWindow::DrawHyperPlanes(const Geom::Vector InNormal, const float min, const float max) const
{
	Geom::Vector barAxisPerp = { -InNormal.y, InNormal.x };

	Geom::Point Pmax = max * InNormal;
	Geom::Point P1 = Pmax + 100 * barAxisPerp;
	Geom::Point P2 = Pmax - 100 * barAxisPerp;

	// First plane
	SDL_RenderDrawLine(pRenderer, P1.x * DRAW_SCALE + SHIFTX,
		P1.y * DRAW_SCALE + SHIFTY,
		P2.x * DRAW_SCALE + SHIFTX,
		P2.y * DRAW_SCALE + SHIFTY);

	Geom::Point Pmin = min * InNormal;
	P1 = Pmin + 100 * barAxisPerp;
	P2 = Pmin - 100 * barAxisPerp;

	// Second plane
	SDL_RenderDrawLine(pRenderer, P1.x * DRAW_SCALE + SHIFTX,
		P1.y * DRAW_SCALE + SHIFTY,
		P2.x * DRAW_SCALE + SHIFTX,
		P2.y * DRAW_SCALE + SHIFTY);
}


void DrawWindow::DrawOriginAxis()
{
	Geom::Point y0 = ToWindowsCoordinate({ 0, -1000 }); Geom::Point y1 = ToWindowsCoordinate({ 0, 1000 });
	Geom::Point x0 = ToWindowsCoordinate({ -1000, 0 }); Geom::Point x1 = ToWindowsCoordinate({ 1000, 0 });

	SDL_RenderDrawLine(pRenderer, x0.x, x0.y, x1.x, x1.y);
	SDL_RenderDrawLine(pRenderer, y0.x, y0.y, y1.x, y1.y);

}

Geom::Vector DrawWindow::ToWindowsCoordinate(Geom::Point InPoint) const
{
	return { InPoint.x * fZoom * DRAW_SCALE + SHIFTX + CameraPosition.x, 
				InPoint.y * fZoom * DRAW_SCALE + SHIFTY + CameraPosition.y };
}

void DrawWindow::CameraMovementEvents(const SDL_Event& event)
{
	switch (event.key.keysym.sym)
	{
	case SDLK_SPACE: // reset cammera
		CameraPosition.x = 0.f; CameraPosition.y = 0.f;
		fZoom = 1.f;
		break;
	case SDLK_a:
		CameraPosition.x -= 10;
		break;
	case SDLK_d:
		CameraPosition.x += 10;
		break;
	case SDLK_w:
		CameraPosition.y -= 10;
		break;
	case SDLK_s:
		CameraPosition.y += 10;
		break;
	case SDLK_z:
		fZoom += 0.1;
		break;
	case SDLK_x:
		fZoom -= 0.1;
		break;
		
	}

}



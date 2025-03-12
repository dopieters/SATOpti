#include "DrawWindow.h"
#include <cassert>
#include <iostream>

#include "Params.h"
#include "Utilities.h"


DrawWindow::DrawWindow():
m_window(nullptr), m_renderer(nullptr), m_isValid(false)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return;
	}

	// Create a window
	m_window = SDL_CreateWindow("Hello SDL",
		100, 100, W_WIDTH, W_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (m_window == nullptr)
	{
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return;
	}

	// Create a renderer
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (m_renderer == nullptr)
	{
		SDL_DestroyWindow(m_window);
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return;
	}

	SDL_RenderClear(m_renderer);
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

	// properly managed to allocate needed memory
	m_isValid = true;
}


DrawWindow::~DrawWindow()
{
	if (!m_isValid) return;


	// Cleanup
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();

	m_renderer = nullptr;
	m_window = nullptr;
}



void DrawWindow::DrawPolygon(const Geom::Polygon& A) const {
	const int nVert = A.vertices.size();

	if (nVert < 3) return;

	for (int ii = 0; ii < nVert - 1; ++ii) {
		SDL_RenderDrawLine(m_renderer, A.vertices[ii].x * m_Zoom * DRAW_SCALE + SHIFTX + m_CameraPos.x,
			A.vertices[ii].y * m_Zoom * DRAW_SCALE + SHIFTY + m_CameraPos.y,
			A.vertices[ii + 1].x * m_Zoom * DRAW_SCALE + SHIFTX + m_CameraPos.x,
			A.vertices[ii + 1].y * m_Zoom * DRAW_SCALE + SHIFTY + m_CameraPos.y);
	}
	SDL_RenderDrawLine(m_renderer, A.vertices[nVert - 1].x * m_Zoom * DRAW_SCALE + SHIFTX + m_CameraPos.x,
		A.vertices[nVert - 1].y * m_Zoom *DRAW_SCALE + SHIFTY + m_CameraPos.y,
		A.vertices[0].x * m_Zoom * DRAW_SCALE + SHIFTX + m_CameraPos.x,
		A.vertices[0].y * m_Zoom * DRAW_SCALE + SHIFTY + m_CameraPos.y);
}

void DrawWindow::DrawSimplex(const Geom::Simplex& s) const
{
	switch ( s.m_size)
	{
	case 0:
		return;
	case 1:
		const Geom::Point p = ToWindowsCoordinate(s.vertices[0]);
		SDL_RenderDrawPoint(m_renderer, p.x, p.y);
		break;
	case 2:
	{
		const Geom::Point p1 = ToWindowsCoordinate(s.vertices[0]);
		const Geom::Point p2 = ToWindowsCoordinate(s.vertices[1]);
		SDL_RenderDrawLine(m_renderer, p1.x, p1.y, p2.x, p2.y);
	}
		break;
	case 3:
	{
		const Geom::Point p1 = ToWindowsCoordinate(s.vertices[0]);
		const Geom::Point p2 = ToWindowsCoordinate(s.vertices[1]);
		const Geom::Point p3 = ToWindowsCoordinate(s.vertices[2]);
		SDL_RenderDrawLine(m_renderer, p1.x, p1.y, p2.x, p2.y);
		SDL_RenderDrawLine(m_renderer, p1.x, p1.y, p3.x, p3.y);
		SDL_RenderDrawLine(m_renderer, p3.x, p3.y, p2.x, p2.y);
	}
		break;
	default:
		assert(false && "Simplex should always have a size inferior to 3");
		break;
	} 

}

void DrawWindow::DrawHyperPlanes(const Geom::Vector v, const float min, const float max) const
{
	Geom::Vector barAxisPerp = { -v.y, v.x };

	Geom::Point Pmax = max * v;
	Geom::Point P1 = Pmax + 100 * barAxisPerp;
	Geom::Point P2 = Pmax - 100 * barAxisPerp;

	// first plane
	SDL_RenderDrawLine(m_renderer, P1.x * DRAW_SCALE + SHIFTX,
		P1.y * DRAW_SCALE + SHIFTY,
		P2.x * DRAW_SCALE + SHIFTX,
		P2.y * DRAW_SCALE + SHIFTY);

	Geom::Point Pmin = min * v;
	P1 = Pmin + 100 * barAxisPerp;
	P2 = Pmin - 100 * barAxisPerp;

	// first plane
	SDL_RenderDrawLine(m_renderer, P1.x * DRAW_SCALE + SHIFTX,
		P1.y * DRAW_SCALE + SHIFTY,
		P2.x * DRAW_SCALE + SHIFTX,
		P2.y * DRAW_SCALE + SHIFTY);
}

void DrawWindow::DrawOriginAxis()
{
	Geom::Point y0 = ToWindowsCoordinate({ 0, -1000 }); Geom::Point y1 = ToWindowsCoordinate({ 0, 1000 });
	Geom::Point x0 = ToWindowsCoordinate({ -1000, 0 }); Geom::Point x1 = ToWindowsCoordinate({ 1000, 0 });

	SDL_RenderDrawLine(m_renderer, x0.x, x0.y, x1.x, x1.y);
	SDL_RenderDrawLine(m_renderer, y0.x, y0.y, y1.x, y1.y);

}

Geom::Vector DrawWindow::ToWindowsCoordinate(Geom::Vector v) const
{
	return { v.x * m_Zoom * DRAW_SCALE + SHIFTX + m_CameraPos.x, v.y * m_Zoom * DRAW_SCALE + SHIFTY + m_CameraPos.y };
}

void DrawWindow::CameraMovementEvents(const SDL_Event& event)
{
	switch (event.key.keysym.sym)
	{
	case SDLK_SPACE: // reset cammera
		m_CameraPos.x = 0.f; m_CameraPos.y = 0.f;
		m_Zoom = 1.f;
		break;
	case SDLK_a:
		m_CameraPos.x -= 10;
		break;
	case SDLK_d:
		m_CameraPos.x += 10;
		break;
	case SDLK_w:
		m_CameraPos.y -= 10;
		break;
	case SDLK_s:
		m_CameraPos.y += 10;
		break;
	case SDLK_z:
		m_Zoom += 0.1;
		break;
	case SDLK_x:
		m_Zoom -= 0.1;
		break;
		
	}

}



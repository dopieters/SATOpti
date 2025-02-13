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



void DrawWindow::DrawPolygon(const Polygon& A) const {
	const int nVert = A.vertices.size();

	if (nVert < 3) return;

	for (int ii = 0; ii < nVert - 1; ++ii) {
		SDL_RenderDrawLine(m_renderer, A.vertices[ii].x * DRAW_SCALE + SHIFTX,
			A.vertices[ii].y * DRAW_SCALE + SHIFTY,
			A.vertices[ii + 1].x * DRAW_SCALE + SHIFTX,
			A.vertices[ii + 1].y * DRAW_SCALE + SHIFTY);
	}
	SDL_RenderDrawLine(m_renderer, A.vertices[nVert - 1].x * DRAW_SCALE + SHIFTX,
		A.vertices[nVert - 1].y * DRAW_SCALE + SHIFTY,
		A.vertices[0].x * DRAW_SCALE + SHIFTX,
		A.vertices[0].y * DRAW_SCALE + SHIFTY);
}

void DrawWindow::DrawHyperPlanes(const Vector v, const float min, const float max) const
{
	Vector barAxisPerp = { -v.y, v.x };

	Point Pmax = max * v;
	Point P1 = Pmax + 100 * barAxisPerp;
	Point P2 = Pmax - 100 * barAxisPerp;

	// first plane
	SDL_RenderDrawLine(m_renderer, P1.x * DRAW_SCALE + SHIFTX,
		P1.y * DRAW_SCALE + SHIFTY,
		P2.x * DRAW_SCALE + SHIFTX,
		P2.y * DRAW_SCALE + SHIFTY);

	Point Pmin = min * v;
	P1 = Pmin + 100 * barAxisPerp;
	P2 = Pmin - 100 * barAxisPerp;

	// first plane
	SDL_RenderDrawLine(m_renderer, P1.x * DRAW_SCALE + SHIFTX,
		P1.y * DRAW_SCALE + SHIFTY,
		P2.x * DRAW_SCALE + SHIFTX,
		P2.y * DRAW_SCALE + SHIFTY);
}



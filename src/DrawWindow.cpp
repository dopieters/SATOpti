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


